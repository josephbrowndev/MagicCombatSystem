// Dragon Mentor Implementation

#include "DragonMentor.h"
#include "NinjaWizardCharacter.h"
#include "MagicComponent.h"
#include "MasteryManagerComponent.h"

ADragonMentor::ADragonMentor()
{
	DragonScale = 5.0f;
	DragonColor = FLinearColor::Red;
}

void ADragonMentor::BeginPlay()
{
	Super::BeginPlay();
}

void ADragonMentor::InitializeMentor()
{
	Super::InitializeMentor();
	SetupElementalSpells();
}

void ADragonMentor::TeachBasicElementalSpells(ANinjaWizardCharacter* Player)
{
	if (!Player || !Player->MagicComponent)
	{
		return;
	}

	// Unlock the primary element
	UnlockElementForPlayer(Player);

	// Teach basic spells for this element
	for (const FSpellData& Spell : TeachableSpells)
	{
		if (Spell.RequiredMastery <= 25) // Basic spells
		{
			TeachSpell(Player, Spell);
		}
	}
}

void ADragonMentor::TeachAdvancedElementalSpells(ANinjaWizardCharacter* Player)
{
	if (!Player || !Player->MagicComponent || !Player->MasteryManager)
	{
		return;
	}

	// Check if player has sufficient mastery
	int32 Mastery = Player->MasteryManager->GetMagicMastery(PrimaryElement);
	if (Mastery < 50)
	{
		return; // Not ready for advanced spells
	}

	// Teach advanced spells
	for (const FSpellData& Spell : TeachableSpells)
	{
		if (Spell.RequiredMastery > 25 && Spell.RequiredMastery <= Mastery)
		{
			TeachSpell(Player, Spell);
		}
	}
}

void ADragonMentor::UnlockCombinationMagic(ANinjaWizardCharacter* Player, EMagicElement SecondaryElement)
{
	if (!Player || !Player->MagicComponent || !Player->MasteryManager)
	{
		return;
	}

	// Check if player has both elements
	if (!Player->MagicComponent->HasElement(PrimaryElement) ||
		!Player->MagicComponent->HasElement(SecondaryElement))
	{
		return;
	}

	// Check if combination is possible
	if (Player->MagicComponent->CanUseCombination(PrimaryElement, SecondaryElement))
	{
		ECombinedMagic CombinedMagic = Player->MagicComponent->GetCombinedMagic(PrimaryElement, SecondaryElement);
		if (CombinedMagic != ECombinedMagic::None)
		{
			Player->MagicComponent->UnlockCombination(CombinedMagic);
			ImproveRelationship(20);
			OnCombinationUnlockedForPlayer(Player, CombinedMagic);
		}
	}
}

void ADragonMentor::SetupElementalSpells()
{
	// This would be populated with specific spells based on PrimaryElement
	// For now, we'll leave it empty and let designers fill it in Blueprint or DataTables

	// Example structure:
	// FSpellData BasicSpell;
	// BasicSpell.SpellName = FName("Fireball");
	// BasicSpell.Element = EMagicElement::Fire;
	// BasicSpell.ManaCost = 20.0f;
	// BasicSpell.BaseDamage = 50.0f;
	// BasicSpell.CastTime = 1.0f;
	// BasicSpell.RequiredMastery = 0;
	// TeachableSpells.Add(BasicSpell);
}
