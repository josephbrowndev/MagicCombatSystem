// Magic Component Implementation

#include "MagicComponent.h"
#include "NinjaWizardCharacter.h"
#include "MasteryManagerComponent.h"

UMagicComponent::UMagicComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMagicComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ANinjaWizardCharacter>(GetOwner());
	InitializeCombinationRecipes();
}

void UMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ============================================
// Spell Casting
// ============================================

bool UMagicComponent::CastSpell(const FSpellData& SpellData)
{
	if (!CanCastSpell(SpellData))
	{
		return false;
	}

	if (!OwnerCharacter)
	{
		return false;
	}

	// Calculate mana cost with mastery discount
	float ManaCost = CalculateManaCost(SpellData);

	// Consume mana
	if (!OwnerCharacter->ConsumeMana(ManaCost))
	{
		return false;
	}

	// Trigger spell effect (implemented in Blueprint)
	OnSpellCast(SpellData);

	// Grant mastery experience
	if (OwnerCharacter->MasteryManager)
	{
		if (SpellData.Element != EMagicElement::None)
		{
			OwnerCharacter->MasteryManager->AddMagicExperience(SpellData.Element, 10.0f);
		}
	}

	return true;
}

bool UMagicComponent::CanCastSpell(const FSpellData& SpellData) const
{
	if (!OwnerCharacter)
	{
		return false;
	}

	// Check if spell is learned
	if (!LearnedSpells.Contains(SpellData))
	{
		return false;
	}

	// Check mana
	float ManaCost = CalculateManaCost(SpellData);
	if (OwnerCharacter->CurrentMana < ManaCost)
	{
		return false;
	}

	// Check mastery requirement
	if (OwnerCharacter->MasteryManager)
	{
		if (SpellData.Element != EMagicElement::None)
		{
			int32 ElementMastery = OwnerCharacter->MasteryManager->GetMagicMastery(SpellData.Element);
			if (ElementMastery < SpellData.RequiredMastery)
			{
				return false;
			}
		}
	}

	return true;
}

void UMagicComponent::LearnSpell(const FSpellData& SpellData)
{
	if (!LearnedSpells.Contains(SpellData))
	{
		LearnedSpells.Add(SpellData);
	}
}

void UMagicComponent::UnlockElement(EMagicElement Element)
{
	if (!UnlockedElements.Contains(Element))
	{
		UnlockedElements.Add(Element);
		OnElementUnlocked(Element);
	}
}

bool UMagicComponent::HasElement(EMagicElement Element) const
{
	return UnlockedElements.Contains(Element);
}

// ============================================
// Magic Combinations
// ============================================

void UMagicComponent::InitializeCombinationRecipes()
{
	CombinationRecipes.Empty();

	// Define all combination recipes
	FMagicCombination Recipe;

	// Fire + Earth = Lava
	Recipe.Element1 = EMagicElement::Fire;
	Recipe.Element2 = EMagicElement::Earth;
	Recipe.ResultingMagic = ECombinedMagic::Lava;
	Recipe.RequiredMastery = 25;
	CombinationRecipes.Add(Recipe);

	// Lightning + Air = Storm
	Recipe.Element1 = EMagicElement::Lightning;
	Recipe.Element2 = EMagicElement::Air;
	Recipe.ResultingMagic = ECombinedMagic::Storm;
	Recipe.RequiredMastery = 30;
	CombinationRecipes.Add(Recipe);

	// Dark + Poison = Curse
	Recipe.Element1 = EMagicElement::Dark;
	Recipe.Element2 = EMagicElement::Poison;
	Recipe.ResultingMagic = ECombinedMagic::Curse;
	Recipe.RequiredMastery = 35;
	CombinationRecipes.Add(Recipe);

	// Fire + Air = Inferno
	Recipe.Element1 = EMagicElement::Fire;
	Recipe.Element2 = EMagicElement::Air;
	Recipe.ResultingMagic = ECombinedMagic::Inferno;
	Recipe.RequiredMastery = 25;
	CombinationRecipes.Add(Recipe);

	// Water + Ice = Glacier
	Recipe.Element1 = EMagicElement::Water;
	Recipe.Element2 = EMagicElement::Ice;
	Recipe.ResultingMagic = ECombinedMagic::Glacier;
	Recipe.RequiredMastery = 20;
	CombinationRecipes.Add(Recipe);

	// Light + Fire = Solar
	Recipe.Element1 = EMagicElement::Light;
	Recipe.Element2 = EMagicElement::Fire;
	Recipe.ResultingMagic = ECombinedMagic::Solar;
	Recipe.RequiredMastery = 40;
	CombinationRecipes.Add(Recipe);

	// Dark + Fire = Hellfire
	Recipe.Element1 = EMagicElement::Dark;
	Recipe.Element2 = EMagicElement::Fire;
	Recipe.ResultingMagic = ECombinedMagic::Hellfire;
	Recipe.RequiredMastery = 45;
	CombinationRecipes.Add(Recipe);

	// Water + Earth = Swamp/Nature
	Recipe.Element1 = EMagicElement::Water;
	Recipe.Element2 = EMagicElement::Earth;
	Recipe.ResultingMagic = ECombinedMagic::Swamp;
	Recipe.RequiredMastery = 20;
	CombinationRecipes.Add(Recipe);

	// Lightning + Water = Electro Flood
	Recipe.Element1 = EMagicElement::Lightning;
	Recipe.Element2 = EMagicElement::Water;
	Recipe.ResultingMagic = ECombinedMagic::ElectroFlood;
	Recipe.RequiredMastery = 30;
	CombinationRecipes.Add(Recipe);

	// Fire + Water = Steam
	Recipe.Element1 = EMagicElement::Fire;
	Recipe.Element2 = EMagicElement::Water;
	Recipe.ResultingMagic = ECombinedMagic::Steam;
	Recipe.RequiredMastery = 25;
	CombinationRecipes.Add(Recipe);

	// Earth + Ice = Crystal
	Recipe.Element1 = EMagicElement::Earth;
	Recipe.Element2 = EMagicElement::Ice;
	Recipe.ResultingMagic = ECombinedMagic::Crystal;
	Recipe.RequiredMastery = 35;
	CombinationRecipes.Add(Recipe);

	// Ice + Air = Blizzard
	Recipe.Element1 = EMagicElement::Ice;
	Recipe.Element2 = EMagicElement::Air;
	Recipe.ResultingMagic = ECombinedMagic::Blizzard;
	Recipe.RequiredMastery = 30;
	CombinationRecipes.Add(Recipe);

	// Light + Air = Radiance
	Recipe.Element1 = EMagicElement::Light;
	Recipe.Element2 = EMagicElement::Air;
	Recipe.ResultingMagic = ECombinedMagic::Radiance;
	Recipe.RequiredMastery = 35;
	CombinationRecipes.Add(Recipe);

	// Dark + Air = Shadow/Void
	Recipe.Element1 = EMagicElement::Dark;
	Recipe.Element2 = EMagicElement::Air;
	Recipe.ResultingMagic = ECombinedMagic::Shadow;
	Recipe.RequiredMastery = 30;
	CombinationRecipes.Add(Recipe);

	// Poison + Water = Toxin
	Recipe.Element1 = EMagicElement::Poison;
	Recipe.Element2 = EMagicElement::Water;
	Recipe.ResultingMagic = ECombinedMagic::Toxin;
	Recipe.RequiredMastery = 25;
	CombinationRecipes.Add(Recipe);

	// Lightning + Fire = Plasma
	Recipe.Element1 = EMagicElement::Lightning;
	Recipe.Element2 = EMagicElement::Fire;
	Recipe.ResultingMagic = ECombinedMagic::Plasma;
	Recipe.RequiredMastery = 40;
	CombinationRecipes.Add(Recipe);

	// Dark + Earth = Corruption
	Recipe.Element1 = EMagicElement::Dark;
	Recipe.Element2 = EMagicElement::Earth;
	Recipe.ResultingMagic = ECombinedMagic::Corruption;
	Recipe.RequiredMastery = 35;
	CombinationRecipes.Add(Recipe);
}

ECombinedMagic UMagicComponent::GetCombinedMagic(EMagicElement Element1, EMagicElement Element2) const
{
	for (const FMagicCombination& Recipe : CombinationRecipes)
	{
		if ((Recipe.Element1 == Element1 && Recipe.Element2 == Element2) ||
			(Recipe.Element1 == Element2 && Recipe.Element2 == Element1))
		{
			return Recipe.ResultingMagic;
		}
	}
	return ECombinedMagic::None;
}

bool UMagicComponent::CanUseCombination(EMagicElement Element1, EMagicElement Element2) const
{
	if (!OwnerCharacter || !OwnerCharacter->MasteryManager)
	{
		return false;
	}

	// Check if both elements are unlocked
	if (!HasElement(Element1) || !HasElement(Element2))
	{
		return false;
	}

	// Find the recipe
	for (const FMagicCombination& Recipe : CombinationRecipes)
	{
		if ((Recipe.Element1 == Element1 && Recipe.Element2 == Element2) ||
			(Recipe.Element1 == Element2 && Recipe.Element2 == Element1))
		{
			// Check mastery requirements
			int32 Mastery1 = OwnerCharacter->MasteryManager->GetMagicMastery(Element1);
			int32 Mastery2 = OwnerCharacter->MasteryManager->GetMagicMastery(Element2);

			return (Mastery1 >= Recipe.RequiredMastery && Mastery2 >= Recipe.RequiredMastery);
		}
	}

	return false;
}

void UMagicComponent::UnlockCombination(ECombinedMagic CombinedMagic)
{
	if (!UnlockedCombinations.Contains(CombinedMagic))
	{
		UnlockedCombinations.Add(CombinedMagic);
		OnCombinationUnlocked(CombinedMagic);
	}
}

TArray<ECombinedMagic> UMagicComponent::GetAvailableCombinations() const
{
	TArray<ECombinedMagic> AvailableCombinations;

	if (!OwnerCharacter || !OwnerCharacter->MasteryManager)
	{
		return AvailableCombinations;
	}

	for (const FMagicCombination& Recipe : CombinationRecipes)
	{
		if (CanUseCombination(Recipe.Element1, Recipe.Element2))
		{
			AvailableCombinations.AddUnique(Recipe.ResultingMagic);
		}
	}

	return AvailableCombinations;
}

// ============================================
// Helper Functions
// ============================================

float UMagicComponent::CalculateManaCost(const FSpellData& SpellData) const
{
	if (!OwnerCharacter || !OwnerCharacter->MasteryManager)
	{
		return SpellData.ManaCost;
	}

	float BaseCost = SpellData.ManaCost;

	// Apply mastery discount (higher mastery = lower cost)
	if (SpellData.Element != EMagicElement::None)
	{
		int32 Mastery = OwnerCharacter->MasteryManager->GetMagicMastery(SpellData.Element);
		float DiscountPercent = FMath::Clamp(Mastery / 100.0f, 0.0f, 0.5f); // Max 50% discount at 100 mastery
		BaseCost *= (1.0f - DiscountPercent);
	}

	return FMath::Max(BaseCost, 1.0f); // Minimum 1 mana
}
