// Combat Entity - Base class for Enemies and Summonable Allies

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AttributeTypes.h"
#include "MagicTypes.h"
#include "CombatEntity.generated.h"

class ANinjaWizardCharacter;

/**
 * Base class for all combat entities (enemies and allies/summons)
 */
UCLASS()
class ELEMENTALDANGER_API ACombatEntity : public ACharacter
{
	GENERATED_BODY()

public:
	ACombatEntity();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ============================================
	// Entity Properties
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	FName EntityName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	EEntityRank Rank;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	int32 Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	EMagicElement ElementType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	EEnemyThreatLevel ThreatLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Entity")
	bool bIsSummonableAlly;

	// ============================================
	// Combat Stats
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Stats")
	float Defense;

	// ============================================
	// Summon-Specific Properties
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Summon")
	bool bIsPlayerSummon;

	UPROPERTY(BlueprintReadOnly, Category = "Summon")
	ANinjaWizardCharacter* OwnerPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Summon")
	int32 SummonCost; // Mana cost to summon

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Summon")
	int32 SummonCapacityUsage; // How much of player's summon capacity this uses

	// ============================================
	// Progression
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 ExperiencePoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	int32 ExperienceToNextLevel;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 KillCount;

	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 SpecialAbilityUseCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<FSummonChallenge> AvailableChallenges;

	// ============================================
	// Abilities
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
	TArray<FSpellData> SpecialAbilities;

	// ============================================
	// Combat Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ApplyDamageFrom(float Damage, AActor* DamageDealer);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void DealDamageTo(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	bool IsAlive() const { return CurrentHealth > 0; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	float GetHealthPercentage() const;

	// ============================================
	// Summon Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Summon")
	void SetAsPlayerSummon(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Summon")
	bool CanBeSummoned() const { return bIsSummonableAlly; }

	UFUNCTION(BlueprintCallable, Category = "Summon")
	void DismissSummon();

	// ============================================
	// Progression Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void AddExperience(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void LevelUp();

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void RecordKill();

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void RecordSpecialAbilityUse();

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void UpdateChallengeProgress(ESummonChallengeType ChallengeType, int32 Progress);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool TryCompleteChallenge(FSummonChallenge& Challenge);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	void RankUp(EEntityRank NewRank);

	UFUNCTION(BlueprintCallable, Category = "Progression")
	TArray<FSummonChallenge> GetAvailableChallenges() const;

	// ============================================
	// Rank System
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Rank")
	float GetRankMultiplier() const;

	UFUNCTION(BlueprintCallable, Category = "Rank")
	FLinearColor GetRankColor() const;

	// ============================================
	// Soul Bonding
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Soul Bonding")
	bool CanBeBonded() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Soul Bonding")
	void OnSoulBonded(ANinjaWizardCharacter* Player);

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnDeath(AActor* Killer);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnLevelUp(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnRankUp(EEntityRank NewRank);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnChallengeCompleted(const FSummonChallenge& Challenge);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnSummoned(ANinjaWizardCharacter* Summoner);

	UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	void OnDismissed();

protected:
	virtual void ApplyRankBonuses();
	virtual void ApplyLevelBonuses();
};
