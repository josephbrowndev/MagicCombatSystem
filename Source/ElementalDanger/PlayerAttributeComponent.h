// Player Attribute Component - Manages STR, WIS, PER, AGI

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeTypes.h"
#include "PlayerAttributeComponent.generated.h"

class ANinjaWizardCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UPlayerAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttributeComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Core Attributes
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FAttributeData Strength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FAttributeData Wisdom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FAttributeData Perception;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	FAttributeData Agility;

	// ============================================
	// Derived Stats (Calculated from Attributes)
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Derived Stats")
	FDerivedStats DerivedStats;

	// ============================================
	// Attribute Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetAttributeValue(EPlayerAttribute Attribute) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetAttributeBaseValue(EPlayerAttribute Attribute, int32 Value);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void AddAttributeBonus(EPlayerAttribute Attribute, int32 Bonus);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void SetAttributeMultiplier(EPlayerAttribute Attribute, float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void IncreaseAttribute(EPlayerAttribute Attribute, int32 Amount);

	// ============================================
	// Derived Stat Calculation
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Derived Stats")
	void RecalculateDerivedStats();

	UFUNCTION(BlueprintCallable, Category = "Derived Stats")
	FDerivedStats GetDerivedStats() const { return DerivedStats; }

	// ============================================
	// Stat Application to Character
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	void ApplyAttributeEffects();

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes|Events")
	void OnAttributeChanged(EPlayerAttribute Attribute, int32 NewValue);

	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes|Events")
	void OnDerivedStatsRecalculated(const FDerivedStats& NewStats);

	// ============================================
	// Perception-Specific Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Attributes|Perception")
	bool CanDetectThreat(AActor* PotentialThreat) const;

	UFUNCTION(BlueprintCallable, Category = "Attributes|Perception")
	void TriggerSlowMotion();

	UFUNCTION(BlueprintCallable, Category = "Attributes|Perception")
	void EndSlowMotion();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes|Perception")
	void OnSlowMotionTriggered();

	UFUNCTION(BlueprintImplementableEvent, Category = "Attributes|Perception")
	void OnSlowMotionEnded();

	// ============================================
	// Utility Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetBlockChance() const { return DerivedStats.BlockChance; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetDodgeChance() const { return DerivedStats.DodgeChance; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetCriticalHitChance() const { return DerivedStats.CriticalHitChance; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetMaxSummonCount() const { return DerivedStats.MaxSummonCount; }

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	int32 GetSummonCapacity() const { return DerivedStats.SummonCapacity; }

private:
	UPROPERTY()
	ANinjaWizardCharacter* OwnerCharacter;

	bool bInSlowMotion;
	FTimerHandle SlowMotionTimerHandle;

	// Calculation helpers
	void CalculateStrengthDerivedStats();
	void CalculateWisdomDerivedStats();
	void CalculatePerceptionDerivedStats();
	void CalculateAgilityDerivedStats();
};
