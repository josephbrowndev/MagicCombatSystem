// Summon Manager Component - Manages player's summoned allies

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeTypes.h"
#include "MagicTypes.h"
#include "SummonManagerComponent.generated.h"

class ANinjaWizardCharacter;
class ACombatEntity;

/**
 * Stored summon data
 */
USTRUCT(BlueprintType)
struct FStoredSummon
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ACombatEntity> SummonClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SummonName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMagicElement ElementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEntityRank Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ExperiencePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 KillCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSummonChallenge> Challenges;

	FStoredSummon()
		: ElementType(EMagicElement::None)
		, Rank(EEntityRank::Common)
		, Level(1)
		, ExperiencePoints(0)
		, KillCount(0)
	{}

	bool operator==(const FStoredSummon& Other) const
	{
		return SummonName == Other.SummonName &&
			   ElementType == Other.ElementType &&
			   Rank == Other.Rank;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API USummonManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USummonManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Summon Collection
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Summons")
	TArray<FStoredSummon> CollectedSummons;

	UPROPERTY(BlueprintReadOnly, Category = "Summons")
	TArray<ACombatEntity*> ActiveSummons;

	// ============================================
	// Soul Bonding
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Soul Bonding")
	bool TryBondWithEntity(ACombatEntity* Entity);

	UFUNCTION(BlueprintCallable, Category = "Soul Bonding")
	bool CanBondWith(ACombatEntity* Entity) const;

	UFUNCTION(BlueprintCallable, Category = "Soul Bonding")
	bool HasBonded(TSubclassOf<ACombatEntity> EntityClass) const;

	// ============================================
	// Summoning
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	ACombatEntity* SummonAlly(const FStoredSummon& SummonData, FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	bool CanSummon(const FStoredSummon& SummonData) const;

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	void DismissSummon(ACombatEntity* Summon);

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	void DismissAllSummons();

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	int32 GetActiveSummonCount() const { return ActiveSummons.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Summoning")
	int32 GetCurrentCapacityUsed() const;

	// ============================================
	// Summon Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Summons")
	TArray<FStoredSummon> GetSummonsByElement(EMagicElement Element) const;

	UFUNCTION(BlueprintCallable, Category = "Summons")
	TArray<FStoredSummon> GetSummonsByRank(EEntityRank Rank) const;

	UFUNCTION(BlueprintCallable, Category = "Summons")
	FStoredSummon* FindStoredSummon(FName SummonName);

	UFUNCTION(BlueprintCallable, Category = "Summons")
	void UpdateSummonData(ACombatEntity* Summon);

	// ============================================
	// Limits & Capacity
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Limits")
	int32 GetMaxSummonCount() const;

	UFUNCTION(BlueprintCallable, Category = "Limits")
	int32 GetMaxSummonCapacity() const;

	UFUNCTION(BlueprintCallable, Category = "Limits")
	bool HasSummonSlotsAvailable() const;

	UFUNCTION(BlueprintCallable, Category = "Limits")
	bool HasCapacityAvailable(int32 RequiredCapacity) const;

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnEntityBonded(const FStoredSummon& NewSummon);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSummonSpawned(ACombatEntity* Summon);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSummonDismissed(ACombatEntity* Summon);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSummonDied(ACombatEntity* Summon);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSummonLevelUp(ACombatEntity* Summon, int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSummonRankUp(ACombatEntity* Summon, EEntityRank NewRank);

	// ============================================
	// Debug & Utility
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void PrintSummonCollection() const;

private:
	UPROPERTY()
	ANinjaWizardCharacter* OwnerPlayer;

	FStoredSummon CreateStoredSummonFromEntity(ACombatEntity* Entity) const;
	void RemoveDeadSummons();
};
