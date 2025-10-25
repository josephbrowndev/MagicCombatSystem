// Base Mentor Implementation

#include "MentorBase.h"
#include "NinjaWizardCharacter.h"
#include "MagicComponent.h"
#include "WeaponComponent.h"
#include "MasteryManagerComponent.h"

AMentorBase::AMentorBase()
{
	PrimaryActorTick.bCanEverTick = true;

	MentorType = EMentorType::None;
	MentorName = FName("Mentor");
	RelationshipLevel = 0;
	PrimaryElement = EMagicElement::None;
}

void AMentorBase::BeginPlay()
{
	Super::BeginPlay();
	InitializeMentor();
}

void AMentorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ============================================
// Interaction
// ============================================

void AMentorBase::InteractWithPlayer(ANinjaWizardCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	OnPlayerInteract(Player);

	// Improve relationship slightly on each interaction
	ImproveRelationship(1);
}

void AMentorBase::TeachSpell(ANinjaWizardCharacter* Player, const FSpellData& Spell)
{
	if (!Player || !Player->MagicComponent)
	{
		return;
	}

	if (!CanTeachPlayer(Player))
	{
		return;
	}

	// Check if this spell is teachable by this mentor
	if (!TeachableSpells.Contains(Spell))
	{
		return;
	}

	// Teach the spell
	Player->MagicComponent->LearnSpell(Spell);

	// Improve relationship
	ImproveRelationship(5);

	OnSpellTaught(Player, Spell);
}

void AMentorBase::TeachAttack(ANinjaWizardCharacter* Player, const FWeaponAttackData& Attack)
{
	if (!Player || !Player->WeaponComponent)
	{
		return;
	}

	if (!CanTeachPlayer(Player))
	{
		return;
	}

	// Check if this attack is teachable by this mentor
	if (!TeachableAttacks.Contains(Attack))
	{
		return;
	}

	// Teach the attack
	Player->WeaponComponent->LearnAttack(Attack);

	// Improve relationship
	ImproveRelationship(5);
}

void AMentorBase::UnlockElementForPlayer(ANinjaWizardCharacter* Player)
{
	if (!Player || !Player->MagicComponent)
	{
		return;
	}

	if (PrimaryElement != EMagicElement::None)
	{
		Player->MagicComponent->UnlockElement(PrimaryElement);
		ImproveRelationship(10);
	}
}

void AMentorBase::UnlockWeaponStyleForPlayer(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle)
{
	if (!Player || !Player->WeaponComponent)
	{
		return;
	}

	if (TeachableWeaponStyles.Contains(WeaponStyle))
	{
		Player->WeaponComponent->UnlockWeaponStyle(WeaponStyle);
		ImproveRelationship(10);
	}
}

void AMentorBase::ImproveRelationship(int32 Amount)
{
	int32 OldLevel = RelationshipLevel;
	RelationshipLevel = FMath::Clamp(RelationshipLevel + Amount, 0, 100);

	if (RelationshipLevel != OldLevel)
	{
		OnRelationshipImproved(RelationshipLevel);
	}
}

bool AMentorBase::CanTeachPlayer(ANinjaWizardCharacter* Player) const
{
	if (!Player)
	{
		return false;
	}

	// Relationship requirement
	if (RelationshipLevel < 10)
	{
		return false;
	}

	return true;
}

// ============================================
// Quests
// ============================================

TArray<FMentorQuest> AMentorBase::GetAvailableQuests(ANinjaWizardCharacter* Player) const
{
	TArray<FMentorQuest> Available;

	if (!Player || !Player->MasteryManager)
	{
		return Available;
	}

	for (const FMentorQuest& Quest : AvailableQuests)
	{
		if (Quest.bCompleted)
		{
			continue;
		}

		// Check mastery requirement
		if (PrimaryElement != EMagicElement::None)
		{
			int32 Mastery = Player->MasteryManager->GetMagicMastery(PrimaryElement);
			if (Mastery >= Quest.RequiredMasteryLevel)
			{
				Available.Add(Quest);
			}
		}
		else
		{
			// For weapon tutors, check weapon mastery
			for (EWeaponStyle WeaponStyle : TeachableWeaponStyles)
			{
				int32 Mastery = Player->MasteryManager->GetWeaponMastery(WeaponStyle);
				if (Mastery >= Quest.RequiredMasteryLevel)
				{
					Available.Add(Quest);
					break;
				}
			}
		}
	}

	return Available;
}

void AMentorBase::CompleteQuest(FMentorQuest& Quest, ANinjaWizardCharacter* Player)
{
	if (!Player || Quest.bCompleted)
	{
		return;
	}

	Quest.bCompleted = true;
	GiveQuestRewards(Quest, Player);
	ImproveRelationship(15);

	OnQuestCompleted(Player, Quest);
}

void AMentorBase::GiveQuestRewards(const FMentorQuest& Quest, ANinjaWizardCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	// Give spell rewards
	if (Player->MagicComponent)
	{
		for (const FSpellData& Spell : Quest.RewardSpells)
		{
			Player->MagicComponent->LearnSpell(Spell);
		}

		if (Quest.RewardElement != EMagicElement::None)
		{
			Player->MagicComponent->UnlockElement(Quest.RewardElement);
		}
	}

	// Give weapon rewards
	if (Player->WeaponComponent)
	{
		for (const FWeaponAttackData& Attack : Quest.RewardAttacks)
		{
			Player->WeaponComponent->LearnAttack(Attack);
		}

		if (Quest.RewardWeaponStyle != EWeaponStyle::None)
		{
			Player->WeaponComponent->UnlockWeaponStyle(Quest.RewardWeaponStyle);
		}
	}
}

void AMentorBase::InitializeMentor()
{
	// Override in child classes for specific initialization
}
