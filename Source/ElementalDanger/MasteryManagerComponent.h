// Mastery Manager Component - Tracks player progression for magic and weapons

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicTypes.h"
#include "MasteryManagerComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UMasteryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMasteryManagerComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Magic Mastery
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mastery|Magic")
	TMap<EMagicElement, FMasteryData> MagicMasteryMap;

	UFUNCTION(BlueprintCallable, Category = "Mastery|Magic")
	int32 GetMagicMastery(EMagicElement Element) const;

	UFUNCTION(BlueprintCallable, Category = "Mastery|Magic")
	void AddMagicExperience(EMagicElement Element, float Experience);

	UFUNCTION(BlueprintCallable, Category = "Mastery|Magic")
	void SetMagicMastery(EMagicElement Element, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Mastery|Magic")
	float GetMagicExperiencePercent(EMagicElement Element) const;

	// ============================================
	// Weapon Mastery
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mastery|Weapon")
	TMap<EWeaponStyle, FMasteryData> WeaponMasteryMap;

	UFUNCTION(BlueprintCallable, Category = "Mastery|Weapon")
	int32 GetWeaponMastery(EWeaponStyle WeaponStyle) const;

	UFUNCTION(BlueprintCallable, Category = "Mastery|Weapon")
	void AddWeaponExperience(EWeaponStyle WeaponStyle, float Experience);

	UFUNCTION(BlueprintCallable, Category = "Mastery|Weapon")
	void SetWeaponMastery(EWeaponStyle WeaponStyle, int32 Level);

	UFUNCTION(BlueprintCallable, Category = "Mastery|Weapon")
	float GetWeaponExperiencePercent(EWeaponStyle WeaponStyle) const;

	// ============================================
	// Progression Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Mastery|Events")
	void OnMagicMasteryLevelUp(EMagicElement Element, int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mastery|Events")
	void OnWeaponMasteryLevelUp(EWeaponStyle WeaponStyle, int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mastery|Events")
	void OnMasteryMilestone(EMagicElement Element, int32 Level);

	// ============================================
	// Progression Helpers
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Mastery")
	TArray<EMagicElement> GetMasteredElements(int32 MinimumLevel = 100) const;

	UFUNCTION(BlueprintCallable, Category = "Mastery")
	TArray<EWeaponStyle> GetMasteredWeapons(int32 MinimumLevel = 100) const;

	UFUNCTION(BlueprintCallable, Category = "Mastery")
	int32 GetTotalMagicMasteryLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Mastery")
	int32 GetTotalWeaponMasteryLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Mastery")
	bool HasMasteredAllElements() const;

	UFUNCTION(BlueprintCallable, Category = "Mastery")
	bool HasMasteredAllWeapons() const;

private:
	void InitializeMasteryMaps();
	float CalculateExperienceForNextLevel(int32 CurrentLevel) const;
	void ProcessLevelUp(EMagicElement Element, FMasteryData& MasteryData);
	void ProcessLevelUp(EWeaponStyle WeaponStyle, FMasteryData& MasteryData);
};
