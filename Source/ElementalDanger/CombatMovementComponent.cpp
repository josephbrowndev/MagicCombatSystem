// Combat Movement Component Implementation

#include "CombatMovementComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatMovementComponent::UCombatMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = ECombatMovementState::Idle;
	StateTimer = 0.0f;
	bIsInvulnerable = false;

	// Block config
	BlockDamageReduction = 0.7f;
	BlockStaminaCost = 10.0f;
	bCanBlockMagic = true;
	MagicBlockReduction = 0.5f;

	// Parry config
	ParryWindowDuration = 0.3f;
	ParryStaminaCost = 15.0f;
	ParryCounterMultiplier = 2.0f;
	ParryStunDuration = 2.0f;

	// Dodge config
	DodgeDuration = 0.5f;
	DodgeDistance = 400.0f;
	DodgeStaminaCost = 20.0f;
	DodgeInvulnerabilityDuration = 0.3f;
	DodgeCooldown = 1.0f;

	// Slide config
	SlideDuration = 0.8f;
	SlideDistance = 600.0f;
	SlideStaminaCost = 15.0f;
	bCanSlideUnderAttacks = true;

	// Stun config
	DefaultStunDuration = 1.5f;
	StunRecoveryTime = 0.5f;
}

void UCombatMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner();
}

void UCombatMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateState(DeltaTime);

	// Update cooldown timers
	if (DodgeCooldownTimer > 0.0f)
	{
		DodgeCooldownTimer -= DeltaTime;
	}
}

// ============================================
// Block Functions
// ============================================

bool UCombatMovementComponent::StartBlocking()
{
	if (!CanPerformAction()) return false;

	// TODO: Check stamina
	// if (!HasEnoughStamina(BlockStaminaCost)) return false;

	ChangeState(ECombatMovementState::Blocking);
	OnBlockStarted();

	UE_LOG(LogTemp, Log, TEXT("Started blocking"));
	return true;
}

void UCombatMovementComponent::StopBlocking()
{
	if (CurrentState != ECombatMovementState::Blocking) return;

	ChangeState(ECombatMovementState::Idle);
	OnBlockStopped();

	UE_LOG(LogTemp, Log, TEXT("Stopped blocking"));
}

float UCombatMovementComponent::CalculateBlockedDamage(float IncomingDamage, bool bIsMagicDamage) const
{
	if (!IsBlocking()) return IncomingDamage;

	// Can't block magic if setting is disabled
	if (bIsMagicDamage && !bCanBlockMagic)
	{
		return IncomingDamage;
	}

	// Apply damage reduction
	float ReductionPercent = bIsMagicDamage ? MagicBlockReduction : BlockDamageReduction;
	float BlockedAmount = IncomingDamage * ReductionPercent;
	float FinalDamage = IncomingDamage - BlockedAmount;

	UE_LOG(LogTemp, Log, TEXT("Blocked %.1f damage (%.1f%%)"), BlockedAmount, ReductionPercent * 100.0f);

	return FinalDamage;
}

// ============================================
// Parry Functions
// ============================================

bool UCombatMovementComponent::StartParry()
{
	if (!CanPerformAction()) return false;

	// TODO: Check stamina
	// if (!HasEnoughStamina(ParryStaminaCost)) return false;

	ChangeState(ECombatMovementState::Parrying);
	StateTimer = ParryWindowDuration;

	OnParryStarted();

	UE_LOG(LogTemp, Log, TEXT("Parry window started (%.2fs)"), ParryWindowDuration);
	return true;
}

bool UCombatMovementComponent::IsInParryWindow() const
{
	return CurrentState == ECombatMovementState::Parrying && StateTimer > 0.0f;
}

FParryResult UCombatMovementComponent::AttemptParry(AActor* Attacker, float IncomingDamage)
{
	FParryResult Result;
	Result.ParriedAttacker = Attacker;

	if (!IsInParryWindow())
	{
		Result.bSuccessful = false;
		OnParryFailed();
		UE_LOG(LogTemp, Warning, TEXT("Parry failed - not in window"));
		return Result;
	}

	// Successful parry!
	Result.bSuccessful = true;
	Result.CounterDamageMultiplier = ParryCounterMultiplier;
	Result.bStunAttacker = true;

	// Stun the attacker
	if (Attacker)
	{
		if (UCombatMovementComponent* AttackerMovement = Attacker->FindComponentByClass<UCombatMovementComponent>())
		{
			AttackerMovement->ApplyStun(ParryStunDuration);
		}
	}

	OnParrySuccessful(Attacker);

	// Return to idle after successful parry
	ChangeState(ECombatMovementState::Idle);

	UE_LOG(LogTemp, Log, TEXT("PARRY SUCCESSFUL! Stunned attacker for %.1fs"), ParryStunDuration);

	return Result;
}

// ============================================
// Dodge Functions
// ============================================

bool UCombatMovementComponent::StartDodge(FVector Direction)
{
	if (!CanPerformAction()) return false;
	if (DodgeCooldownTimer > 0.0f) return false;

	// TODO: Check stamina
	// if (!HasEnoughStamina(DodgeStaminaCost)) return false;

	DodgeDirection = Direction.GetSafeNormal();
	DodgeProgress = 0.0f;
	StateTimer = DodgeDuration;
	bIsInvulnerable = true;

	ChangeState(ECombatMovementState::Dodging);
	DodgeCooldownTimer = DodgeCooldown;

	OnDodgeStarted(DodgeDirection);

	UE_LOG(LogTemp, Log, TEXT("Dodge started in direction: %s"), *DodgeDirection.ToString());
	return true;
}

void UCombatMovementComponent::ProcessDodge(float DeltaTime)
{
	if (!OwnerCharacter) return;

	DodgeProgress += DeltaTime;

	// Calculate dodge movement
	float Alpha = DodgeProgress / DodgeDuration;
	float DistanceThisFrame = (DodgeDistance / DodgeDuration) * DeltaTime;

	// Apply movement
	FVector NewLocation = OwnerCharacter->GetActorLocation() + (DodgeDirection * DistanceThisFrame);
	OwnerCharacter->SetActorLocation(NewLocation, true);

	// Remove invulnerability after I-frames expire
	if (DodgeProgress >= DodgeInvulnerabilityDuration)
	{
		bIsInvulnerable = false;
	}

	// End dodge when duration expires
	if (StateTimer <= 0.0f)
	{
		ChangeState(ECombatMovementState::Idle);
		bIsInvulnerable = false;
		OnDodgeCompleted();

		UE_LOG(LogTemp, Log, TEXT("Dodge completed"));
	}
}

// ============================================
// Slide Functions
// ============================================

bool UCombatMovementComponent::StartSlide(FVector Direction)
{
	if (!CanPerformAction()) return false;

	// TODO: Check stamina
	// if (!HasEnoughStamina(SlideStaminaCost)) return false;

	SlideDirection = Direction.GetSafeNormal();
	SlideProgress = 0.0f;
	StateTimer = SlideDuration;

	// Can dodge under attacks while sliding
	if (bCanSlideUnderAttacks)
	{
		bIsInvulnerable = true;
	}

	ChangeState(ECombatMovementState::Sliding);

	OnSlideStarted(SlideDirection);

	UE_LOG(LogTemp, Log, TEXT("Slide started in direction: %s"), *SlideDirection.ToString());
	return true;
}

void UCombatMovementComponent::ProcessSlide(float DeltaTime)
{
	if (!OwnerCharacter) return;

	SlideProgress += DeltaTime;

	// Calculate slide movement (with ease-out for natural deceleration)
	float Alpha = SlideProgress / SlideDuration;
	float EasedAlpha = 1.0f - FMath::Pow(1.0f - Alpha, 2.0f); // Quadratic ease-out

	float DistanceThisFrame = (SlideDistance / SlideDuration) * DeltaTime * (1.0f - EasedAlpha);

	// Apply movement
	FVector NewLocation = OwnerCharacter->GetActorLocation() + (SlideDirection * DistanceThisFrame);
	OwnerCharacter->SetActorLocation(NewLocation, true);

	// End slide when duration expires
	if (StateTimer <= 0.0f)
	{
		ChangeState(ECombatMovementState::Idle);
		bIsInvulnerable = false;
		OnSlideCompleted();

		UE_LOG(LogTemp, Log, TEXT("Slide completed"));
	}
}

// ============================================
// Stun Functions
// ============================================

void UCombatMovementComponent::ApplyStun(float Duration)
{
	StateTimer = Duration;
	ChangeState(ECombatMovementState::Stunned);

	OnStunApplied(Duration);

	UE_LOG(LogTemp, Warning, TEXT("STUNNED for %.1f seconds!"), Duration);
}

void UCombatMovementComponent::BreakStun()
{
	if (CurrentState != ECombatMovementState::Stunned) return;

	ChangeState(ECombatMovementState::Idle);
	OnStunBroken();

	UE_LOG(LogTemp, Log, TEXT("Stun broken early"));
}

float UCombatMovementComponent::GetRemainingStunTime() const
{
	if (CurrentState != ECombatMovementState::Stunned) return 0.0f;
	return StateTimer;
}

// ============================================
// State Query
// ============================================

bool UCombatMovementComponent::CanPerformAction() const
{
	// Can't perform actions while stunned
	if (CurrentState == ECombatMovementState::Stunned) return false;

	// Can't perform actions while dodging or sliding
	if (CurrentState == ECombatMovementState::Dodging) return false;
	if (CurrentState == ECombatMovementState::Sliding) return false;

	return true;
}

// ============================================
// Internal State Management
// ============================================

void UCombatMovementComponent::UpdateState(float DeltaTime)
{
	StateTimer -= DeltaTime;

	switch (CurrentState)
	{
	case ECombatMovementState::Parrying:
		if (StateTimer <= 0.0f)
		{
			// Parry window expired without success
			ChangeState(ECombatMovementState::Idle);
			OnParryFailed();
			UE_LOG(LogTemp, Log, TEXT("Parry window expired"));
		}
		break;

	case ECombatMovementState::Dodging:
		ProcessDodge(DeltaTime);
		break;

	case ECombatMovementState::Sliding:
		ProcessSlide(DeltaTime);
		break;

	case ECombatMovementState::Stunned:
		if (StateTimer <= 0.0f)
		{
			ChangeState(ECombatMovementState::Idle);
			OnStunBroken();
			UE_LOG(LogTemp, Log, TEXT("Recovered from stun"));
		}
		break;

	default:
		break;
	}
}

void UCombatMovementComponent::ChangeState(ECombatMovementState NewState)
{
	if (CurrentState == NewState) return;

	ECombatMovementState OldState = CurrentState;
	CurrentState = NewState;

	OnStateChanged(NewState);

	UE_LOG(LogTemp, Log, TEXT("Combat state changed: %d -> %d"), (int32)OldState, (int32)NewState);
}
