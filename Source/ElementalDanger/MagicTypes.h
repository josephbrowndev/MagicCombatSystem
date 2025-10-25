// Magic System Types and Enumerations

#pragma once

#include "CoreMinimal.h"
#include "MagicTypes.generated.h"

/**
 * Base elements available in the magic system
 */
UENUM(BlueprintType)
enum class EMagicElement : uint8
{
	Fire        UMETA(DisplayName = "Fire"),
	Water       UMETA(DisplayName = "Water"),
	Earth       UMETA(DisplayName = "Earth"),
	Air         UMETA(DisplayName = "Air"),
	Lightning   UMETA(DisplayName = "Lightning"),
	Light       UMETA(DisplayName = "Light"),
	Dark        UMETA(DisplayName = "Dark"),
	Poison      UMETA(DisplayName = "Poison"),
	Ice         UMETA(DisplayName = "Ice"),
	None        UMETA(DisplayName = "None")
};

/**
 * Combined/Fusion magic types
 */
UENUM(BlueprintType)
enum class ECombinedMagic : uint8
{
	Lava            UMETA(DisplayName = "Lava"),           // Fire + Earth
	Storm           UMETA(DisplayName = "Storm"),          // Lightning + Air
	Curse           UMETA(DisplayName = "Curse"),          // Dark + Poison
	Inferno         UMETA(DisplayName = "Inferno"),        // Fire + Air
	Glacier         UMETA(DisplayName = "Glacier"),        // Water + Ice
	Solar           UMETA(DisplayName = "Solar"),          // Light + Fire
	Hellfire        UMETA(DisplayName = "Hellfire"),       // Dark + Fire
	Swamp           UMETA(DisplayName = "Swamp"),          // Water + Earth
	ElectroFlood    UMETA(DisplayName = "Electro Flood"),  // Lightning + Water
	Steam           UMETA(DisplayName = "Steam"),          // Fire + Water
	Crystal         UMETA(DisplayName = "Crystal"),        // Earth + Ice
	Blizzard        UMETA(DisplayName = "Blizzard"),       // Ice + Air
	Radiance        UMETA(DisplayName = "Radiance"),       // Light + Air
	Shadow          UMETA(DisplayName = "Shadow"),         // Dark + Air
	Toxin           UMETA(DisplayName = "Toxin"),          // Poison + Water
	Magma           UMETA(DisplayName = "Magma"),          // Fire + Earth (alternative)
	Plasma          UMETA(DisplayName = "Plasma"),         // Lightning + Fire
	Void            UMETA(DisplayName = "Void"),           // Dark + Air
	Nature          UMETA(DisplayName = "Nature"),         // Earth + Water
	Corruption      UMETA(DisplayName = "Corruption"),     // Dark + Earth
	None            UMETA(DisplayName = "None")
};

/**
 * Weapon styles available to the player
 */
UENUM(BlueprintType)
enum class EWeaponStyle : uint8
{
	TwoHandedSword  UMETA(DisplayName = "Two-Handed Sword"),
	Spear           UMETA(DisplayName = "Spear"),
	Fist            UMETA(DisplayName = "Fist/Martial Arts"),
	Bow             UMETA(DisplayName = "Bow"),
	None            UMETA(DisplayName = "None")
};

/**
 * Mentor types for learning
 */
UENUM(BlueprintType)
enum class EMentorType : uint8
{
	DragonFire      UMETA(DisplayName = "Fire Dragon"),
	DragonWater     UMETA(DisplayName = "Water Dragon"),
	DragonEarth     UMETA(DisplayName = "Earth Dragon"),
	DragonAir       UMETA(DisplayName = "Air Dragon"),
	DragonLightning UMETA(DisplayName = "Lightning Dragon"),
	DragonLight     UMETA(DisplayName = "Light Dragon"),
	DragonDark      UMETA(DisplayName = "Dark Dragon"),
	DragonPoison    UMETA(DisplayName = "Poison Dragon"),
	DragonIce       UMETA(DisplayName = "Ice Dragon"),
	WarriorTutor    UMETA(DisplayName = "Warrior Tutor"),
	NinjaTutor      UMETA(DisplayName = "Ninja Tutor"),
	None            UMETA(DisplayName = "None")
};

/**
 * Structure for magic element combination recipes
 */
USTRUCT(BlueprintType)
struct FMagicCombination
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMagicElement Element1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMagicElement Element2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECombinedMagic ResultingMagic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredMastery; // Minimum mastery level required for both elements

	FMagicCombination()
		: Element1(EMagicElement::None)
		, Element2(EMagicElement::None)
		, ResultingMagic(ECombinedMagic::None)
		, RequiredMastery(25)
	{}
};

/**
 * Structure for spell data
 */
USTRUCT(BlueprintType)
struct FSpellData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpellName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMagicElement Element;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECombinedMagic CombinedElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ManaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CastTime; // In seconds

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredMastery;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;

	FSpellData()
		: SpellName(NAME_None)
		, Element(EMagicElement::None)
		, CombinedElement(ECombinedMagic::None)
		, ManaCost(10.0f)
		, BaseDamage(50.0f)
		, CastTime(1.0f)
		, RequiredMastery(0)
	{}
};

/**
 * Structure for mastery tracking
 */
USTRUCT(BlueprintType)
struct FMasteryData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Level; // 0-100

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Experience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExperienceToNextLevel;

	FMasteryData()
		: Level(0)
		, Experience(0.0f)
		, ExperienceToNextLevel(100.0f)
	{}
};
