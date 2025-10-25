// Interactable Interface - For objects/NPCs that can be interacted with

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

class ANinjaWizardCharacter;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for any actor that can be interacted with by the player
 */
class ELEMENTALDANGER_API IInteractableInterface
{
	GENERATED_BODY()

public:
	/**
	 * Called when the player interacts with this object
	 * @param Player - The player character interacting
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(ANinjaWizardCharacter* Player);

	/**
	 * Gets the interaction text to display to the player
	 * @return Text to show in UI (e.g., "Press E to Talk")
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText() const;

	/**
	 * Checks if this object can currently be interacted with
	 * @param Player - The player attempting to interact
	 * @return True if interaction is allowed
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(ANinjaWizardCharacter* Player) const;

	/**
	 * Called when player looks at this interactable (for highlighting)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionFocus();

	/**
	 * Called when player stops looking at this interactable
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void OnInteractionUnfocus();
};
