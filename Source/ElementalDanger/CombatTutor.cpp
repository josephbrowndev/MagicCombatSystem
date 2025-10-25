// Combat Tutor Implementation

#include "CombatTutor.h"
#include "NinjaWizardCharacter.h"
#include "WeaponComponent.h"
#include "MasteryManagerComponent.h"

ACombatTutor::ACombatTutor()
{
	Specialization = ETutorSpecialization::Warrior;
}

void ACombatTutor::BeginPlay()
{
	Super::BeginPlay();
}

void ACombatTutor::InitializeMentor()
{
	Super::InitializeMentor();

	// Set up weapons based on specialization
	if (Specialization == ETutorSpecialization::Warrior)
	{
		SetupWarriorWeapons();
		MentorType = EMentorType::WarriorTutor;
	}
	else if (Specialization == ETutorSpecialization::Ninja)
	{
		SetupNinjaWeapons();
		MentorType = EMentorType::NinjaTutor;
	}

	SetupWeaponAttacks();
}

void ACombatTutor::TeachBasicWeaponTechniques(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle)
{
	if (!Player || !Player->WeaponComponent)
	{
		return;
	}

	// Check if this tutor teaches this weapon
	if (!TeachableWeaponStyles.Contains(WeaponStyle))
	{
		return;
	}

	// Unlock the weapon style
	UnlockWeaponStyleForPlayer(Player, WeaponStyle);

	// Teach basic attacks
	for (const FWeaponAttackData& Attack : TeachableAttacks)
	{
		if (Attack.WeaponStyle == WeaponStyle && Attack.RequiredMastery <= 25)
		{
			TeachAttack(Player, Attack);
		}
	}

	OnWeaponStyleUnlockedForPlayer(Player, WeaponStyle);
}

void ACombatTutor::TeachAdvancedWeaponTechniques(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle)
{
	if (!Player || !Player->WeaponComponent || !Player->MasteryManager)
	{
		return;
	}

	// Check mastery level
	int32 Mastery = Player->MasteryManager->GetWeaponMastery(WeaponStyle);
	if (Mastery < 50)
	{
		return; // Not ready for advanced techniques
	}

	// Teach advanced attacks
	for (const FWeaponAttackData& Attack : TeachableAttacks)
	{
		if (Attack.WeaponStyle == WeaponStyle &&
			Attack.RequiredMastery > 25 &&
			Attack.RequiredMastery <= Mastery)
		{
			TeachAttack(Player, Attack);
			OnAdvancedTechniqueUnlocked(Player, Attack);
		}
	}
}

void ACombatTutor::TeachComboAttacks(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle)
{
	if (!Player || !Player->WeaponComponent || !Player->MasteryManager)
	{
		return;
	}

	// Check mastery level for combos (require higher mastery)
	int32 Mastery = Player->MasteryManager->GetWeaponMastery(WeaponStyle);
	if (Mastery < 40)
	{
		return;
	}

	// Teach combo attacks (attacks with ComboIndex > 0)
	for (const FWeaponAttackData& Attack : TeachableAttacks)
	{
		if (Attack.WeaponStyle == WeaponStyle &&
			Attack.ComboIndex > 0 &&
			Attack.RequiredMastery <= Mastery)
		{
			TeachAttack(Player, Attack);
		}
	}
}

TArray<EWeaponStyle> ACombatTutor::GetSpecializedWeapons() const
{
	return TeachableWeaponStyles;
}

void ACombatTutor::SetupWarriorWeapons()
{
	TeachableWeaponStyles.Empty();
	TeachableWeaponStyles.Add(EWeaponStyle::TwoHandedSword);
	TeachableWeaponStyles.Add(EWeaponStyle::Spear);
}

void ACombatTutor::SetupNinjaWeapons()
{
	TeachableWeaponStyles.Empty();
	TeachableWeaponStyles.Add(EWeaponStyle::Fist);
	TeachableWeaponStyles.Add(EWeaponStyle::Bow);
}

void ACombatTutor::SetupWeaponAttacks()
{
	// This would be populated with specific attacks based on specialization
	// For now, we'll leave it empty and let designers fill it in Blueprint or DataTables

	// Example for Warrior - Two-Handed Sword attacks:
	/*
	FWeaponAttackData BasicSlash;
	BasicSlash.AttackName = FName("BasicSlash");
	BasicSlash.WeaponStyle = EWeaponStyle::TwoHandedSword;
	BasicSlash.StaminaCost = 15.0f;
	BasicSlash.BaseDamage = 40.0f;
	BasicSlash.AttackSpeed = 1.2f;
	BasicSlash.ComboIndex = 0;
	BasicSlash.RequiredMastery = 0;
	TeachableAttacks.Add(BasicSlash);

	FWeaponAttackData PowerStrike;
	PowerStrike.AttackName = FName("PowerStrike");
	PowerStrike.WeaponStyle = EWeaponStyle::TwoHandedSword;
	PowerStrike.StaminaCost = 30.0f;
	PowerStrike.BaseDamage = 80.0f;
	PowerStrike.AttackSpeed = 1.8f;
	PowerStrike.ComboIndex = 1;
	PowerStrike.RequiredMastery = 25;
	TeachableAttacks.Add(PowerStrike);
	*/

	// Similar setup for other weapon styles...
}
