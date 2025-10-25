// Dialogue Widget Base - For mentor conversations

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class AMentorBase;
class ANinjaWizardCharacter;

/**
 * Base class for dialogue UI widgets
 * Extend this in Blueprint to create your custom dialogue UI
 */
UCLASS()
class ELEMENTALDANGER_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initialize the dialogue with a mentor
	 * @param Mentor - The mentor to talk to
	 * @param Player - The player character
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void InitializeDialogue(AMentorBase* Mentor, ANinjaWizardCharacter* Player);

	/**
	 * Close the dialogue window
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void CloseDialogue();

	/**
	 * Display a specific dialogue line
	 * @param DialogueText - The text to display
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void DisplayDialogue(const FText& DialogueText);

	/**
	 * Show available dialogue options to the player
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	virtual void ShowDialogueOptions();

	// ============================================
	// Blueprint Events
	// ============================================

	/**
	 * Called when dialogue is initialized
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueInitialized(AMentorBase* Mentor, ANinjaWizardCharacter* Player);

	/**
	 * Called when dialogue is closed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueClosed();

	/**
	 * Called when a new dialogue line is displayed
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueDisplayed(const FText& DialogueText);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	AMentorBase* CurrentMentor;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	ANinjaWizardCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bIsDialogueActive;
};
