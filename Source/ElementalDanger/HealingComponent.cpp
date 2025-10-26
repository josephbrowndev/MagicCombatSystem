// Healing Component Implementation

#include "HealingComponent.h"
#include "InventoryComponent.h"
#include "SafeZoneVolume.h"
#include "NinjaWizardCharacter.h"

UHealingComponent::UHealingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Set default healing restrictions
	HealingRestrictions.bAllowNaturalRegeneration = false; // NO natural regen!
	HealingRestrictions.MinHealthPercentForPotions = 0.1f; // 10% minimum
	HealingRestrictions.AllowedHealingSources.Add(EHealingSource::SafeZone);
	HealingRestrictions.AllowedHealingSources.Add(EHealingSource::ClericNPC);
	HealingRestrictions.AllowedHealingSources.Add(EHealingSource::ClericSummon);
	HealingRestrictions.AllowedHealingSources.Add(EHealingSource::Potion);

	SafeZoneHealRate = 5.0f;
	ClericHealRate = 10.0f;
	OutOfCombatRegenRate = 2.0f;
	OutOfCombatDelay = 8.0f;

	bIsInSafeZone = false;
	bIsBeingHealedByCleric = false;
	bIsInCombat = false;
	TimeSinceLastCombat = 0.0f;
	CurrentClericHealer = nullptr;
	CurrentSafeZone = nullptr;
}

void UHealingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner();
	if (OwnerCharacter)
	{
		InventoryComponent = OwnerCharacter->FindComponentByClass<UInventoryComponent>();
	}
}

void UHealingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Track time since last combat
	if (!bIsInCombat)
	{
		TimeSinceLastCombat += DeltaTime;
	}

	// Process safe zone healing
	if (bIsInSafeZone)
	{
		ProcessSafeZoneHealing(DeltaTime);
	}

	// Process cleric healing
	if (bIsBeingHealedByCleric)
	{
		ProcessClericHealing(DeltaTime);
	}

	// Process out-of-combat regeneration
	if (CanStartOutOfCombatRegen())
	{
		ProcessOutOfCombatRegen(DeltaTime);
	}
}

// ============================================
// Healing Functions
// ============================================

bool UHealingComponent::CanHeal(EHealingSource Source, float CurrentHealthPercent) const
{
	// Check if healing source is allowed
	if (!IsHealingSourceAllowed(Source))
	{
		return false;
	}

	// Special check for potions - must be above 10% health
	if (Source == EHealingSource::Potion)
	{
		if (CurrentHealthPercent < HealingRestrictions.MinHealthPercentForPotions)
		{
			return false;
		}
	}

	// Can't heal if already at full health
	if (CurrentHealthPercent >= 1.0f)
	{
		return false;
	}

	return true;
}

float UHealingComponent::ApplyHealing(float HealAmount, EHealingSource Source)
{
	float CurrentHealthPercent = GetCurrentHealthPercent();

	if (!CanHeal(Source, CurrentHealthPercent))
	{
		return 0.0f;
	}

	// Apply the healing
	HealCharacter(HealAmount);

	OnHealingReceived(HealAmount, Source);

	return HealAmount;
}

bool UHealingComponent::TryUsePotion(FName PotionID)
{
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("No inventory component found for healing!"));
		return false;
	}

	float CurrentHealthPercent = GetCurrentHealthPercent();

	// Check if we can use potions at this health level
	if (CurrentHealthPercent < HealingRestrictions.MinHealthPercentForPotions)
	{
		OnPotionRejected(PotionID, CurrentHealthPercent);
		UE_LOG(LogTemp, Warning, TEXT("Cannot use potion - health too low! Must be above 10%%"));
		return false;
	}

	// Check if we can use the potion
	if (!InventoryComponent->CanUsePotion(PotionID, CurrentHealthPercent))
	{
		return false;
	}

	// Get the potion data
	FPotionData* Potion = InventoryComponent->FindPotionByID(PotionID);
	if (!Potion)
	{
		return false;
	}

	// Apply healing
	float HealAmount = ApplyHealing(Potion->HealAmount, EHealingSource::Potion);
	if (HealAmount > 0.0f)
	{
		// Consume the potion
		InventoryComponent->UsePotion(PotionID);
		return true;
	}

	return false;
}

// ============================================
// Safe Zone Healing
// ============================================

void UHealingComponent::EnterSafeZone(ASafeZoneVolume* SafeZone)
{
	if (!SafeZone) return;

	bIsInSafeZone = true;
	CurrentSafeZone = SafeZone;

	OnEnteredSafeZone(SafeZone);

	UE_LOG(LogTemp, Log, TEXT("Entered safe zone - healing enabled"));
}

void UHealingComponent::ExitSafeZone()
{
	if (!bIsInSafeZone) return;

	bIsInSafeZone = false;
	CurrentSafeZone = nullptr;

	OnExitedSafeZone();

	UE_LOG(LogTemp, Log, TEXT("Exited safe zone - healing stopped"));
}

void UHealingComponent::ProcessSafeZoneHealing(float DeltaTime)
{
	if (!CurrentSafeZone) return;

	float HealAmount = SafeZoneHealRate * DeltaTime;
	ApplyHealing(HealAmount, EHealingSource::SafeZone);
}

// ============================================
// Cleric Healing
// ============================================

void UHealingComponent::StartClericHealing(AActor* ClericHealer)
{
	if (!ClericHealer) return;

	bIsBeingHealedByCleric = true;
	CurrentClericHealer = ClericHealer;

	OnClericHealingStarted(ClericHealer);

	UE_LOG(LogTemp, Log, TEXT("Cleric healing started"));
}

void UHealingComponent::StopClericHealing()
{
	if (!bIsBeingHealedByCleric) return;

	bIsBeingHealedByCleric = false;
	CurrentClericHealer = nullptr;

	OnClericHealingStopped();

	UE_LOG(LogTemp, Log, TEXT("Cleric healing stopped"));
}

void UHealingComponent::ProcessClericHealing(float DeltaTime)
{
	if (!CurrentClericHealer) return;

	// Check if cleric is still in range (you may want to add distance check here)
	float HealAmount = ClericHealRate * DeltaTime;

	// Determine if this is NPC or summon cleric
	// For now, we'll use ClericNPC - you can add logic to distinguish
	ApplyHealing(HealAmount, EHealingSource::ClericNPC);
}

// ============================================
// Combat State Management
// ============================================

void UHealingComponent::EnterCombat()
{
	if (bIsInCombat) return;

	bIsInCombat = true;
	TimeSinceLastCombat = 0.0f;

	OnCombatStateChanged(true);

	UE_LOG(LogTemp, Log, TEXT("Entered combat - regeneration stopped"));
}

void UHealingComponent::ExitCombat()
{
	if (!bIsInCombat) return;

	bIsInCombat = false;
	TimeSinceLastCombat = 0.0f;

	OnCombatStateChanged(false);

	UE_LOG(LogTemp, Log, TEXT("Exited combat - regeneration will start in %.1f seconds"), OutOfCombatDelay);
}

bool UHealingComponent::CanStartOutOfCombatRegen() const
{
	if (bIsInCombat) return false;
	if (TimeSinceLastCombat < OutOfCombatDelay) return false;
	if (GetCurrentHealthPercent() >= 1.0f) return false;

	return true;
}

void UHealingComponent::ProcessOutOfCombatRegen(float DeltaTime)
{
	static bool bRegenStarted = false;

	if (!bRegenStarted)
	{
		bRegenStarted = true;
		OnOutOfCombatRegenStarted();
		UE_LOG(LogTemp, Log, TEXT("Out-of-combat regeneration started"));
	}

	float HealAmount = OutOfCombatRegenRate * DeltaTime;
	HealCharacter(HealAmount);

	// If we reach full health, stop regen visually
	if (GetCurrentHealthPercent() >= 1.0f)
	{
		bRegenStarted = false;
		OnOutOfCombatRegenStopped();
	}
}

// ============================================
// Healing Validation
// ============================================

bool UHealingComponent::IsHealingSourceAllowed(EHealingSource Source) const
{
	return HealingRestrictions.AllowedHealingSources.Contains(Source);
}

float UHealingComponent::GetCurrentHealthPercent() const
{
	float MaxHP = GetMaxHealth();
	if (MaxHP <= 0.0f) return 0.0f;

	float CurrentHP = GetCurrentHealth();
	return CurrentHP / MaxHP;
}

bool UHealingComponent::CanUsePotions(float CurrentHealthPercent) const
{
	return CurrentHealthPercent >= HealingRestrictions.MinHealthPercentForPotions;
}

// ============================================
// Helper Functions
// ============================================

float UHealingComponent::GetMaxHealth() const
{
	if (ANinjaWizardCharacter* Character = Cast<ANinjaWizardCharacter>(OwnerCharacter))
	{
		// Assuming you have a max health property
		// You'll need to add this to NinjaWizardCharacter or use your existing health system
		return 100.0f; // Placeholder - replace with actual max health
	}
	return 100.0f;
}

float UHealingComponent::GetCurrentHealth() const
{
	if (ANinjaWizardCharacter* Character = Cast<ANinjaWizardCharacter>(OwnerCharacter))
	{
		// You'll need to add this to NinjaWizardCharacter or use your existing health system
		return 100.0f; // Placeholder - replace with actual current health
	}
	return 100.0f;
}

void UHealingComponent::HealCharacter(float Amount)
{
	if (ANinjaWizardCharacter* Character = Cast<ANinjaWizardCharacter>(OwnerCharacter))
	{
		// TODO: Implement actual healing
		// Character->CurrentHealth = FMath::Min(Character->CurrentHealth + Amount, Character->MaxHealth);
		UE_LOG(LogTemp, Log, TEXT("Healed for %.2f HP"), Amount);
	}
}
