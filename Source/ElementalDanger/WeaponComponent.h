// Weapon Component - Handles weapon combat and styles

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicTypes.h"
#include "WeaponComponent.generated.h"

class ANinjaWizardCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Weapon System
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<EWeaponStyle> UnlockedWeaponStyles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TArray<FWeaponAttackData> LearnedAttacks;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	EWeaponStyle CurrentWeaponStyle;

	// ============================================
	// Combat
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool PerformAttack(EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanPerformAttack(const FWeaponAttackData& AttackData) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipWeapon(EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void UnlockWeaponStyle(EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool HasWeaponStyle(EWeaponStyle WeaponStyle) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void LearnAttack(const FWeaponAttackData& AttackData);

	// ============================================
	// Combo System
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Combo")
	int32 CurrentComboCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Combo")
	float ComboResetTime; // Time before combo resets

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combo")
	void ResetCombo();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Combo")
	void IncrementCombo();

	// ============================================
	// Magic Infusion
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Infusion")
	EMagicElement InfusedElement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Infusion")
	ECombinedMagic InfusedCombinedMagic;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Infusion")
	void InfuseWeapon(EMagicElement Element);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Infusion")
	void InfuseWeaponWithCombination(ECombinedMagic CombinedMagic);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Infusion")
	void ClearInfusion();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Infusion")
	float GetInfusedDamageBonus() const;

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Events")
	void OnAttackPerformed(const FWeaponAttackData& AttackData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Events")
	void OnWeaponEquipped(EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Events")
	void OnComboIncremented(int32 ComboCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon|Events")
	void OnWeaponInfused(EMagicElement Element);

private:
	UPROPERTY()
	ANinjaWizardCharacter* OwnerCharacter;

	float TimeSinceLastAttack;
	float CalculateStaminaCost(const FWeaponAttackData& AttackData) const;
	float CalculateDamage(const FWeaponAttackData& AttackData) const;
};
