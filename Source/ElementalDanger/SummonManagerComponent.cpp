// Summon Manager Component Implementation

#include "SummonManagerComponent.h"
#include "NinjaWizardCharacter.h"
#include "CombatEntity.h"
#include "PlayerAttributeComponent.h"

USummonManagerComponent::USummonManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USummonManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPlayer = Cast<ANinjaWizardCharacter>(GetOwner());
}

void USummonManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Clean up dead summons
	RemoveDeadSummons();
}

// ============================================
// Soul Bonding
// ============================================

bool USummonManagerComponent::TryBondWithEntity(ACombatEntity* Entity)
{
	if (!CanBondWith(Entity))
	{
		return false;
	}

	// Create stored summon data
	FStoredSummon NewSummon = CreateStoredSummonFromEntity(Entity);

	// Add to collection
	CollectedSummons.Add(NewSummon);

	// Trigger bonding event
	OnEntityBonded(NewSummon);

	// Trigger event on entity
	Entity->OnSoulBonded(OwnerPlayer);

	return true;
}

bool USummonManagerComponent::CanBondWith(ACombatEntity* Entity) const
{
	if (!Entity || !OwnerPlayer)
	{
		return false;
	}

	// Entity must be bondable
	if (!Entity->CanBeBonded())
	{
		return false;
	}

	// Entity must be dead (defeated)
	if (Entity->IsAlive())
	{
		return false;
	}

	// Check if already bonded
	if (HasBonded(Entity->GetClass()))
	{
		return false;
	}

	return true;
}

bool USummonManagerComponent::HasBonded(TSubclassOf<ACombatEntity> EntityClass) const
{
	for (const FStoredSummon& Summon : CollectedSummons)
	{
		if (Summon.SummonClass == EntityClass)
		{
			return true;
		}
	}
	return false;
}

// ============================================
// Summoning
// ============================================

ACombatEntity* USummonManagerComponent::SummonAlly(const FStoredSummon& SummonData, FVector SpawnLocation)
{
	if (!CanSummon(SummonData))
	{
		return nullptr;
	}

	// Consume mana
	// Calculate mana cost based on rank and level
	float ManaCost = 25.0f * (static_cast<int32>(SummonData.Rank) + 1) * (SummonData.Level * 0.5f);
	if (!OwnerPlayer->ConsumeMana(ManaCost))
	{
		return nullptr;
	}

	// Spawn the summon
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerPlayer;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ACombatEntity* SpawnedSummon = GetWorld()->SpawnActor<ACombatEntity>(
		SummonData.SummonClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		SpawnParams
	);

	if (SpawnedSummon)
	{
		// Set as player summon
		SpawnedSummon->SetAsPlayerSummon(OwnerPlayer);

		// Apply stored data
		SpawnedSummon->Level = SummonData.Level;
		SpawnedSummon->Rank = SummonData.Rank;
		SpawnedSummon->ExperiencePoints = SummonData.ExperiencePoints;
		SpawnedSummon->KillCount = SummonData.KillCount;
		SpawnedSummon->AvailableChallenges = SummonData.Challenges;

		// Add to active summons
		ActiveSummons.Add(SpawnedSummon);

		OnSummonSpawned(SpawnedSummon);
	}

	return SpawnedSummon;
}

bool USummonManagerComponent::CanSummon(const FStoredSummon& SummonData) const
{
	if (!OwnerPlayer)
	{
		return false;
	}

	// Check summon slot limit
	if (!HasSummonSlotsAvailable())
	{
		return false;
	}

	// Check capacity
	int32 RequiredCapacity = static_cast<int32>(SummonData.Rank) + 1;
	if (!HasCapacityAvailable(RequiredCapacity))
	{
		return false;
	}

	// Check mana
	float ManaCost = 25.0f * (static_cast<int32>(SummonData.Rank) + 1) * (SummonData.Level * 0.5f);
	if (OwnerPlayer->CurrentMana < ManaCost)
	{
		return false;
	}

	return true;
}

void USummonManagerComponent::DismissSummon(ACombatEntity* Summon)
{
	if (!Summon || !ActiveSummons.Contains(Summon))
	{
		return;
	}

	// Save progress before dismissing
	UpdateSummonData(Summon);

	// Remove from active list
	ActiveSummons.Remove(Summon);

	OnSummonDismissed(Summon);

	// Destroy the actor
	Summon->DismissSummon();
}

void USummonManagerComponent::DismissAllSummons()
{
	TArray<ACombatEntity*> SummonsToRemove = ActiveSummons;

	for (ACombatEntity* Summon : SummonsToRemove)
	{
		DismissSummon(Summon);
	}
}

int32 USummonManagerComponent::GetCurrentCapacityUsed() const
{
	int32 TotalCapacity = 0;

	for (ACombatEntity* Summon : ActiveSummons)
	{
		if (Summon && Summon->IsAlive())
		{
			TotalCapacity += Summon->SummonCapacityUsage;
		}
	}

	return TotalCapacity;
}

// ============================================
// Summon Management
// ============================================

TArray<FStoredSummon> USummonManagerComponent::GetSummonsByElement(EMagicElement Element) const
{
	TArray<FStoredSummon> FilteredSummons;

	for (const FStoredSummon& Summon : CollectedSummons)
	{
		if (Summon.ElementType == Element)
		{
			FilteredSummons.Add(Summon);
		}
	}

	return FilteredSummons;
}

TArray<FStoredSummon> USummonManagerComponent::GetSummonsByRank(EEntityRank Rank) const
{
	TArray<FStoredSummon> FilteredSummons;

	for (const FStoredSummon& Summon : CollectedSummons)
	{
		if (Summon.Rank == Rank)
		{
			FilteredSummons.Add(Summon);
		}
	}

	return FilteredSummons;
}

FStoredSummon* USummonManagerComponent::FindStoredSummon(FName SummonName)
{
	for (FStoredSummon& Summon : CollectedSummons)
	{
		if (Summon.SummonName == SummonName)
		{
			return &Summon;
		}
	}
	return nullptr;
}

void USummonManagerComponent::UpdateSummonData(ACombatEntity* Summon)
{
	if (!Summon)
	{
		return;
	}

	FStoredSummon* StoredData = FindStoredSummon(Summon->EntityName);
	if (StoredData)
	{
		// Update stored data with current summon state
		StoredData->Level = Summon->Level;
		StoredData->Rank = Summon->Rank;
		StoredData->ExperiencePoints = Summon->ExperiencePoints;
		StoredData->KillCount = Summon->KillCount;
		StoredData->Challenges = Summon->AvailableChallenges;
	}
}

// ============================================
// Limits & Capacity
// ============================================

int32 USummonManagerComponent::GetMaxSummonCount() const
{
	if (!OwnerPlayer || !OwnerPlayer->AttributeComponent)
	{
		return 1;
	}

	return OwnerPlayer->AttributeComponent->GetMaxSummonCount();
}

int32 USummonManagerComponent::GetMaxSummonCapacity() const
{
	if (!OwnerPlayer || !OwnerPlayer->AttributeComponent)
	{
		return 1;
	}

	return OwnerPlayer->AttributeComponent->GetSummonCapacity();
}

bool USummonManagerComponent::HasSummonSlotsAvailable() const
{
	return GetActiveSummonCount() < GetMaxSummonCount();
}

bool USummonManagerComponent::HasCapacityAvailable(int32 RequiredCapacity) const
{
	int32 CurrentCapacity = GetCurrentCapacityUsed();
	int32 MaxCapacity = GetMaxSummonCapacity();

	return (CurrentCapacity + RequiredCapacity) <= MaxCapacity;
}

// ============================================
// Debug & Utility
// ============================================

void USummonManagerComponent::PrintSummonCollection() const
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
			FString::Printf(TEXT("Collected Summons: %d"), CollectedSummons.Num()));

		for (const FStoredSummon& Summon : CollectedSummons)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White,
				FString::Printf(TEXT("- %s (Rank: %d, Level: %d)"),
					*Summon.SummonName.ToString(),
					static_cast<int32>(Summon.Rank),
					Summon.Level));
		}
	}
}

// ============================================
// Private Functions
// ============================================

FStoredSummon USummonManagerComponent::CreateStoredSummonFromEntity(ACombatEntity* Entity) const
{
	FStoredSummon NewSummon;
	NewSummon.SummonClass = Entity->GetClass();
	NewSummon.SummonName = Entity->EntityName;
	NewSummon.ElementType = Entity->ElementType;
	NewSummon.Rank = Entity->Rank;
	NewSummon.Level = Entity->Level;
	NewSummon.ExperiencePoints = Entity->ExperiencePoints;
	NewSummon.KillCount = Entity->KillCount;
	NewSummon.Challenges = Entity->AvailableChallenges;

	return NewSummon;
}

void USummonManagerComponent::RemoveDeadSummons()
{
	TArray<ACombatEntity*> DeadSummons;

	for (ACombatEntity* Summon : ActiveSummons)
	{
		if (!Summon || !Summon->IsAlive())
		{
			DeadSummons.Add(Summon);
		}
	}

	for (ACombatEntity* DeadSummon : DeadSummons)
	{
		if (DeadSummon)
		{
			UpdateSummonData(DeadSummon);
			OnSummonDied(DeadSummon);
		}
		ActiveSummons.Remove(DeadSummon);
	}
}
