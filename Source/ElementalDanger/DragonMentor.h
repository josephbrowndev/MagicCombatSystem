// Dragon Mentor - Teaches elemental magic

#pragma once

#include "CoreMinimal.h"
#include "MentorBase.h"
#include "DragonMentor.generated.h"

UCLASS()
class ELEMENTALDANGER_API ADragonMentor : public AMentorBase
{
	GENERATED_BODY()

public:
	ADragonMentor();

protected:
	virtual void BeginPlay() override;
	virtual void InitializeMentor() override;

public:
	// ============================================
	// Dragon Properties
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dragon")
	FLinearColor DragonColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dragon")
	float DragonScale;

	// ============================================
	// Elemental Teaching
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Dragon")
	void TeachBasicElementalSpells(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Dragon")
	void TeachAdvancedElementalSpells(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Dragon")
	void UnlockCombinationMagic(ANinjaWizardCharacter* Player, EMagicElement SecondaryElement);

	// ============================================
	// Dragon Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Dragon|Events")
	void OnElementUnlockedForPlayer(ANinjaWizardCharacter* Player, EMagicElement Element);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dragon|Events")
	void OnCombinationUnlockedForPlayer(ANinjaWizardCharacter* Player, ECombinedMagic CombinedMagic);

private:
	void SetupElementalSpells();
};
