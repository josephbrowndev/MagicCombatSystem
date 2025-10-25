// Player Attribute Component Implementation

#include "PlayerAttributeComponent.h"
#include "NinjaWizardCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UPlayerAttributeComponent::UPlayerAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// Set default attribute values
	Strength.BaseValue = 10;
	Wisdom.BaseValue = 10;
	Perception.BaseValue = 10;
	Agility.BaseValue = 10;

	bInSlowMotion = false;
}

void UPlayerAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ANinjaWizardCharacter>(GetOwner());
	RecalculateDerivedStats();
	ApplyAttributeEffects();
}

void UPlayerAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ============================================
// Attribute Functions
// ============================================

int32 UPlayerAttributeComponent::GetAttributeValue(EPlayerAttribute Attribute) const
{
	switch (Attribute)
	{
		case EPlayerAttribute::Strength:
			return Strength.GetTotalValue();
		case EPlayerAttribute::Wisdom:
			return Wisdom.GetTotalValue();
		case EPlayerAttribute::Perception:
			return Perception.GetTotalValue();
		case EPlayerAttribute::Agility:
			return Agility.GetTotalValue();
		default:
			return 0;
	}
}

void UPlayerAttributeComponent::SetAttributeBaseValue(EPlayerAttribute Attribute, int32 Value)
{
	switch (Attribute)
	{
		case EPlayerAttribute::Strength:
			Strength.BaseValue = Value;
			break;
		case EPlayerAttribute::Wisdom:
			Wisdom.BaseValue = Value;
			break;
		case EPlayerAttribute::Perception:
			Perception.BaseValue = Value;
			break;
		case EPlayerAttribute::Agility:
			Agility.BaseValue = Value;
			break;
	}

	RecalculateDerivedStats();
	OnAttributeChanged(Attribute, GetAttributeValue(Attribute));
}

void UPlayerAttributeComponent::AddAttributeBonus(EPlayerAttribute Attribute, int32 Bonus)
{
	switch (Attribute)
	{
		case EPlayerAttribute::Strength:
			Strength.BonusValue += Bonus;
			break;
		case EPlayerAttribute::Wisdom:
			Wisdom.BonusValue += Bonus;
			break;
		case EPlayerAttribute::Perception:
			Perception.BonusValue += Bonus;
			break;
		case EPlayerAttribute::Agility:
			Agility.BonusValue += Bonus;
			break;
	}

	RecalculateDerivedStats();
	OnAttributeChanged(Attribute, GetAttributeValue(Attribute));
}

void UPlayerAttributeComponent::SetAttributeMultiplier(EPlayerAttribute Attribute, float Multiplier)
{
	switch (Attribute)
	{
		case EPlayerAttribute::Strength:
			Strength.Multiplier = Multiplier;
			break;
		case EPlayerAttribute::Wisdom:
			Wisdom.Multiplier = Multiplier;
			break;
		case EPlayerAttribute::Perception:
			Perception.Multiplier = Multiplier;
			break;
		case EPlayerAttribute::Agility:
			Agility.Multiplier = Multiplier;
			break;
	}

	RecalculateDerivedStats();
	OnAttributeChanged(Attribute, GetAttributeValue(Attribute));
}

void UPlayerAttributeComponent::IncreaseAttribute(EPlayerAttribute Attribute, int32 Amount)
{
	switch (Attribute)
	{
		case EPlayerAttribute::Strength:
			Strength.BaseValue += Amount;
			break;
		case EPlayerAttribute::Wisdom:
			Wisdom.BaseValue += Amount;
			break;
		case EPlayerAttribute::Perception:
			Perception.BaseValue += Amount;
			break;
		case EPlayerAttribute::Agility:
			Agility.BaseValue += Amount;
			break;
	}

	RecalculateDerivedStats();
	ApplyAttributeEffects();
	OnAttributeChanged(Attribute, GetAttributeValue(Attribute));
}

// ============================================
// Derived Stat Calculation
// ============================================

void UPlayerAttributeComponent::RecalculateDerivedStats()
{
	CalculateStrengthDerivedStats();
	CalculateWisdomDerivedStats();
	CalculatePerceptionDerivedStats();
	CalculateAgilityDerivedStats();

	OnDerivedStatsRecalculated(DerivedStats);
}

void UPlayerAttributeComponent::CalculateStrengthDerivedStats()
{
	int32 STR = Strength.GetTotalValue();

	// Block Chance: 0.5% per STR point, max 50%
	DerivedStats.BlockChance = FMath::Min(STR * 0.5f, 50.0f);

	// Weapon Damage: +2% per STR point
	DerivedStats.WeaponDamageMultiplier = 1.0f + (STR * 0.02f);

	// Max Stamina: +5 per STR point
	DerivedStats.MaxStaminaBonus = STR * 5.0f;

	// Max Health: +10 per STR point
	DerivedStats.MaxHealthBonus = STR * 10.0f;

	// Carry Weight: +5 per STR point
	DerivedStats.CarryWeightBonus = STR * 5.0f;

	// Knockback Resistance: +1% per STR point, max 75%
	DerivedStats.KnockbackResistance = FMath::Min(STR * 1.0f, 75.0f);
}

void UPlayerAttributeComponent::CalculateWisdomDerivedStats()
{
	int32 WIS = Wisdom.GetTotalValue();

	// Magical Damage: +3% per WIS point
	DerivedStats.MagicalDamageMultiplier = 1.0f + (WIS * 0.03f);

	// Spell Size: +2% per WIS point
	DerivedStats.SpellSizeMultiplier = 1.0f + (WIS * 0.02f);

	// Max Summon Count: 1 + (WIS / 15), max 10
	DerivedStats.MaxSummonCount = FMath::Min(1 + (WIS / 15), 10);

	// Summon Capacity: WIS / 10, min 1
	DerivedStats.SummonCapacity = FMath::Max(WIS / 10, 1);

	// Max Mana: +8 per WIS point
	DerivedStats.MaxManaBonus = WIS * 8.0f;

	// Mana Regen: +0.3 per WIS point
	DerivedStats.ManaRegenBonus = WIS * 0.3f;

	// Cooldown Reduction: +0.5% per WIS point, max 40%
	DerivedStats.SpellCooldownReduction = FMath::Min(WIS * 0.5f, 40.0f);
}

void UPlayerAttributeComponent::CalculatePerceptionDerivedStats()
{
	int32 PER = Perception.GetTotalValue();

	// Danger Sense Range: 50 units per PER point
	DerivedStats.DangerSenseRange = PER * 50.0f;

	// Level Difference Detection: Can see levels +/- PER
	DerivedStats.LevelDifferenceDetection = PER;

	// Slow Motion Chance: 1% per PER point, max 50%
	DerivedStats.SlowMotionChance = FMath::Min(PER * 1.0f, 50.0f);

	// Slow Motion Duration: 0.1 seconds per PER point
	DerivedStats.SlowMotionDuration = PER * 0.1f;

	// Threat Detection Range: Base 1000 + 100 per PER point
	DerivedStats.ThreatDetectionRange = 1000.0f + (PER * 100.0f);

	// Critical Hit Chance: 1% per PER point, max 50%
	DerivedStats.CriticalHitChance = FMath::Min(PER * 1.0f, 50.0f);

	// Stealth Detection: +5% per PER point
	DerivedStats.StealthDetectionBonus = PER * 5.0f;
}

void UPlayerAttributeComponent::CalculateAgilityDerivedStats()
{
	int32 AGI = Agility.GetTotalValue();

	// Attack Speed: +2% per AGI point
	DerivedStats.AttackSpeedMultiplier = 1.0f + (AGI * 0.02f);

	// Movement Speed: +1.5% per AGI point
	DerivedStats.MovementSpeedMultiplier = 1.0f + (AGI * 0.015f);

	// Dodge Chance: 0.8% per AGI point, max 50%
	DerivedStats.DodgeChance = FMath::Min(AGI * 0.8f, 50.0f);

	// Jump Height: +1% per AGI point
	DerivedStats.JumpHeightMultiplier = 1.0f + (AGI * 0.01f);

	// Fall Damage Reduction: 2% per AGI point, max 80%
	DerivedStats.FallDamageReduction = FMath::Min(AGI * 2.0f, 80.0f);

	// Stamina Regen: +0.4 per AGI point
	DerivedStats.StaminaRegenBonus = AGI * 0.4f;

	// Evasion Rating: +2 per AGI point (used for calculations)
	DerivedStats.EvasionRating = AGI * 2.0f;
}

// ============================================
// Stat Application to Character
// ============================================

void UPlayerAttributeComponent::ApplyAttributeEffects()
{
	if (!OwnerCharacter)
	{
		return;
	}

	// Apply Strength effects
	OwnerCharacter->MaxStamina = 100.0f + DerivedStats.MaxStaminaBonus;
	// TODO: Apply MaxHealth when health system is implemented

	// Apply Wisdom effects
	OwnerCharacter->MaxMana = 100.0f + DerivedStats.MaxManaBonus;
	OwnerCharacter->ManaRegenRate = 5.0f + DerivedStats.ManaRegenBonus;

	// Apply Agility effects
	if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = 600.0f * DerivedStats.MovementSpeedMultiplier;
		Movement->JumpZVelocity = 700.0f * DerivedStats.JumpHeightMultiplier;
	}

	OwnerCharacter->StaminaRegenRate = 10.0f + DerivedStats.StaminaRegenBonus;
}

// ============================================
// Perception-Specific Functions
// ============================================

bool UPlayerAttributeComponent::CanDetectThreat(AActor* PotentialThreat) const
{
	if (!OwnerCharacter || !PotentialThreat)
	{
		return false;
	}

	float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), PotentialThreat->GetActorLocation());
	return Distance <= DerivedStats.ThreatDetectionRange;
}

void UPlayerAttributeComponent::TriggerSlowMotion()
{
	if (bInSlowMotion)
	{
		return;
	}

	// Check if slow motion triggers based on perception
	float RandomValue = FMath::FRandRange(0.0f, 100.0f);
	if (RandomValue > DerivedStats.SlowMotionChance)
	{
		return;
	}

	bInSlowMotion = true;

	// Set game to slow motion
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.3f);

	OnSlowMotionTriggered();

	// Set timer to end slow motion
	GetWorld()->GetTimerManager().SetTimer(
		SlowMotionTimerHandle,
		this,
		&UPlayerAttributeComponent::EndSlowMotion,
		DerivedStats.SlowMotionDuration,
		false
	);
}

void UPlayerAttributeComponent::EndSlowMotion()
{
	if (!bInSlowMotion)
	{
		return;
	}

	bInSlowMotion = false;

	// Restore normal time
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

	OnSlowMotionEnded();

	GetWorld()->GetTimerManager().ClearTimer(SlowMotionTimerHandle);
}
