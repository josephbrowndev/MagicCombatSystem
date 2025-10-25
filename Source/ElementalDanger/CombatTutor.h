// Combat Tutor - Teaches weapon styles (Warrior and Ninja)

#pragma once

#include "CoreMinimal.h"
#include "MentorBase.h"
#include "CombatTutor.generated.h"

UENUM(BlueprintType)
enum class ETutorSpecialization : uint8
{
	Warrior     UMETA(DisplayName = "Warrior"),  // Teaches Sword and Spear
	Ninja       UMETA(DisplayName = "Ninja")     // Teaches Fist and Bow
};

UCLASS()
class ELEMENTALDANGER_API ACombatTutor : public AMentorBase
{
	GENERATED_BODY()

public:
	ACombatTutor();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeMentor() override;

public:
	// ============================================
	// Tutor Properties
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tutor")
	ETutorSpecialization Specialization;

	// ============================================
	// Weapon Teaching
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void TeachBasicWeaponTechniques(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void TeachAdvancedWeaponTechniques(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Tutor")
	void TeachComboAttacks(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Tutor")
	TArray<EWeaponStyle> GetSpecializedWeapons() const;

	// ============================================
	// Training Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Tutor|Events")
	void OnWeaponStyleUnlockedForPlayer(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintImplementableEvent, Category = "Tutor|Events")
	void OnAdvancedTechniqueUnlocked(ANinjaWizardCharacter* Player, const FWeaponAttackData& Attack);

private:
	void SetupWarriorWeapons();
	void SetupNinjaWeapons();
	void SetupWeaponAttacks();
};
