// Mastery Manager Component Implementation

#include "MasteryManagerComponent.h"

UMasteryManagerComponent::UMasteryManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMasteryManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeMasteryMaps();
}

void UMasteryManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ============================================
// Magic Mastery
// ============================================

int32 UMasteryManagerComponent::GetMagicMastery(EMagicElement Element) const
{
	if (const FMasteryData* MasteryData = MagicMasteryMap.Find(Element))
	{
		return MasteryData->Level;
	}
	return 0;
}

void UMasteryManagerComponent::AddMagicExperience(EMagicElement Element, float Experience)
{
	if (Element == EMagicElement::None)
	{
		return;
	}

	FMasteryData* MasteryData = MagicMasteryMap.Find(Element);
	if (!MasteryData)
	{
		FMasteryData NewMastery;
		MagicMasteryMap.Add(Element, NewMastery);
		MasteryData = MagicMasteryMap.Find(Element);
	}

	if (MasteryData && MasteryData->Level < 100)
	{
		MasteryData->Experience += Experience;

		// Check for level up
		while (MasteryData->Experience >= MasteryData->ExperienceToNextLevel && MasteryData->Level < 100)
		{
			MasteryData->Experience -= MasteryData->ExperienceToNextLevel;
			MasteryData->Level++;
			MasteryData->ExperienceToNextLevel = CalculateExperienceForNextLevel(MasteryData->Level);

			ProcessLevelUp(Element, *MasteryData);
		}
	}
}

void UMasteryManagerComponent::SetMagicMastery(EMagicElement Element, int32 Level)
{
	FMasteryData MasteryData;
	MasteryData.Level = FMath::Clamp(Level, 0, 100);
	MasteryData.Experience = 0.0f;
	MasteryData.ExperienceToNextLevel = CalculateExperienceForNextLevel(MasteryData.Level);

	MagicMasteryMap.Add(Element, MasteryData);
}

float UMasteryManagerComponent::GetMagicExperiencePercent(EMagicElement Element) const
{
	if (const FMasteryData* MasteryData = MagicMasteryMap.Find(Element))
	{
		if (MasteryData->ExperienceToNextLevel > 0)
		{
			return MasteryData->Experience / MasteryData->ExperienceToNextLevel;
		}
	}
	return 0.0f;
}

// ============================================
// Weapon Mastery
// ============================================

int32 UMasteryManagerComponent::GetWeaponMastery(EWeaponStyle WeaponStyle) const
{
	if (const FMasteryData* MasteryData = WeaponMasteryMap.Find(WeaponStyle))
	{
		return MasteryData->Level;
	}
	return 0;
}

void UMasteryManagerComponent::AddWeaponExperience(EWeaponStyle WeaponStyle, float Experience)
{
	if (WeaponStyle == EWeaponStyle::None)
	{
		return;
	}

	FMasteryData* MasteryData = WeaponMasteryMap.Find(WeaponStyle);
	if (!MasteryData)
	{
		FMasteryData NewMastery;
		WeaponMasteryMap.Add(WeaponStyle, NewMastery);
		MasteryData = WeaponMasteryMap.Find(WeaponStyle);
	}

	if (MasteryData && MasteryData->Level < 100)
	{
		MasteryData->Experience += Experience;

		// Check for level up
		while (MasteryData->Experience >= MasteryData->ExperienceToNextLevel && MasteryData->Level < 100)
		{
			MasteryData->Experience -= MasteryData->ExperienceToNextLevel;
			MasteryData->Level++;
			MasteryData->ExperienceToNextLevel = CalculateExperienceForNextLevel(MasteryData->Level);

			ProcessLevelUp(WeaponStyle, *MasteryData);
		}
	}
}

void UMasteryManagerComponent::SetWeaponMastery(EWeaponStyle WeaponStyle, int32 Level)
{
	FMasteryData MasteryData;
	MasteryData.Level = FMath::Clamp(Level, 0, 100);
	MasteryData.Experience = 0.0f;
	MasteryData.ExperienceToNextLevel = CalculateExperienceForNextLevel(MasteryData.Level);

	WeaponMasteryMap.Add(WeaponStyle, MasteryData);
}

float UMasteryManagerComponent::GetWeaponExperiencePercent(EWeaponStyle WeaponStyle) const
{
	if (const FMasteryData* MasteryData = WeaponMasteryMap.Find(WeaponStyle))
	{
		if (MasteryData->ExperienceToNextLevel > 0)
		{
			return MasteryData->Experience / MasteryData->ExperienceToNextLevel;
		}
	}
	return 0.0f;
}

// ============================================
// Progression Helpers
// ============================================

TArray<EMagicElement> UMasteryManagerComponent::GetMasteredElements(int32 MinimumLevel) const
{
	TArray<EMagicElement> MasteredElements;

	for (const auto& Pair : MagicMasteryMap)
	{
		if (Pair.Value.Level >= MinimumLevel)
		{
			MasteredElements.Add(Pair.Key);
		}
	}

	return MasteredElements;
}

TArray<EWeaponStyle> UMasteryManagerComponent::GetMasteredWeapons(int32 MinimumLevel) const
{
	TArray<EWeaponStyle> MasteredWeapons;

	for (const auto& Pair : WeaponMasteryMap)
	{
		if (Pair.Value.Level >= MinimumLevel)
		{
			MasteredWeapons.Add(Pair.Key);
		}
	}

	return MasteredWeapons;
}

int32 UMasteryManagerComponent::GetTotalMagicMasteryLevel() const
{
	int32 Total = 0;
	for (const auto& Pair : MagicMasteryMap)
	{
		Total += Pair.Value.Level;
	}
	return Total;
}

int32 UMasteryManagerComponent::GetTotalWeaponMasteryLevel() const
{
	int32 Total = 0;
	for (const auto& Pair : WeaponMasteryMap)
	{
		Total += Pair.Value.Level;
	}
	return Total;
}

bool UMasteryManagerComponent::HasMasteredAllElements() const
{
	// Check if all 9 elements are at level 100
	int32 ElementCount = 0;
	for (const auto& Pair : MagicMasteryMap)
	{
		if (Pair.Key != EMagicElement::None && Pair.Value.Level >= 100)
		{
			ElementCount++;
		}
	}
	return ElementCount >= 9;
}

bool UMasteryManagerComponent::HasMasteredAllWeapons() const
{
	// Check if all 4 weapon styles are at level 100
	int32 WeaponCount = 0;
	for (const auto& Pair : WeaponMasteryMap)
	{
		if (Pair.Key != EWeaponStyle::None && Pair.Value.Level >= 100)
		{
			WeaponCount++;
		}
	}
	return WeaponCount >= 4;
}

// ============================================
// Private Helper Functions
// ============================================

void UMasteryManagerComponent::InitializeMasteryMaps()
{
	// Initialize all magic elements
	TArray<EMagicElement> Elements = {
		EMagicElement::Fire,
		EMagicElement::Water,
		EMagicElement::Earth,
		EMagicElement::Air,
		EMagicElement::Lightning,
		EMagicElement::Light,
		EMagicElement::Dark,
		EMagicElement::Poison,
		EMagicElement::Ice
	};

	for (EMagicElement Element : Elements)
	{
		if (!MagicMasteryMap.Contains(Element))
		{
			FMasteryData NewMastery;
			MagicMasteryMap.Add(Element, NewMastery);
		}
	}

	// Initialize all weapon styles
	TArray<EWeaponStyle> Weapons = {
		EWeaponStyle::TwoHandedSword,
		EWeaponStyle::Spear,
		EWeaponStyle::Fist,
		EWeaponStyle::Bow
	};

	for (EWeaponStyle Weapon : Weapons)
	{
		if (!WeaponMasteryMap.Contains(Weapon))
		{
			FMasteryData NewMastery;
			WeaponMasteryMap.Add(Weapon, NewMastery);
		}
	}
}

float UMasteryManagerComponent::CalculateExperienceForNextLevel(int32 CurrentLevel) const
{
	// Exponential curve: each level requires more experience
	// Formula: 100 * (1.1 ^ CurrentLevel)
	float BaseExperience = 100.0f;
	float Multiplier = FMath::Pow(1.1f, static_cast<float>(CurrentLevel));
	return BaseExperience * Multiplier;
}

void UMasteryManagerComponent::ProcessLevelUp(EMagicElement Element, FMasteryData& MasteryData)
{
	OnMagicMasteryLevelUp(Element, MasteryData.Level);

	// Check for milestones
	if (MasteryData.Level == 25 || MasteryData.Level == 50 ||
		MasteryData.Level == 75 || MasteryData.Level == 100)
	{
		OnMasteryMilestone(Element, MasteryData.Level);
	}
}

void UMasteryManagerComponent::ProcessLevelUp(EWeaponStyle WeaponStyle, FMasteryData& MasteryData)
{
	OnWeaponMasteryLevelUp(WeaponStyle, MasteryData.Level);
}
