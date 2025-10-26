// Healing Component - Manages healing restrictions and sources

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionTypes.h"
#include "HealingComponent.generated.h"

class UInventoryComponent;
class ASafeZoneVolume;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UHealingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealingComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Healing Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing|Config")
	FHealingRestrictions HealingRestrictions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing|Config")
	float SafeZoneHealRate = 5.0f; // HP per second in safe zones

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing|Config")
	float ClericHealRate = 10.0f; // HP per second from cleric

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing|Config")
	float OutOfCombatRegenRate = 2.0f; // HP per second when out of combat

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Healing|Config")
	float OutOfCombatDelay = 8.0f; // Seconds after combat before regen starts

	// ============================================
	// Current Healing State
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Healing|State")
	bool bIsInCombat = false;

	UPROPERTY(BlueprintReadOnly, Category = "Healing|State")
	float TimeSinceLastCombat = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Healing|State")
	bool bIsInSafeZone = false;

	UPROPERTY(BlueprintReadOnly, Category = "Healing|State")
	bool bIsBeingHealedByCleric = false;

	UPROPERTY(BlueprintReadOnly, Category = "Healing|State")
	AActor* CurrentClericHealer = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Healing|State")
	ASafeZoneVolume* CurrentSafeZone = nullptr;

	// ============================================
	// Healing Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Healing")
	bool CanHeal(EHealingSource Source, float CurrentHealthPercent) const;

	UFUNCTION(BlueprintCallable, Category = "Healing")
	float ApplyHealing(float HealAmount, EHealingSource Source);

	UFUNCTION(BlueprintCallable, Category = "Healing")
	bool TryUsePotion(FName PotionID);

	// ============================================
	// Safe Zone Healing
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Healing|SafeZone")
	void EnterSafeZone(ASafeZoneVolume* SafeZone);

	UFUNCTION(BlueprintCallable, Category = "Healing|SafeZone")
	void ExitSafeZone();

	UFUNCTION(BlueprintCallable, Category = "Healing|SafeZone")
	bool IsInSafeZone() const { return bIsInSafeZone; }

	// ============================================
	// Cleric Healing
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Healing|Cleric")
	void StartClericHealing(AActor* ClericHealer);

	UFUNCTION(BlueprintCallable, Category = "Healing|Cleric")
	void StopClericHealing();

	UFUNCTION(BlueprintCallable, Category = "Healing|Cleric")
	bool IsBeingHealedByCleric() const { return bIsBeingHealedByCleric; }

	// ============================================
	// Combat State Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Healing|Combat")
	void EnterCombat();

	UFUNCTION(BlueprintCallable, Category = "Healing|Combat")
	void ExitCombat();

	UFUNCTION(BlueprintCallable, Category = "Healing|Combat")
	bool IsInCombat() const { return bIsInCombat; }

	UFUNCTION(BlueprintCallable, Category = "Healing|Combat")
	bool CanStartOutOfCombatRegen() const;

	// ============================================
	// Healing Validation
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Healing|Validation")
	bool IsHealingSourceAllowed(EHealingSource Source) const;

	UFUNCTION(BlueprintCallable, Category = "Healing|Validation")
	float GetCurrentHealthPercent() const;

	UFUNCTION(BlueprintCallable, Category = "Healing|Validation")
	bool CanUsePotions(float CurrentHealthPercent) const;

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnHealingReceived(float HealAmount, EHealingSource Source);

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnPotionRejected(FName PotionID, float CurrentHealthPercent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnEnteredSafeZone(ASafeZoneVolume* SafeZone);

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnExitedSafeZone();

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnClericHealingStarted(AActor* Cleric);

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnClericHealingStopped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnOutOfCombatRegenStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnOutOfCombatRegenStopped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Healing|Events")
	void OnCombatStateChanged(bool bInCombat);

protected:
	// ============================================
	// Internal Healing Logic
	// ============================================

	void ProcessSafeZoneHealing(float DeltaTime);
	void ProcessClericHealing(float DeltaTime);
	void ProcessOutOfCombatRegen(float DeltaTime);

	// ============================================
	// References
	// ============================================

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	AActor* OwnerCharacter;

	// ============================================
	// Helper Functions
	// ============================================

	float GetMaxHealth() const;
	float GetCurrentHealth() const;
	void HealCharacter(float Amount);
};
