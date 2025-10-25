// Ninja Wizard HUD Implementation

#include "NinjaWizardHUD.h"
#include "DialogueWidget.h"
#include "Blueprint/UserWidget.h"
#include "MentorBase.h"
#include "NinjaWizardCharacter.h"

ANinjaWizardHUD::ANinjaWizardHUD()
{
	PlayerHUDWidget = nullptr;
	DialogueWidget = nullptr;
	InteractionPromptWidget = nullptr;
}

void ANinjaWizardHUD::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerHUD();
}

void ANinjaWizardHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ANinjaWizardHUD::CreatePlayerHUD()
{
	if (PlayerHUDClass)
	{
		PlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		if (PlayerHUDWidget)
		{
			PlayerHUDWidget->AddToViewport();
		}
	}

	// Create interaction prompt widget (hidden by default)
	if (InteractionPromptClass)
	{
		InteractionPromptWidget = CreateWidget<UUserWidget>(GetWorld(), InteractionPromptClass);
		if (InteractionPromptWidget)
		{
			InteractionPromptWidget->AddToViewport();
			InteractionPromptWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ANinjaWizardHUD::ShowDialogue(AMentorBase* Mentor, ANinjaWizardCharacter* Player)
{
	if (!DialogueWidgetClass || !Mentor || !Player)
	{
		return;
	}

	// Close existing dialogue if any
	if (DialogueWidget)
	{
		DialogueWidget->CloseDialogue();
		DialogueWidget = nullptr;
	}

	// Create new dialogue widget
	DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
	if (DialogueWidget)
	{
		DialogueWidget->AddToViewport(10); // Higher Z-order
		DialogueWidget->InitializeDialogue(Mentor, Player);
	}

	// Hide interaction prompt
	HideInteractionPrompt();
}

void ANinjaWizardHUD::HideDialogue()
{
	if (DialogueWidget)
	{
		DialogueWidget->CloseDialogue();
		DialogueWidget = nullptr;
	}
}

void ANinjaWizardHUD::ShowInteractionPrompt(const FText& PromptText)
{
	if (InteractionPromptWidget)
	{
		InteractionPromptWidget->SetVisibility(ESlateVisibility::Visible);
		// You can bind the text in Blueprint or expose a function to set it
	}
}

void ANinjaWizardHUD::HideInteractionPrompt()
{
	if (InteractionPromptWidget)
	{
		InteractionPromptWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ANinjaWizardHUD::TogglePlayerHUD(bool bVisible)
{
	if (PlayerHUDWidget)
	{
		PlayerHUDWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}
