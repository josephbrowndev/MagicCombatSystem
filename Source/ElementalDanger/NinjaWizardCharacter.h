// Main player character class for Ninja Wizard

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MagicTypes.h"
#include "NinjaWizardCharacter.generated.h"

class UMagicComponent;
class UWeaponComponent;
class UMasteryManagerComponent;

UCLASS()
class ELEMENTALDANGER_API ANinjaWizardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANinjaWizardCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ============================================
	// Resource Management (Mana & Stamina)
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	float MaxMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resources")
	float CurrentMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	float ManaRegenRate; // Per second

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	float MaxStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resources")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
	float StaminaRegenRate; // Per second

	// ============================================
	// Components
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMagicComponent* MagicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMasteryManagerComponent* MasteryManager;

	// ============================================
	// Resource Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool ConsumeMana(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void RestoreMana(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	void RestoreStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Resources")
	float GetManaPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Resources")
	float GetStaminaPercentage() const;

	// ============================================
	// Combat Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CastSpell(const FSpellData& SpellData);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformWeaponAttack(EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EquipWeapon(EWeaponStyle WeaponStyle);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EWeaponStyle CurrentWeaponStyle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bIsCasting;

private:
	void RegenerateMana(float DeltaTime);
	void RegenerateStamina(float DeltaTime);

	float CastingTimeRemaining;
};
