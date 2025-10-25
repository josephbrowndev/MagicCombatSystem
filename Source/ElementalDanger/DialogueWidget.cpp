// Dialogue Widget Implementation

#include "DialogueWidget.h"
#include "MentorBase.h"
#include "NinjaWizardCharacter.h"

void UDialogueWidget::InitializeDialogue(AMentorBase* Mentor, ANinjaWizardCharacter* Player)
{
	if (!Mentor || !Player)
	{
		return;
	}

	CurrentMentor = Mentor;
	PlayerCharacter = Player;
	bIsDialogueActive = true;

	// Call Blueprint event
	OnDialogueInitialized(Mentor, Player);

	// Set input mode to UI
	if (APlayerController* PC = Player->GetController<APlayerController>())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UDialogueWidget::CloseDialogue()
{
	bIsDialogueActive = false;

	// Restore game input mode
	if (PlayerCharacter)
	{
		if (APlayerController* PC = PlayerCharacter->GetController<APlayerController>())
		{
			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}

	// Call Blueprint event
	OnDialogueClosed();

	// Remove from parent
	RemoveFromParent();
}

void UDialogueWidget::DisplayDialogue(const FText& DialogueText)
{
	OnDialogueDisplayed(DialogueText);
}

void UDialogueWidget::ShowDialogueOptions()
{
	// This will be implemented in Blueprint
	// Show buttons for:
	// - Learn Spells
	// - Learn Weapon Techniques
	// - View Quests
	// - Improve Relationship
	// - Exit
}
