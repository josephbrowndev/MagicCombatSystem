// Combat Movement Component - Handles block, parry, dodge, slide, and stun mechanics

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatMovementComponent.generated.h"

UENUM(BlueprintType)
enum class ECombatMovementState : uint8
{
	Idle           UMETA(DisplayName = "Idle"),
	Blocking       UMETA(DisplayName = "Blocking"),
	Dodging        UMETA(DisplayName = "Dodging"),
	Sliding        UMETA(DisplayName = "Sliding"),
	Stunned        UMETA(DisplayName = "Stunned"),
	Parrying       UMETA(DisplayName = "Parrying")
};

USTRUCT(BlueprintType)
struct FParryResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Parry")
	bool bSuccessful = false;

	UPROPERTY(BlueprintReadOnly, Category = "Parry")
	AActor* ParriedAttacker = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Parry")
	float CounterDamageMultiplier = 2.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Parry")
	bool bStunAttacker = true;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UCombatMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Current State
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Combat Movement|State")
	ECombatMovementState CurrentState = ECombatMovementState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Movement|State")
	float StateTimer = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Movement|State")
	bool bIsInvulnerable = false;

	// ============================================
	// Block Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Block")
	float BlockDamageReduction = 0.7f; // Reduce 70% of damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Block")
	float BlockStaminaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Block")
	bool bCanBlockMagic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Block")
	float MagicBlockReduction = 0.5f; // Only block 50% of magic damage

	// ============================================
	// Parry Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Parry")
	float ParryWindowDuration = 0.3f; // 300ms timing window

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Parry")
	float ParryStaminaCost = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Parry")
	float ParryCounterMultiplier = 2.0f; // Deal 2x damage on successful parry counter

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Parry")
	float ParryStunDuration = 2.0f; // Stun attacker for 2 seconds

	// ============================================
	// Dodge Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Dodge")
	float DodgeDuration = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Dodge")
	float DodgeDistance = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Dodge")
	float DodgeStaminaCost = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Dodge")
	float DodgeInvulnerabilityDuration = 0.3f; // I-frames

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Dodge")
	float DodgeCooldown = 1.0f;

	// ============================================
	// Slide Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Slide")
	float SlideDuration = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Slide")
	float SlideDistance = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Slide")
	float SlideStaminaCost = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Slide")
	bool bCanSlideUnderAttacks = true;

	// ============================================
	// Stun Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Stun")
	float DefaultStunDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat Movement|Stun")
	float StunRecoveryTime = 0.5f;

	// ============================================
	// Block Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Block")
	bool StartBlocking();

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Block")
	void StopBlocking();

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Block")
	bool IsBlocking() const { return CurrentState == ECombatMovementState::Blocking; }

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Block")
	float CalculateBlockedDamage(float IncomingDamage, bool bIsMagicDamage) const;

	// ============================================
	// Parry Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Parry")
	bool StartParry();

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Parry")
	bool IsInParryWindow() const;

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Parry")
	FParryResult AttemptParry(AActor* Attacker, float IncomingDamage);

	// ============================================
	// Dodge Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Dodge")
	bool StartDodge(FVector Direction);

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Dodge")
	bool IsDodging() const { return CurrentState == ECombatMovementState::Dodging; }

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Dodge")
	bool HasDodgeInvulnerability() const { return bIsInvulnerable; }

	// ============================================
	// Slide Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Slide")
	bool StartSlide(FVector Direction);

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Slide")
	bool IsSliding() const { return CurrentState == ECombatMovementState::Sliding; }

	// ============================================
	// Stun Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Stun")
	void ApplyStun(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Stun")
	void BreakStun();

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Stun")
	bool IsStunned() const { return CurrentState == ECombatMovementState::Stunned; }

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|Stun")
	float GetRemainingStunTime() const;

	// ============================================
	// State Query
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|State")
	bool CanPerformAction() const;

	UFUNCTION(BlueprintCallable, Category = "Combat Movement|State")
	ECombatMovementState GetCurrentState() const { return CurrentState; }

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnBlockStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnBlockStopped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnDamageBlocked(float BlockedAmount, bool bIsMagic);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnParryStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnParrySuccessful(AActor* ParriedAttacker);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnParryFailed();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnDodgeStarted(FVector Direction);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnDodgeCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnSlideStarted(FVector Direction);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnSlideCompleted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnStunApplied(float Duration);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnStunBroken();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat Movement|Events")
	void OnStateChanged(ECombatMovementState NewState);

protected:
	// ============================================
	// Internal State Management
	// ============================================

	void UpdateState(float DeltaTime);
	void ChangeState(ECombatMovementState NewState);
	void ProcessDodge(float DeltaTime);
	void ProcessSlide(float DeltaTime);

	// ============================================
	// Cooldown Tracking
	// ============================================

	UPROPERTY()
	float DodgeCooldownTimer = 0.0f;

	UPROPERTY()
	AActor* OwnerCharacter;

	// ============================================
	// Movement Tracking
	// ============================================

	FVector DodgeDirection = FVector::ZeroVector;
	FVector SlideDirection = FVector::ZeroVector;
	float DodgeProgress = 0.0f;
	float SlideProgress = 0.0f;
};
