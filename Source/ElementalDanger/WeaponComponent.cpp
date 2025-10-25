// Weapon Component Implementation

#include "WeaponComponent.h"
#include "NinjaWizardCharacter.h"
#include "MasteryManagerComponent.h"

UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentWeaponStyle = EWeaponStyle::None;
	CurrentComboCount = 0;
	ComboResetTime = 2.0f;
	TimeSinceLastAttack = 0.0f;
	InfusedElement = EMagicElement::None;
	InfusedCombinedMagic = ECombinedMagic::None;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ANinjaWizardCharacter>(GetOwner());
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Handle combo reset timer
	TimeSinceLastAttack += DeltaTime;
	if (TimeSinceLastAttack >= ComboResetTime)
	{
		ResetCombo();
	}
}

// ============================================
// Combat
// ============================================

bool UWeaponComponent::PerformAttack(EWeaponStyle WeaponStyle)
{
	if (!OwnerCharacter)
	{
		return false;
	}

	// Find appropriate attack for this weapon style
	FWeaponAttackData AttackData;
	bool bFoundAttack = false;

	for (const FWeaponAttackData& Attack : LearnedAttacks)
	{
		if (Attack.WeaponStyle == WeaponStyle && Attack.ComboIndex == CurrentComboCount)
		{
			AttackData = Attack;
			bFoundAttack = true;
			break;
		}
	}

	if (!bFoundAttack)
	{
		// Use basic attack
		AttackData.WeaponStyle = WeaponStyle;
		AttackData.AttackName = FName("BasicAttack");
	}

	if (!CanPerformAttack(AttackData))
	{
		return false;
	}

	// Calculate stamina cost
	float StaminaCost = CalculateStaminaCost(AttackData);

	// Consume stamina
	if (!OwnerCharacter->ConsumeStamina(StaminaCost))
	{
		return false;
	}

	// Perform attack
	OnAttackPerformed(AttackData);

	// Grant mastery experience
	if (OwnerCharacter->MasteryManager)
	{
		OwnerCharacter->MasteryManager->AddWeaponExperience(WeaponStyle, 5.0f);
	}

	// Increment combo
	IncrementCombo();
	TimeSinceLastAttack = 0.0f;

	return true;
}

bool UWeaponComponent::CanPerformAttack(const FWeaponAttackData& AttackData) const
{
	if (!OwnerCharacter)
	{
		return false;
	}

	// Check stamina
	float StaminaCost = CalculateStaminaCost(AttackData);
	if (OwnerCharacter->CurrentStamina < StaminaCost)
	{
		return false;
	}

	// Check mastery requirement
	if (OwnerCharacter->MasteryManager)
	{
		int32 WeaponMastery = OwnerCharacter->MasteryManager->GetWeaponMastery(AttackData.WeaponStyle);
		if (WeaponMastery < AttackData.RequiredMastery)
		{
			return false;
		}
	}

	return true;
}

void UWeaponComponent::EquipWeapon(EWeaponStyle WeaponStyle)
{
	if (!HasWeaponStyle(WeaponStyle))
	{
		return;
	}

	CurrentWeaponStyle = WeaponStyle;
	ResetCombo();
	OnWeaponEquipped(WeaponStyle);
}

void UWeaponComponent::UnlockWeaponStyle(EWeaponStyle WeaponStyle)
{
	if (!UnlockedWeaponStyles.Contains(WeaponStyle))
	{
		UnlockedWeaponStyles.Add(WeaponStyle);
	}
}

bool UWeaponComponent::HasWeaponStyle(EWeaponStyle WeaponStyle) const
{
	return UnlockedWeaponStyles.Contains(WeaponStyle);
}

void UWeaponComponent::LearnAttack(const FWeaponAttackData& AttackData)
{
	if (!LearnedAttacks.Contains(AttackData))
	{
		LearnedAttacks.Add(AttackData);
	}
}

// ============================================
// Combo System
// ============================================

void UWeaponComponent::ResetCombo()
{
	if (CurrentComboCount > 0)
	{
		CurrentComboCount = 0;
	}
}

void UWeaponComponent::IncrementCombo()
{
	CurrentComboCount++;
	OnComboIncremented(CurrentComboCount);
}

// ============================================
// Magic Infusion
// ============================================

void UWeaponComponent::InfuseWeapon(EMagicElement Element)
{
	InfusedElement = Element;
	InfusedCombinedMagic = ECombinedMagic::None;
	OnWeaponInfused(Element);
}

void UWeaponComponent::InfuseWeaponWithCombination(ECombinedMagic CombinedMagic)
{
	InfusedCombinedMagic = CombinedMagic;
	InfusedElement = EMagicElement::None;
}

void UWeaponComponent::ClearInfusion()
{
	InfusedElement = EMagicElement::None;
	InfusedCombinedMagic = ECombinedMagic::None;
}

float UWeaponComponent::GetInfusedDamageBonus() const
{
	if (InfusedElement != EMagicElement::None)
	{
		return 0.25f; // 25% damage bonus for single element infusion
	}
	else if (InfusedCombinedMagic != ECombinedMagic::None)
	{
		return 0.5f; // 50% damage bonus for combined magic infusion
	}
	return 0.0f;
}

// ============================================
// Helper Functions
// ============================================

float UWeaponComponent::CalculateStaminaCost(const FWeaponAttackData& AttackData) const
{
	if (!OwnerCharacter || !OwnerCharacter->MasteryManager)
	{
		return AttackData.StaminaCost;
	}

	float BaseCost = AttackData.StaminaCost;

	// Apply mastery discount (higher mastery = lower cost)
	int32 Mastery = OwnerCharacter->MasteryManager->GetWeaponMastery(AttackData.WeaponStyle);
	float DiscountPercent = FMath::Clamp(Mastery / 100.0f, 0.0f, 0.4f); // Max 40% discount at 100 mastery
	BaseCost *= (1.0f - DiscountPercent);

	return FMath::Max(BaseCost, 1.0f); // Minimum 1 stamina
}

float UWeaponComponent::CalculateDamage(const FWeaponAttackData& AttackData) const
{
	float BaseDamage = AttackData.BaseDamage;

	// Apply mastery bonus
	if (OwnerCharacter && OwnerCharacter->MasteryManager)
	{
		int32 Mastery = OwnerCharacter->MasteryManager->GetWeaponMastery(AttackData.WeaponStyle);
		float MasteryBonus = Mastery * 0.5f; // 0.5% damage per mastery level
		BaseDamage *= (1.0f + (MasteryBonus / 100.0f));
	}

	// Apply combo bonus
	float ComboBonus = CurrentComboCount * 0.1f; // 10% per combo hit
	BaseDamage *= (1.0f + ComboBonus);

	// Apply infusion bonus
	BaseDamage *= (1.0f + GetInfusedDamageBonus());

	return BaseDamage;
}
