// Damage Calculation Component - Handles elemental interactions and damage scaling

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicTypes.h"
#include "DamageCalculationComponent.generated.h"

USTRUCT(BlueprintType)
struct FDamageCalculationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float FinalDamage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float BaseDamage = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float ElementalMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float LevelScaling = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float WisdomScaling = 1.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	bool bIsWeakness = false;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	bool bIsDualElement = false;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FString DamageBreakdown;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UDamageCalculationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamageCalculationComponent();

protected:
	virtual void BeginPlay() override;

public:
	// ============================================
	// Damage Calculation Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
	float WeaknessMultiplier = 1.5f; // Opposite elements deal 1.5x damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
	float LevelScalingFactor = 0.05f; // 5% damage increase per level

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
	float WisdomScalingFactor = 0.02f; // 2% damage increase per wisdom point

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage|Config")
	bool bDualElementIgnoresWeakness = true; // Dual magic ignores elemental weaknesses

	// ============================================
	// Elemental Weakness System
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Damage|Elements")
	bool IsElementWeakTo(EMagicType AttackElement, EMagicType DefenderElement) const;

	UFUNCTION(BlueprintCallable, Category = "Damage|Elements")
	EMagicType GetOppositeElement(EMagicType Element) const;

	UFUNCTION(BlueprintCallable, Category = "Damage|Elements")
	bool HasOppositeElement(EMagicType Element) const;

	// ============================================
	// Damage Calculation
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Damage")
	FDamageCalculationResult CalculateDamage(
		float BaseDamage,
		EMagicType AttackElement,
		EMagicType DefenderElement,
		int32 AttackerLevel,
		int32 AttackerWisdom,
		bool bIsDualElement = false
	) const;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	float CalculateSimpleDamage(
		float BaseDamage,
		EMagicType AttackElement,
		EMagicType DefenderElement
	) const;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	float GetLevelScalingMultiplier(int32 Level) const;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	float GetWisdomScalingMultiplier(int32 Wisdom) const;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	float GetElementalMultiplier(EMagicType AttackElement, EMagicType DefenderElement, bool bIsDualElement) const;

	// ============================================
	// Utility Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Damage|Utility")
	FString GetDamageBreakdownString(const FDamageCalculationResult& Result) const;

	UFUNCTION(BlueprintCallable, Category = "Damage|Utility")
	bool IsCriticalHit(float CritChance) const;

	UFUNCTION(BlueprintCallable, Category = "Damage|Utility")
	float ApplyCriticalMultiplier(float Damage, float CritMultiplier = 2.0f) const;

protected:
	// ============================================
	// Internal Elemental Weakness Map
	// ============================================

	// Maps elements to their opposites for weakness calculation
	TMap<EMagicType, EMagicType> ElementalOpposites;

	void InitializeElementalOpposites();
};
