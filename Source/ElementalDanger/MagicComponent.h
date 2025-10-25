// Magic Component - Handles spell casting and magic combinations

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicTypes.h"
#include "MagicComponent.generated.h"

class ANinjaWizardCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UMagicComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMagicComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Learned Spells
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magic")
	TArray<FSpellData> LearnedSpells;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magic")
	TArray<EMagicElement> UnlockedElements;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magic")
	TArray<ECombinedMagic> UnlockedCombinations;

	// ============================================
	// Combination Recipes
	// ============================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magic|Combinations")
	TArray<FMagicCombination> CombinationRecipes;

	// ============================================
	// Spell Casting
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Magic")
	bool CastSpell(const FSpellData& SpellData);

	UFUNCTION(BlueprintCallable, Category = "Magic")
	bool CanCastSpell(const FSpellData& SpellData) const;

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void LearnSpell(const FSpellData& SpellData);

	UFUNCTION(BlueprintCallable, Category = "Magic")
	void UnlockElement(EMagicElement Element);

	UFUNCTION(BlueprintCallable, Category = "Magic")
	bool HasElement(EMagicElement Element) const;

	// ============================================
	// Magic Combinations
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Magic|Combinations")
	ECombinedMagic GetCombinedMagic(EMagicElement Element1, EMagicElement Element2) const;

	UFUNCTION(BlueprintCallable, Category = "Magic|Combinations")
	bool CanUseCombination(EMagicElement Element1, EMagicElement Element2) const;

	UFUNCTION(BlueprintCallable, Category = "Magic|Combinations")
	void UnlockCombination(ECombinedMagic CombinedMagic);

	UFUNCTION(BlueprintCallable, Category = "Magic|Combinations")
	TArray<ECombinedMagic> GetAvailableCombinations() const;

	// ============================================
	// Spell Effects
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Magic|Effects")
	void OnSpellCast(const FSpellData& SpellData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Magic|Effects")
	void OnElementUnlocked(EMagicElement Element);

	UFUNCTION(BlueprintImplementableEvent, Category = "Magic|Effects")
	void OnCombinationUnlocked(ECombinedMagic CombinedMagic);

private:
	UPROPERTY()
	ANinjaWizardCharacter* OwnerCharacter;

	void InitializeCombinationRecipes();
	float CalculateManaCost(const FSpellData& SpellData) const;
};
