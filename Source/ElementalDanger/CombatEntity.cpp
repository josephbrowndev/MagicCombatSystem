// Combat Entity Implementation

#include "CombatEntity.h"
#include "NinjaWizardCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ACombatEntity::ACombatEntity()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default values
	Rank = EEntityRank::Common;
	Level = 1;
	ElementType = EMagicElement::None;
	ThreatLevel = EEnemyThreatLevel::Aggressive;
	bIsSummonableAlly = true;
	bIsPlayerSummon = false;
	OwnerPlayer = nullptr;

	// Combat stats
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	BaseDamage = 20.0f;
	AttackSpeed = 1.0f;
	MovementSpeed = 400.0f;
	Defense = 10.0f;

	// Summon properties
	SummonCost = 25;
	SummonCapacityUsage = 1;

	// Progression
	ExperiencePoints = 0;
	ExperienceToNextLevel = 100;
	KillCount = 0;
	SpecialAbilityUseCount = 0;
}

void ACombatEntity::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;
	ApplyRankBonuses();
	ApplyLevelBonuses();

	// Set movement speed
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = MovementSpeed;
	}
}

void ACombatEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ============================================
// Combat Functions
// ============================================

void ACombatEntity::TakeDamage(float Damage, AActor* DamageDealer)
{
	if (!IsAlive())
	{
		return;
	}

	// Apply defense reduction
	float ActualDamage = FMath::Max(Damage - Defense, 0.0f);
	CurrentHealth -= ActualDamage;

	if (CurrentHealth <= 0)
	{
		CurrentHealth = 0;
		Die();
	}
}

void ACombatEntity::DealDamageTo(AActor* Target)
{
	if (!Target)
	{
		return;
	}

	// Calculate damage with rank multiplier
	float TotalDamage = BaseDamage * GetRankMultiplier();

	// If target is a combat entity, call its TakeDamage
	if (ACombatEntity* TargetEntity = Cast<ACombatEntity>(Target))
	{
		TargetEntity->TakeDamage(TotalDamage, this);
	}
}

void ACombatEntity::Die()
{
	if (!IsAlive())
	{
		return; // Already dead
	}

	CurrentHealth = 0;

	// Trigger death event
	OnDeath(nullptr);

	// If this is a player summon, notify the summon manager
	if (bIsPlayerSummon && OwnerPlayer)
	{
		// Summon manager will handle cleanup
	}

	// Disable collision and input
	SetActorEnableCollision(false);
	DisableInput(nullptr);

	// Destroy after delay (or play death animation first)
	SetLifeSpan(5.0f);
}

float ACombatEntity::GetHealthPercentage() const
{
	return MaxHealth > 0 ? (CurrentHealth / MaxHealth) : 0.0f;
}

// ============================================
// Summon Functions
// ============================================

void ACombatEntity::SetAsPlayerSummon(ANinjaWizardCharacter* Player)
{
	if (!Player)
	{
		return;
	}

	bIsPlayerSummon = true;
	OwnerPlayer = Player;

	// Change team/faction
	// TODO: Implement team system

	OnSummoned(Player);
}

void ACombatEntity::DismissSummon()
{
	if (!bIsPlayerSummon)
	{
		return;
	}

	OnDismissed();

	// Destroy the summon with fade effect
	SetLifeSpan(1.0f);
}

// ============================================
// Progression Functions
// ============================================

void ACombatEntity::AddExperience(int32 Amount)
{
	if (!bIsPlayerSummon)
	{
		return; // Only summons gain XP
	}

	ExperiencePoints += Amount;

	while (ExperiencePoints >= ExperienceToNextLevel)
	{
		ExperiencePoints -= ExperienceToNextLevel;
		LevelUp();
	}
}

void ACombatEntity::LevelUp()
{
	Level++;
	ExperienceToNextLevel = FMath::RoundToInt(ExperienceToNextLevel * 1.15f);

	ApplyLevelBonuses();
	OnLevelUp(Level);
}

void ACombatEntity::RecordKill()
{
	KillCount++;

	// Update challenges
	UpdateChallengeProgress(ESummonChallengeType::KillMultipleEnemies, 1);

	// Grant experience
	AddExperience(10);
}

void ACombatEntity::RecordSpecialAbilityUse()
{
	SpecialAbilityUseCount++;

	// Update challenges
	UpdateChallengeProgress(ESummonChallengeType::UseSpecialAbility, 1);
}

void ACombatEntity::UpdateChallengeProgress(ESummonChallengeType ChallengeType, int32 Progress)
{
	for (FSummonChallenge& Challenge : AvailableChallenges)
	{
		if (Challenge.ChallengeType == ChallengeType && !Challenge.bCompleted)
		{
			Challenge.CurrentProgress += Progress;

			if (Challenge.CurrentProgress >= Challenge.RequiredCount)
			{
				TryCompleteChallenge(Challenge);
			}
		}
	}
}

bool ACombatEntity::TryCompleteChallenge(FSummonChallenge& Challenge)
{
	if (Challenge.bCompleted)
	{
		return false;
	}

	if (Challenge.CurrentProgress < Challenge.RequiredCount)
	{
		return false;
	}

	// Complete the challenge
	Challenge.bCompleted = true;

	// Grant rank up
	RankUp(Challenge.RewardRank);

	OnChallengeCompleted(Challenge);

	return true;
}

void ACombatEntity::RankUp(EEntityRank NewRank)
{
	if (NewRank <= Rank)
	{
		return; // Can't downrank
	}

	Rank = NewRank;
	ApplyRankBonuses();

	OnRankUp(NewRank);
}

TArray<FSummonChallenge> ACombatEntity::GetAvailableChallenges() const
{
	TArray<FSummonChallenge> Available;

	for (const FSummonChallenge& Challenge : AvailableChallenges)
	{
		// Only show challenges for current rank
		if (Challenge.RequiredRankToUnlock == Rank && !Challenge.bCompleted)
		{
			Available.Add(Challenge);
		}
	}

	return Available;
}

// ============================================
// Rank System
// ============================================

float ACombatEntity::GetRankMultiplier() const
{
	switch (Rank)
	{
		case EEntityRank::Common:
			return 1.0f;
		case EEntityRank::Soldier:
			return 1.5f;
		case EEntityRank::Captain:
			return 2.0f;
		case EEntityRank::General:
			return 3.0f;
		case EEntityRank::Unique:
			return 4.0f;
		default:
			return 1.0f;
	}
}

FLinearColor ACombatEntity::GetRankColor() const
{
	switch (Rank)
	{
		case EEntityRank::Common:
			return FLinearColor::White;
		case EEntityRank::Soldier:
			return FLinearColor::Green;
		case EEntityRank::Captain:
			return FLinearColor::Blue;
		case EEntityRank::General:
			return FLinearColor(1.0f, 0.0f, 1.0f); // Purple
		case EEntityRank::Unique:
			return FLinearColor(1.0f, 0.5f, 0.0f); // Orange/Gold
		default:
			return FLinearColor::Gray;
	}
}

// ============================================
// Soul Bonding
// ============================================

bool ACombatEntity::CanBeBonded() const
{
	// All enemies can be bonded (alive or dead), as long as they're not already a player summon
	// This allows bonding with defeated enemies and even corpses found in the world
	return !bIsPlayerSummon;
}

// ============================================
// Protected Functions
// ============================================

void ACombatEntity::ApplyRankBonuses()
{
	float Multiplier = GetRankMultiplier();

	// Apply multiplier to all stats
	BaseDamage *= Multiplier;
	MaxHealth *= Multiplier;
	Defense *= Multiplier;

	// Adjust summon cost based on rank
	SummonCost = FMath::RoundToInt(SummonCost * Multiplier);
	SummonCapacityUsage = FMath::RoundToInt(SummonCapacityUsage * Multiplier);
}

void ACombatEntity::ApplyLevelBonuses()
{
	// Each level grants +10% to all stats
	float LevelMultiplier = 1.0f + ((Level - 1) * 0.1f);

	BaseDamage *= LevelMultiplier;
	MaxHealth *= LevelMultiplier;
	Defense *= LevelMultiplier;

	// Restore health on level up
	CurrentHealth = MaxHealth;
}
