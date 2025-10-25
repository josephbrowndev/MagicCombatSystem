// AI Behavior System Type Definitions

#pragma once

#include "CoreMinimal.h"
#include "MagicTypes.h"
#include "AIBehaviorTypes.generated.h"

// Forward declarations
class AActor;
class ANinjaWizardCharacter;

// ============================================
// AI Behavior Types
// ============================================

/**
 * Main behavior types for AI entities
 */
UENUM(BlueprintType)
enum class EAIBehaviorType : uint8
{
	Passive         UMETA(DisplayName = "Passive - Explores, Non-Hostile"),
	Chasing         UMETA(DisplayName = "Chasing - Catchable Creatures"),
	Neutral         UMETA(DisplayName = "Neutral - Defensive Only"),
	AreaGuard       UMETA(DisplayName = "Area Guard - Territorial"),
	Aggressive      UMETA(DisplayName = "Aggressive - Zone Bosses")
};

/**
 * AI combat roles for different enemy types
 */
UENUM(BlueprintType)
enum class EAICombatRole : uint8
{
	Warrior         UMETA(DisplayName = "Warrior - Melee Combos"),
	Mage            UMETA(DisplayName = "Mage - Magic Caster"),
	Archer          UMETA(DisplayName = "Archer - Ranged Attacks"),
	Tank            UMETA(DisplayName = "Tank - Heavy Defense"),
	Assassin        UMETA(DisplayName = "Assassin - Fast Strikes"),
	Boss            UMETA(DisplayName = "Boss - Complex Patterns"),
	Support         UMETA(DisplayName = "Support - Heals/Buffs")
};

/**
 * Current AI state
 */
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle            UMETA(DisplayName = "Idle"),
	Wandering       UMETA(DisplayName = "Wandering"),
	Investigating   UMETA(DisplayName = "Investigating"),
	Fleeing         UMETA(DisplayName = "Fleeing"),
	Chasing         UMETA(DisplayName = "Chasing"),
	Attacking       UMETA(DisplayName = "Attacking"),
	Patrolling      UMETA(DisplayName = "Patrolling"),
	Eating          UMETA(DisplayName = "Eating"),
	Resting         UMETA(DisplayName = "Resting"),
	Guarding        UMETA(DisplayName = "Guarding"),
	Returning       UMETA(DisplayName = "Returning to Territory")
};

/**
 * Group behavior types
 */
UENUM(BlueprintType)
enum class EGroupBehavior : uint8
{
	Solo            UMETA(DisplayName = "Solo - No Group"),
	SmallPack       UMETA(DisplayName = "Small Pack (2-4)"),
	MediumPack      UMETA(DisplayName = "Medium Pack (5-10)"),
	LargeHerd       UMETA(DisplayName = "Large Herd (10+)"),
	BossPack        UMETA(DisplayName = "Boss + Minions")
};

/**
 * Boss attack pattern types
 */
UENUM(BlueprintType)
enum class EBossAttackPattern : uint8
{
	BasicCombo      UMETA(DisplayName = "Basic Attack Combo"),
	AreaOfEffect    UMETA(DisplayName = "AOE Attack"),
	Charge          UMETA(DisplayName = "Charge Attack"),
	SummonMinions   UMETA(DisplayName = "Summon Minions"),
	EnragedMode     UMETA(DisplayName = "Enraged Mode"),
	TeleportStrike  UMETA(DisplayName = "Teleport Strike"),
	GroundSlam      UMETA(DisplayName = "Ground Slam"),
	RangedBarrage   UMETA(DisplayName = "Ranged Barrage"),
	DefensiveStance UMETA(DisplayName = "Defensive Stance"),
	ElementalBurst  UMETA(DisplayName = "Elemental Burst"),
	PhasedAttack    UMETA(DisplayName = "Multi-Phase Attack")
};

// ============================================
// AI Behavior Structs
// ============================================

/**
 * Passive mob behavior settings
 */
USTRUCT(BlueprintType)
struct FPassiveBehaviorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	float WanderRadius = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	float WanderInterval = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	float EatingInterval = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	float RestingInterval = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	bool bCanFlee = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	float FleeDistance = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Passive")
	TArray<FName> PreferredVegetationTags;
};

/**
 * Chasing mob behavior settings (catchable creatures)
 */
USTRUCT(BlueprintType)
struct FChasingBehaviorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	float FleeSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	float DetectionRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	float CatchRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	float StaminaDrainRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	TSubclassOf<AActor> RewardItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chasing")
	int32 RewardQuantity = 1;
};

/**
 * Neutral mob behavior settings
 */
USTRUCT(BlueprintType)
struct FNeutralBehaviorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	EGroupBehavior GroupType = EGroupBehavior::Solo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	int32 MinGroupSize = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	int32 MaxGroupSize = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	float GroupCohesionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	float AggroRadius = 0.0f; // 0 = only when attacked

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	bool bCallForHelp = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Neutral")
	float HelpCallRadius = 500.0f;
};

/**
 * Area guard behavior settings (territorial with anti-cheese)
 */
USTRUCT(BlueprintType)
struct FAreaGuardSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard")
	FVector TerritoryCenter = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard")
	float TerritoryRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard")
	float AggroRadius = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard")
	bool bEnableAntiCheese = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard|Anti-Cheese")
	float AntiCheeseDetectionRadius = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard|Anti-Cheese")
	int32 PunishmentMobCount = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard|Anti-Cheese")
	float PunishmentMobStrengthMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard|Anti-Cheese")
	TSubclassOf<AActor> PunishmentMobClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard|Loot")
	TArray<TSubclassOf<AActor>> UniqueWeaponDrops;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Area Guard|Loot")
	TArray<TSubclassOf<AActor>> UniqueArmorDrops;
};

/**
 * Aggressive mob behavior settings (zone bosses)
 */
USTRUCT(BlueprintType)
struct FAggressiveBehaviorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	float DetectionRadius = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	float ChaseSpeed = 700.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	bool bChaseIndefinitely = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	bool bRespectSafeZones = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	float SafeZoneStareDistance = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	bool bIsBoss = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aggressive")
	TArray<EBossAttackPattern> BossAttackPatterns;
};

/**
 * Combat attack data for AI
 */
USTRUCT(BlueprintType)
struct FAIAttackData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	FName AttackName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Damage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Range = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float Cooldown = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float WindupTime = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float RecoveryTime = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bIsRanged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	EMagicElement ElementType = EMagicElement::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;
};

/**
 * Melee combo chain data
 */
USTRUCT(BlueprintType)
struct FMeleeComboChain
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	TArray<FAIAttackData> ComboAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	float ComboWindow = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 MinComboLength = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combo")
	int32 MaxComboLength = 4;
};

/**
 * Boss phase data
 */
USTRUCT(BlueprintType)
struct FBossPhaseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	int32 PhaseNumber = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	float HealthThreshold = 1.0f; // Percentage (0.75 = 75% health)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	TArray<EBossAttackPattern> AvailablePatterns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	float AttackSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	bool bSummonMinionsOnStart = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	int32 MinionCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Phase")
	TSubclassOf<AActor> MinionClass;
};
