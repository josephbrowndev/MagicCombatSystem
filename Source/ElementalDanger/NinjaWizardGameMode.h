// Ninja Wizard Game Mode

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NinjaWizardGameMode.generated.h"

UCLASS()
class ELEMENTALDANGER_API ANinjaWizardGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANinjaWizardGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// ============================================
	// Game Configuration
	// ============================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	bool bIsOpenWorld;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	int32 CurrentLevelNumber;

	// ============================================
	// Progression
	// ============================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Settings|Progression")
	TArray<FName> LevelProgression;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void LoadNextLevel();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void TransitionToOpenWorld();

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Game|Events")
	void OnLevelComplete();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game|Events")
	void OnOpenWorldUnlocked();
};
