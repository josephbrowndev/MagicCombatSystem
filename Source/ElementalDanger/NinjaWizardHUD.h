// Ninja Wizard HUD - Manages UI display

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "NinjaWizardHUD.generated.h"

class UDialogueWidget;
class UUserWidget;
class AMentorBase;
class ANinjaWizardCharacter;

/**
 * HUD class for the Ninja Wizard game
 * Manages all UI elements
 */
UCLASS()
class ELEMENTALDANGER_API ANinjaWizardHUD : public AHUD
{
	GENERATED_BODY()

public:
	ANinjaWizardHUD();

protected:
	virtual void BeginPlay() override;

public:
	virtual void DrawHUD() override;

	// ============================================
	// Widget Classes
	// ============================================

	/** Main player HUD widget class (set in Blueprint) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> PlayerHUDClass;

	/** Dialogue widget class (set in Blueprint) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;

	/** Interaction prompt widget class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<UUserWidget> InteractionPromptClass;

	// ============================================
	// Widget Instances
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UUserWidget* PlayerHUDWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UDialogueWidget* DialogueWidget;

	UPROPERTY(BlueprintReadOnly, Category = "Widgets")
	UUserWidget* InteractionPromptWidget;

	// ============================================
	// HUD Functions
	// ============================================

	/**
	 * Show the dialogue UI for a mentor
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowDialogue(AMentorBase* Mentor, ANinjaWizardCharacter* Player);

	/**
	 * Hide the dialogue UI
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideDialogue();

	/**
	 * Show interaction prompt
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowInteractionPrompt(const FText& PromptText);

	/**
	 * Hide interaction prompt
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void HideInteractionPrompt();

	/**
	 * Toggle player HUD visibility
	 */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void TogglePlayerHUD(bool bVisible);

protected:
	void CreatePlayerHUD();
};
