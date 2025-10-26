// Main player character class for Ninja Wizard

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MagicTypes.h"
#include "NinjaWizardCharacter.generated.h"

class UMagicComponent;
class UWeaponComponent;
class UMasteryManagerComponent;
class UPlayerAttributeComponent;
class USummonManagerComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IInteractableInterface;
class UDamageCalculationComponent;
class UCombatMovementComponent;
class UGrappleComponent;
class UHealingComponent;
class UWeaponReturnComponent;
class USkillTreeComponent;
class UInventoryComponent;

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
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMagicComponent* MagicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMasteryManagerComponent* MasteryManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UPlayerAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USummonManagerComponent* SummonManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDamageCalculationComponent* DamageCalculationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCombatMovementComponent* CombatMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGrappleComponent* GrappleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealingComponent* HealingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponReturnComponent* WeaponReturnComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkillTreeComponent* SkillTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	// Note: Soul bonding is handled by SummonManager component

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

	// ============================================
	// Camera & Movement
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseSensitivity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bInvertCameraY;

	// ============================================
	// Enhanced Input
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CastSpellAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;

	// ============================================
	// Interaction System
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionCheckFrequency;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	AActor* FocusedInteractable;

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void PerformInteractionCheck();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void BeginInteract();

	UFUNCTION(BlueprintCallable, Category = "Interaction")
	AActor* GetFocusedInteractable() const { return FocusedInteractable; }

protected:
	// Input callbacks
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartJump();
	void StopJump();
	void Interact();
	void StartCastSpell();
	void StartAttack();

private:
	void RegenerateMana(float DeltaTime);
	void RegenerateStamina(float DeltaTime);

	float CastingTimeRemaining;
	float InteractionCheckTimer;
};
