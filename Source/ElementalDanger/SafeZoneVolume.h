// Safe Zone Volume - Protects players from aggressive mobs

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SafeZoneVolume.generated.h"

class UBoxComponent;

UCLASS()
class ELEMENTALDANGER_API ASafeZoneVolume : public AActor
{
	GENERATED_BODY()

public:
	ASafeZoneVolume();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ============================================
	// Components
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* SafeZoneBox;

	// ============================================
	// Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	FString ZoneName = "Safe Zone";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	bool bShowDebugBounds = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	FLinearColor DebugColor = FLinearColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	bool bPreventCombat = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	bool bPreventEnemyEntry = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	bool bHealPlayers = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Safe Zone")
	float HealRate = 5.0f; // HP per second

	// ============================================
	// Safe Zone Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Safe Zone")
	bool IsActorInSafeZone(AActor* Actor) const;

	UFUNCTION(BlueprintCallable, Category = "Safe Zone")
	void DrawDebugBounds();

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Safe Zone|Events")
	void OnPlayerEntered(AActor* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "Safe Zone|Events")
	void OnPlayerExited(AActor* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "Safe Zone|Events")
	void OnEnemyAttemptedEntry(AActor* Enemy);

protected:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	TArray<AActor*> PlayersInZone;
};
