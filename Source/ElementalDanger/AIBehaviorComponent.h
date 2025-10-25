// AI Behavior Component - Handles mob behavior patterns

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBehaviorTypes.h"
#include "AIBehaviorComponent.generated.h"

class ACombatEntity;
class ANinjaWizardCharacter;
class AAIController;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UAIBehaviorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIBehaviorComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	EAIBehaviorType BehaviorType = EAIBehaviorType::Passive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior")
	EAIState CurrentState = EAIState::Idle;

	// Behavior settings for each type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior|Passive")
	FPassiveBehaviorSettings PassiveSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior|Chasing")
	FChasingBehaviorSettings ChasingSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior|Neutral")
	FNeutralBehaviorSettings NeutralSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior|Area Guard")
	FAreaGuardSettings AreaGuardSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Behavior|Aggressive")
	FAggressiveBehaviorSettings AggressiveSettings;

	// ============================================
	// State Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	void SetBehaviorType(EAIBehaviorType NewType);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	void SetState(EAIState NewState);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	EAIState GetCurrentState() const { return CurrentState; }

	// ============================================
	// Passive Behavior
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Passive")
	void StartWandering();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Passive")
	void LookForVegetation();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Passive")
	void StartEating(AActor* Vegetation);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Passive")
	void StartResting();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Passive")
	void FleeFromThreat(AActor* Threat);

	// ============================================
	// Chasing Behavior (Catchable)
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Chasing")
	void DetectPlayer();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Chasing")
	void FleeFromPlayer(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Chasing")
	void OnCaught(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Chasing")
	void DrainStamina(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Chasing")
	bool IsExhausted() const;

	// ============================================
	// Neutral Behavior
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Neutral")
	void FormGroup(TArray<AActor*> NearbyAllies);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Neutral")
	void MaintainGroupCohesion();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Neutral")
	void OnAttackedByPlayer(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Neutral")
	void CallForHelp();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Neutral")
	void RespondToHelpCall(AActor* Caller);

	// ============================================
	// Area Guard Behavior
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	void SetTerritory(FVector Center, float Radius);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	bool IsInTerritory(FVector Location) const;

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	bool IsPlayerInAggroRange(ANinjaWizardCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	void GuardTerritory();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	void ReturnToTerritory();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	void DetectCheesing(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	void SpawnPunishmentMobs(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Area Guard")
	void DropUniqueLoot();

	// ============================================
	// Aggressive Behavior (Zone Bosses)
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Aggressive")
	void DetectPlayerInZone();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Aggressive")
	void ChasePlayerIndefinitely(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Aggressive")
	bool IsPlayerInSafeZone(ANinjaWizardCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Aggressive")
	void StareAtPlayerInSafeZone(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Aggressive")
	void ResumeChaseWhenPlayerLeavesSafeZone(ANinjaWizardCharacter* Player);

	// ============================================
	// Group Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Group")
	void RegisterWithGroup(AActor* GroupLeader);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Group")
	void LeaveGroup();

	UFUNCTION(BlueprintCallable, Category = "AI Behavior|Group")
	TArray<AActor*> GetGroupMembers() const { return GroupMembers; }

	// ============================================
	// Utility Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	ANinjaWizardCharacter* FindNearestPlayer() const;

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	float GetDistanceToPlayer(ANinjaWizardCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	FVector GetRandomLocationInRadius(FVector Origin, float Radius) const;

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	void MoveToLocation(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "AI Behavior")
	void StopMovement();

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnStateChanged(EAIState OldState, EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnDetectedPlayer(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnLostPlayer(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnEnteredCombat(AActor* Enemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnExitedCombat();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnTerritoryInvaded(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnCheesingDetected(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI Behavior|Events")
	void OnCaughtByPlayer(ANinjaWizardCharacter* Player);

protected:
	// References
	UPROPERTY()
	ACombatEntity* OwnerEntity;

	UPROPERTY()
	AAIController* AIController;

	UPROPERTY()
	ANinjaWizardCharacter* TargetPlayer;

	// Group management
	UPROPERTY()
	TArray<AActor*> GroupMembers;

	UPROPERTY()
	AActor* GroupLeader;

	// State tracking
	FVector SpawnLocation;
	FVector WanderTarget;
	AActor* CurrentVegetation;

	float CurrentStamina;
	float TimeSinceLastAction;
	float TimeSinceLastWander;
	float TimeSinceLastEat;

	bool bIsInCombat;
	bool bIsFleeing;
	bool bIsExhausted;
	bool bCheesingSeen;
	bool bInSafeZone;

	// Timers
	FTimerHandle WanderTimerHandle;
	FTimerHandle EatTimerHandle;
	FTimerHandle RestTimerHandle;
	FTimerHandle PatrolTimerHandle;

	// Internal behavior updates
	void UpdatePassiveBehavior(float DeltaTime);
	void UpdateChasingBehavior(float DeltaTime);
	void UpdateNeutralBehavior(float DeltaTime);
	void UpdateAreaGuardBehavior(float DeltaTime);
	void UpdateAggressiveBehavior(float DeltaTime);

	// Helper functions
	bool CanSeeActor(AActor* Target) const;
	void RotateTowards(AActor* Target, float DeltaTime);
	AActor* FindNearestVegetation() const;
};
