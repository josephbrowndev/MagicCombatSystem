// Progression System Types - Skills, Inventory, Gear, Items

#pragma once

#include "CoreMinimal.h"
#include "MagicTypes.h"
#include "AttributeTypes.h"
#include "ProgressionTypes.generated.h"

// Forward declarations
class UTexture2D;
class AActor;
class ACombatEntity;

// ============================================
// Item Rarity System (7 Tiers)
// ============================================

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Special     UMETA(DisplayName = "Special"),
	Rare        UMETA(DisplayName = "Rare"),
	Mythic      UMETA(DisplayName = "Mythic"),
	Legendary   UMETA(DisplayName = "Legendary"),
	Unique      UMETA(DisplayName = "Unique"),
	Dragon      UMETA(DisplayName = "Dragon")
};

// ============================================
// Item Types
// ============================================

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon          UMETA(DisplayName = "Weapon"),
	Armor           UMETA(DisplayName = "Armor"),
	Potion          UMETA(DisplayName = "Potion"),
	Material        UMETA(DisplayName = "Crafting Material"),
	QuestItem       UMETA(DisplayName = "Quest Item"),
	Consumable      UMETA(DisplayName = "Consumable"),
	Throwable       UMETA(DisplayName = "Throwable"),
	KeyItem         UMETA(DisplayName = "Key Item")
};

UENUM(BlueprintType)
enum class EArmorSlot : uint8
{
	Head        UMETA(DisplayName = "Head"),
	Chest       UMETA(DisplayName = "Chest"),
	Legs        UMETA(DisplayName = "Legs"),
	Hands       UMETA(DisplayName = "Hands"),
	Feet        UMETA(DisplayName = "Feet"),
	Accessory1  UMETA(DisplayName = "Accessory 1"),
	Accessory2  UMETA(DisplayName = "Accessory 2"),
	Cloak       UMETA(DisplayName = "Cloak")
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary     UMETA(DisplayName = "Primary Weapon"),
	Secondary   UMETA(DisplayName = "Secondary Weapon"),
	Tertiary    UMETA(DisplayName = "Tertiary Weapon"),
	Quaternary  UMETA(DisplayName = "Quaternary Weapon")
};

// ============================================
// Skill Tree System
// ============================================

UENUM(BlueprintType)
enum class ESkillTreeType : uint8
{
	// Elemental Trees
	FireTree        UMETA(DisplayName = "Fire Mastery"),
	WaterTree       UMETA(DisplayName = "Water Mastery"),
	EarthTree       UMETA(DisplayName = "Earth Mastery"),
	AirTree         UMETA(DisplayName = "Air Mastery"),
	LightningTree   UMETA(DisplayName = "Lightning Mastery"),
	LightTree       UMETA(DisplayName = "Light Mastery"),
	DarkTree        UMETA(DisplayName = "Dark Mastery"),
	PoisonTree      UMETA(DisplayName = "Poison Mastery"),
	IceTree         UMETA(DisplayName = "Ice Mastery"),

	// Weapon Trees
	SwordTree       UMETA(DisplayName = "Sword Mastery"),
	SpearTree       UMETA(DisplayName = "Spear Mastery"),
	FistTree        UMETA(DisplayName = "Fist Mastery"),
	BowTree         UMETA(DisplayName = "Bow Mastery")
};

UENUM(BlueprintType)
enum class ESkillNodeType : uint8
{
	Passive         UMETA(DisplayName = "Passive Bonus"),
	Active          UMETA(DisplayName = "Active Ability"),
	Ultimate        UMETA(DisplayName = "Ultimate Ability"),
	Modifier        UMETA(DisplayName = "Ability Modifier"),
	Stat            UMETA(DisplayName = "Stat Increase"),
	Special         UMETA(DisplayName = "Special Effect")
};

UENUM(BlueprintType)
enum class ESkillEffectType : uint8
{
	DamageBoost         UMETA(DisplayName = "Damage Boost"),
	CooldownReduction   UMETA(DisplayName = "Cooldown Reduction"),
	ManaCostReduction   UMETA(DisplayName = "Mana Cost Reduction"),
	AOEIncrease         UMETA(DisplayName = "AOE Increase"),
	RangeIncrease       UMETA(DisplayName = "Range Increase"),
	StatusEffect        UMETA(DisplayName = "Apply Status Effect"),
	ChainEffect         UMETA(DisplayName = "Chain to Targets"),
	SummonBoost         UMETA(DisplayName = "Summon Enhancement"),
	ComboExtension      UMETA(DisplayName = "Combo Extension"),
	ElementalFusion     UMETA(DisplayName = "Elemental Fusion"),
	WeaponReturn        UMETA(DisplayName = "Weapon Return"),
	CriticalBoost       UMETA(DisplayName = "Critical Chance/Damage"),
	LifeSteal           UMETA(DisplayName = "Life Steal"),
	Reflect             UMETA(DisplayName = "Damage Reflect")
};

// ============================================
// Skill Tree Structs
// ============================================

USTRUCT(BlueprintType)
struct FSkillNodeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	FName NodeID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	FText NodeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	ESkillNodeType NodeType = ESkillNodeType::Passive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	ESkillEffectType EffectType = ESkillEffectType::DamageBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	int32 RequiredLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	int32 SkillPointCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	TArray<FName> PrerequisiteNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	float EffectValue = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	bool bIsDiscovered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	bool bIsUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node")
	UTexture2D* NodeIcon = nullptr;

	// For skill tree UI positioning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Node|UI")
	FVector2D UIPosition = FVector2D::ZeroVector;

	FSkillNodeData()
		: NodeType(ESkillNodeType::Passive)
		, EffectType(ESkillEffectType::DamageBoost)
		, RequiredLevel(1)
		, SkillPointCost(1)
		, EffectValue(10.0f)
		, bIsDiscovered(false)
		, bIsUnlocked(false)
		, UIPosition(FVector2D::ZeroVector)
	{}
};

USTRUCT(BlueprintType)
struct FSkillTreeData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	ESkillTreeType TreeType = ESkillTreeType::FireTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	FText TreeName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	TArray<FSkillNodeData> SkillNodes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	bool bIsDiscovered = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	int32 PointsSpent = 0;

	FSkillTreeData()
		: TreeType(ESkillTreeType::FireTree)
		, bIsDiscovered(false)
		, PointsSpent(0)
	{}
};

// ============================================
// Item System Structs
// ============================================

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemType ItemType = EItemType::Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	UTexture2D* ItemIcon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 StackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 MaxStackSize = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 Value = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsStackable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bIsQuestItem = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanDrop = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	bool bCanSell = true;

	FItemData()
		: ItemType(EItemType::Weapon)
		, Rarity(EItemRarity::Common)
		, StackSize(1)
		, MaxStackSize(1)
		, Value(0)
		, bIsQuestItem(false)
		, bCanDrop(true)
		, bCanSell(true)
	{}
};

USTRUCT(BlueprintType)
struct FWeaponData : public FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponStyle WeaponStyle = EWeaponStyle::TwoHandedSword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseDamage = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EMagicElement ElementalAffinity = EMagicElement::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ElementalDamageBonus = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<FName> SpecialAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bCanReturn = false; // For "Return" skill

	FWeaponData()
	{
		ItemType = EItemType::Weapon;
		WeaponStyle = EWeaponStyle::TwoHandedSword;
		BaseDamage = 20.0f;
		AttackSpeed = 1.0f;
		ElementalAffinity = EMagicElement::None;
		ElementalDamageBonus = 0.0f;
		bCanReturn = false;
	}
};

USTRUCT(BlueprintType)
struct FArmorData : public FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	EArmorSlot ArmorSlot = EArmorSlot::Chest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float Defense = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float MagicResistance = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 StrengthBonus = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 WisdomBonus = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 PerceptionBonus = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	int32 AgilityBonus = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	TArray<FName> SetBonusID; // For armor sets

	FArmorData()
	{
		ItemType = EItemType::Armor;
		ArmorSlot = EArmorSlot::Chest;
		Defense = 10.0f;
		MagicResistance = 0.0f;
		StrengthBonus = 0;
		WisdomBonus = 0;
		PerceptionBonus = 0;
		AgilityBonus = 0;
	}
};

USTRUCT(BlueprintType)
struct FPotionData : public FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float HealthRestore = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float HealAmount = 50.0f; // Alias for HealthRestore for compatibility

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float ManaRestore = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float StaminaRestore = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float MinHealthPercentRequired = 0.1f; // 10% minimum health to use

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	float CooldownTime = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Potion")
	TArray<FName> BuffEffects;

	FPotionData()
	{
		ItemType = EItemType::Potion;
		MaxStackSize = 99;
		HealthRestore = 50.0f;
		ManaRestore = 0.0f;
		StaminaRestore = 0.0f;
		MinHealthPercentRequired = 0.1f;
		CooldownTime = 5.0f;
	}
};

// ============================================
// Equipment Loadout
// ============================================

USTRUCT(BlueprintType)
struct FEquipmentLoadout
{
	GENERATED_BODY()

	// Weapons (4 slots for quick swap) - using fixed size arrays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Weapons")
	FWeaponData EquippedWeapons[4];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Weapons")
	bool bWeaponSlotFilled[4];

	// Armor (8 slots) - using fixed size arrays
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Armor")
	FArmorData EquippedArmor[8];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment|Armor")
	bool bArmorSlotFilled[8];

	FEquipmentLoadout()
	{
		// Initialize arrays
		for (int32 i = 0; i < 4; i++)
		{
			bWeaponSlotFilled[i] = false;
		}
		for (int32 i = 0; i < 8; i++)
		{
			bArmorSlotFilled[i] = false;
		}
	}

	// Calculated stats from equipment
	UPROPERTY(BlueprintReadOnly, Category = "Equipment|Stats")
	float TotalDefense = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment|Stats")
	float TotalMagicResistance = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment|Stats")
	int32 TotalStrengthBonus = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment|Stats")
	int32 TotalWisdomBonus = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment|Stats")
	int32 TotalPerceptionBonus = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Equipment|Stats")
	int32 TotalAgilityBonus = 0;
};

// ============================================
// Healing System
// ============================================

UENUM(BlueprintType)
enum class EHealingSource : uint8
{
	SafeZone        UMETA(DisplayName = "Safe Zone"),
	ClericNPC       UMETA(DisplayName = "Cleric NPC"),
	ClericSummon    UMETA(DisplayName = "Cleric Summon"),
	Potion          UMETA(DisplayName = "Health Potion"),
	RestArea        UMETA(DisplayName = "Rest Area")
};

USTRUCT(BlueprintType)
struct FHealingRestrictions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	bool bAllowNaturalRegeneration = false; // Should be false!

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	bool bRequireSafeZoneForHealing = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	float MinHealthPercentForPotions = 0.1f; // 10%

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	float PotionGlobalCooldown = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing")
	TArray<EHealingSource> AllowedHealingSources;

	FHealingRestrictions()
		: bAllowNaturalRegeneration(false)
		, bRequireSafeZoneForHealing(true)
		, MinHealthPercentForPotions(0.1f)
		, PotionGlobalCooldown(5.0f)
	{
		AllowedHealingSources.Add(EHealingSource::SafeZone);
		AllowedHealingSources.Add(EHealingSource::ClericNPC);
		AllowedHealingSources.Add(EHealingSource::ClericSummon);
		AllowedHealingSources.Add(EHealingSource::Potion);
	}
};

// ============================================
// Rarity Colors and Multipliers
// ============================================

USTRUCT(BlueprintType)
struct FRaritySettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity")
	EItemRarity Rarity = EItemRarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity")
	float StatMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rarity")
	int32 MaxEnchantmentSlots = 0;

	FRaritySettings()
		: Rarity(EItemRarity::Common)
		, Color(FLinearColor::White)
		, StatMultiplier(1.0f)
		, MaxEnchantmentSlots(0)
	{}
};
