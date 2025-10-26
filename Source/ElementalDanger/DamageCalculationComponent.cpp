// Damage Calculation Component Implementation

#include "DamageCalculationComponent.h"

UDamageCalculationComponent::UDamageCalculationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	WeaknessMultiplier = 1.5f;
	LevelScalingFactor = 0.05f;
	WisdomScalingFactor = 0.02f;
	bDualElementIgnoresWeakness = true;
}

void UDamageCalculationComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeElementalOpposites();
}

// ============================================
// Elemental Weakness System
// ============================================

void UDamageCalculationComponent::InitializeElementalOpposites()
{
	// Define elemental opposite pairs
	// Fire <-> Ice (hot vs cold temperature extremes)
	ElementalOpposites.Add(EMagicType::Fire, EMagicType::Ice);
	ElementalOpposites.Add(EMagicType::Ice, EMagicType::Fire);

	// Water <-> Lightning (water conducts electricity)
	ElementalOpposites.Add(EMagicType::Water, EMagicType::Lightning);
	ElementalOpposites.Add(EMagicType::Lightning, EMagicType::Water);

	// Earth <-> Air (ground vs sky)
	ElementalOpposites.Add(EMagicType::Earth, EMagicType::Air);
	ElementalOpposites.Add(EMagicType::Air, EMagicType::Earth);

	// Light <-> Dark (classic good vs evil, purification vs corruption)
	ElementalOpposites.Add(EMagicType::Light, EMagicType::Dark);
	ElementalOpposites.Add(EMagicType::Dark, EMagicType::Light);

	// Poison <-> Light (purification destroys toxins)
	ElementalOpposites.Add(EMagicType::Poison, EMagicType::Light);
	// Note: Light already has Dark as opposite, so Poison is weak to Light but Light's main opposite is Dark

	UE_LOG(LogTemp, Log, TEXT("Elemental opposites initialized: %d pairs"), ElementalOpposites.Num());
}

bool UDamageCalculationComponent::IsElementWeakTo(EMagicType AttackElement, EMagicType DefenderElement) const
{
	// Check if attacker's element is the defender's weakness
	const EMagicType* OppositeElement = ElementalOpposites.Find(DefenderElement);
	if (OppositeElement && *OppositeElement == AttackElement)
	{
		return true;
	}

	return false;
}

EMagicType UDamageCalculationComponent::GetOppositeElement(EMagicType Element) const
{
	const EMagicType* Opposite = ElementalOpposites.Find(Element);
	if (Opposite)
	{
		return *Opposite;
	}

	return EMagicType::Fire; // Default fallback
}

bool UDamageCalculationComponent::HasOppositeElement(EMagicType Element) const
{
	return ElementalOpposites.Contains(Element);
}

// ============================================
// Damage Calculation
// ============================================

FDamageCalculationResult UDamageCalculationComponent::CalculateDamage(
	float BaseDamage,
	EMagicType AttackElement,
	EMagicType DefenderElement,
	int32 AttackerLevel,
	int32 AttackerWisdom,
	bool bIsDualElement) const
{
	FDamageCalculationResult Result;
	Result.BaseDamage = BaseDamage;
	Result.bIsDualElement = bIsDualElement;

	// Get elemental multiplier
	Result.ElementalMultiplier = GetElementalMultiplier(AttackElement, DefenderElement, bIsDualElement);
	Result.bIsWeakness = (Result.ElementalMultiplier > 1.0f);

	// Get level scaling
	Result.LevelScaling = GetLevelScalingMultiplier(AttackerLevel);

	// Get wisdom scaling
	Result.WisdomScaling = GetWisdomScalingMultiplier(AttackerWisdom);

	// Calculate final damage
	Result.FinalDamage = BaseDamage * Result.ElementalMultiplier * Result.LevelScaling * Result.WisdomScaling;

	// Generate breakdown string
	Result.DamageBreakdown = GetDamageBreakdownString(Result);

	return Result;
}

float UDamageCalculationComponent::CalculateSimpleDamage(
	float BaseDamage,
	EMagicType AttackElement,
	EMagicType DefenderElement) const
{
	float ElementalMultiplier = GetElementalMultiplier(AttackElement, DefenderElement, false);
	return BaseDamage * ElementalMultiplier;
}

float UDamageCalculationComponent::GetLevelScalingMultiplier(int32 Level) const
{
	// 1.0 + (Level * 0.05)
	// Level 1 = 1.05x
	// Level 10 = 1.5x
	// Level 20 = 2.0x
	return 1.0f + (Level * LevelScalingFactor);
}

float UDamageCalculationComponent::GetWisdomScalingMultiplier(int32 Wisdom) const
{
	// 1.0 + (Wisdom * 0.02)
	// Wisdom 10 = 1.2x
	// Wisdom 50 = 2.0x
	// Wisdom 100 = 3.0x
	return 1.0f + (Wisdom * WisdomScalingFactor);
}

float UDamageCalculationComponent::GetElementalMultiplier(EMagicType AttackElement, EMagicType DefenderElement, bool bIsDualElement) const
{
	// Dual element attacks ignore weaknesses per design
	if (bIsDualElement && bDualElementIgnoresWeakness)
	{
		return 1.0f;
	}

	// Check if this is a weakness matchup
	if (IsElementWeakTo(AttackElement, DefenderElement))
	{
		return WeaknessMultiplier; // 1.5x by default
	}

	// No weakness, normal damage
	return 1.0f;
}

// ============================================
// Utility Functions
// ============================================

FString UDamageCalculationComponent::GetDamageBreakdownString(const FDamageCalculationResult& Result) const
{
	FString Breakdown = FString::Printf(
		TEXT("Base: %.1f | Element: %.2fx | Level: %.2fx | Wisdom: %.2fx | Final: %.1f"),
		Result.BaseDamage,
		Result.ElementalMultiplier,
		Result.LevelScaling,
		Result.WisdomScaling,
		Result.FinalDamage
	);

	if (Result.bIsWeakness)
	{
		Breakdown += TEXT(" [WEAKNESS!]");
	}

	if (Result.bIsDualElement)
	{
		Breakdown += TEXT(" [DUAL ELEMENT]");
	}

	return Breakdown;
}

bool UDamageCalculationComponent::IsCriticalHit(float CritChance) const
{
	float RandomValue = FMath::FRand(); // 0.0 to 1.0
	return RandomValue <= CritChance;
}

float UDamageCalculationComponent::ApplyCriticalMultiplier(float Damage, float CritMultiplier) const
{
	return Damage * CritMultiplier;
}
