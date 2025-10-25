// Ninja Wizard Game Mode Implementation

#include "NinjaWizardGameMode.h"
#include "NinjaWizardCharacter.h"
#include "Kismet/GameplayStatics.h"

ANinjaWizardGameMode::ANinjaWizardGameMode()
{
	// Set default pawn class to our ninja wizard character
	DefaultPawnClass = ANinjaWizardCharacter::StaticClass();

	bIsOpenWorld = false;
	CurrentLevelNumber = 1;
}

void ANinjaWizardGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Initialize level progression if not set
	if (LevelProgression.Num() == 0)
	{
		// Default level progression - can be customized in Blueprint
		LevelProgression.Add(FName("Tutorial"));
		LevelProgression.Add(FName("FireDragonTemple"));
		LevelProgression.Add(FName("WaterDragonShrine"));
		LevelProgression.Add(FName("EarthDragonCavern"));
		LevelProgression.Add(FName("AirDragonPeak"));
		LevelProgression.Add(FName("LightningDragonStorm"));
		LevelProgression.Add(FName("LightDragonSanctuary"));
		LevelProgression.Add(FName("DarkDragonAbyss"));
		LevelProgression.Add(FName("PoisonDragonSwamp"));
		LevelProgression.Add(FName("IceDragonGlacier"));
		LevelProgression.Add(FName("OpenWorld"));
	}
}

void ANinjaWizardGameMode::LoadNextLevel()
{
	if (CurrentLevelNumber < LevelProgression.Num())
	{
		CurrentLevelNumber++;
		FName NextLevel = LevelProgression[CurrentLevelNumber - 1];

		// Check if transitioning to open world
		if (NextLevel == FName("OpenWorld"))
		{
			TransitionToOpenWorld();
		}
		else
		{
			UGameplayStatics::OpenLevel(GetWorld(), NextLevel);
		}

		OnLevelComplete();
	}
}

void ANinjaWizardGameMode::TransitionToOpenWorld()
{
	bIsOpenWorld = true;
	OnOpenWorldUnlocked();

	// Load the open world map
	UGameplayStatics::OpenLevel(GetWorld(), FName("OpenWorld"));
}
