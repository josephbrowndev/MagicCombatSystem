// Player Attribute System Types

#pragma once

#include "CoreMinimal.h"
#include "AttributeTypes.generated.h"

/**
 * Player core attributes
 */
UENUM(BlueprintType)
enum class EPlayerAttribute : uint8
{
	Strength    UMETA(DisplayName = "Strength (STR)"),
	Wisdom      UMETA(DisplayName = "Wisdom (WIS)"),
	Perception  UMETA(DisplayName = "Perception (PER)"),
	Agility     UMETA(DisplayName = "Agility (AGI)")
};

/**
 * Rank levels for enemies and allies
 */
UENUM(BlueprintType)
enum class EEntityRank : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Soldier     UMETA(DisplayName = "Soldier"),
	Captain     UMETA(DisplayName = "Captain"),
	General     UMETA(DisplayName = "General"),
	Unique      UMETA(DisplayName = "Unique")
};

/**
 * Enemy threat level and behavior
 */
UENUM(BlueprintType)
enum class EEnemyThreatLevel : uint8
{
	Passive         UMETA(DisplayName = "Passive - Won't Attack"),
	Defensive       UMETA(DisplayName = "Defensive - Attacks if Attacked"),
	Territorial     UMETA(DisplayName = "Territorial - Attacks if Too Close"),
	Aggressive      UMETA(DisplayName = "Aggressive - Always Attacks"),
	Boss            UMETA(DisplayName = "Boss - Special Behavior")
};

/**
 * Structure for attribute data
 */
USTRUCT(BlueprintType)
struct FAttributeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 BaseValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int32 BonusValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	float Multiplier;

	FAttributeData()
		: BaseValue(10)
		, BonusValue(0)
		, Multiplier(1.0f)
	{}

	int32 GetTotalValue() const
	{
		return FMath::RoundToInt((BaseValue + BonusValue) * Multiplier);
	}
};

/**
 * Attribute bonuses calculated from attributes
 */
USTRUCT(BlueprintType)
struct FDerivedStats
{
	GENERATED_BODY()

	// From Strength
	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Strength")
	float BlockChance;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Strength")
	float WeaponDamageMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Strength")
	float MaxStaminaBonus;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Strength")
	float MaxHealthBonus;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Strength")
	float CarryWeightBonus;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Strength")
	float KnockbackResistance;

	// From Wisdom
	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	float MagicalDamageMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	float SpellSizeMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	int32 MaxSummonCount;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	int32 SummonCapacity;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	float MaxManaBonus;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	float ManaRegenBonus;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Wisdom")
	float SpellCooldownReduction;

	// From Perception
	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float DangerSenseRange;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float LevelDifferenceDetection;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float SlowMotionChance;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float SlowMotionDuration;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float ThreatDetectionRange;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float CriticalHitChance;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Perception")
	float StealthDetectionBonus;

	// From Agility
	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float AttackSpeedMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float MovementSpeedMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float DodgeChance;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float JumpHeightMultiplier;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float FallDamageReduction;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float StaminaRegenBonus;

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats|Agility")
	float EvasionRating;

	FDerivedStats()
		: BlockChance(0.0f)
		, WeaponDamageMultiplier(1.0f)
		, MaxStaminaBonus(0.0f)
		, MaxHealthBonus(0.0f)
		, CarryWeightBonus(0.0f)
		, KnockbackResistance(0.0f)
		, MagicalDamageMultiplier(1.0f)
		, SpellSizeMultiplier(1.0f)
		, MaxSummonCount(1)
		, SummonCapacity(1)
		, MaxManaBonus(0.0f)
		, ManaRegenBonus(0.0f)
		, SpellCooldownReduction(0.0f)
		, DangerSenseRange(0.0f)
		, LevelDifferenceDetection(0.0f)
		, SlowMotionChance(0.0f)
		, SlowMotionDuration(0.0f)
		, ThreatDetectionRange(1000.0f)
		, CriticalHitChance(0.0f)
		, StealthDetectionBonus(0.0f)
		, AttackSpeedMultiplier(1.0f)
		, MovementSpeedMultiplier(1.0f)
		, DodgeChance(0.0f)
		, JumpHeightMultiplier(1.0f)
		, FallDamageReduction(0.0f)
		, StaminaRegenBonus(0.0f)
		, EvasionRating(0.0f)
	{}
};

/**
 * Summon challenge types for rank-up
 */
UENUM(BlueprintType)
enum class ESummonChallengeType : uint8
{
	KillSpecificEnemy       UMETA(DisplayName = "Kill Specific Enemy"),
	KillMultipleEnemies     UMETA(DisplayName = "Kill Multiple Enemies"),
	KillWithinTimeLimit     UMETA(DisplayName = "Kill Within Time Limit"),
	DealDamageThreshold     UMETA(DisplayName = "Deal X Damage"),
	SurviveTimeLimit        UMETA(DisplayName = "Survive X Seconds"),
	UseSpecialAbility       UMETA(DisplayName = "Use Special Ability X Times"),
	ProtectPlayer           UMETA(DisplayName = "Protect Player"),
	DefeatEliteEnemy        UMETA(DisplayName = "Defeat Elite Enemy")
};

/**
 * Summon challenge data
 */
USTRUCT(BlueprintType)
struct FSummonChallenge
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	ESummonChallengeType ChallengeType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	FText ChallengeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	FText ChallengeDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	int32 RequiredCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	float TimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	EEntityRank RequiredRankToUnlock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Challenge")
	EEntityRank RewardRank;

	UPROPERTY(BlueprintReadOnly, Category = "Challenge")
	int32 CurrentProgress;

	UPROPERTY(BlueprintReadOnly, Category = "Challenge")
	bool bCompleted;

	FSummonChallenge()
		: ChallengeType(ESummonChallengeType::KillMultipleEnemies)
		, RequiredCount(10)
		, TimeLimit(0.0f)
		, RequiredRankToUnlock(EEntityRank::Common)
		, RewardRank(EEntityRank::Soldier)
		, CurrentProgress(0)
		, bCompleted(false)
	{}
};
