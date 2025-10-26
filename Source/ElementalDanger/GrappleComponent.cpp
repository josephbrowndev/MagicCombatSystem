// Grapple Component Implementation

#include "GrappleComponent.h"
#include "CombatEntity.h"
#include "CombatMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"

UGrappleComponent::UGrappleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = EGrappleState::Idle;
	SelectedThrowDirection = EGrappleDirection::PullToPlayer;

	MaxGrappleRange = 3000.0f;
	GrappleSpeed = 5000.0f;
	PullSpeed = 1000.0f;
	ThrowSpeed = 1500.0f;
	GrappleCooldown = 2.0f;
	bCanGrappleEnemies = true;
	bCanGrappleEnvironment = true;

	ThrowUpForce = 800.0f;
	ThrowHorizontalForce = 600.0f;
	ThrowSlamForce = 1000.0f;
	StunDurationOnImpact = 2.0f;
	ImpactDamage = 50.0f;
	AirComboWindow = 3.0f;

	bShowGrappleLine = true;
	GrappleLineColor = FLinearColor::Yellow;
	GrappleElement = EMagicType::Lightning;
}

void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner();
}

void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGrapple(DeltaTime);

	// Update cooldown
	if (CooldownTimer > 0.0f)
	{
		CooldownTimer -= DeltaTime;
	}

	// Update air combo window
	if (bInAirCombo)
	{
		AirComboTimer -= DeltaTime;
		if (AirComboTimer <= 0.0f)
		{
			bInAirCombo = false;
		}
	}
}

// ============================================
// Grapple Shooting
// ============================================

bool UGrappleComponent::ShootGrapple(FVector TargetLocation)
{
	if (!CanShootGrapple()) return false;
	if (!OwnerCharacter) return false;

	GrappleStartLocation = OwnerCharacter->GetActorLocation();
	GrappleEndLocation = TargetLocation;
	GrappleProgress = 0.0f;

	// Check if target is in range
	float Distance = FVector::Dist(GrappleStartLocation, GrappleEndLocation);
	if (Distance > MaxGrappleRange)
	{
		OnGrappleFailed(TargetLocation);
		UE_LOG(LogTemp, Warning, TEXT("Grapple target out of range: %.1f / %.1f"), Distance, MaxGrappleRange);
		return false;
	}

	// Perform line trace to check for valid target
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GrappleStartLocation,
		GrappleEndLocation,
		ECC_Visibility,
		QueryParams
	);

	if (bHit)
	{
		GrappleEndLocation = HitResult.Location;
		CurrentTarget.TargetActor = HitResult.GetActor();
		CurrentTarget.TargetLocation = HitResult.Location;
		CurrentTarget.TargetType = GetTargetType(HitResult.GetActor());
		CurrentTarget.bIsValid = IsValidGrappleTarget(HitResult.GetActor());

		if (CurrentTarget.bIsValid)
		{
			ChangeState(EGrappleState::Shooting);
			CooldownTimer = GrappleCooldown;

			OnGrappleShot(GrappleEndLocation);

			UE_LOG(LogTemp, Log, TEXT("Grapple shot at target: %s"), *HitResult.GetActor()->GetName());
			return true;
		}
	}

	OnGrappleFailed(TargetLocation);
	UE_LOG(LogTemp, Warning, TEXT("No valid grapple target found"));
	return false;
}

bool UGrappleComponent::ShootGrappleAtActor(AActor* TargetActor)
{
	if (!TargetActor) return false;

	return ShootGrapple(TargetActor->GetActorLocation());
}

bool UGrappleComponent::CanShootGrapple() const
{
	if (CurrentState != EGrappleState::Idle) return false;
	if (CooldownTimer > 0.0f) return false;

	return true;
}

void UGrappleComponent::CancelGrapple()
{
	if (GrappleHookActor)
	{
		GrappleHookActor->Destroy();
		GrappleHookActor = nullptr;
	}

	CurrentTarget = FGrappleTarget();
	ChangeState(EGrappleState::Idle);

	UE_LOG(LogTemp, Log, TEXT("Grapple cancelled"));
}

// ============================================
// Target Selection
// ============================================

FGrappleTarget UGrappleComponent::FindGrappleTarget(FVector Direction, float MaxDistance)
{
	FGrappleTarget Target;

	if (!OwnerCharacter) return Target;

	FVector Start = OwnerCharacter->GetActorLocation();
	FVector End = Start + (Direction.GetSafeNormal() * MaxDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	if (bHit)
	{
		Target.TargetActor = HitResult.GetActor();
		Target.TargetLocation = HitResult.Location;
		Target.TargetType = GetTargetType(HitResult.GetActor());
		Target.bIsValid = IsValidGrappleTarget(HitResult.GetActor());
	}

	return Target;
}

bool UGrappleComponent::IsValidGrappleTarget(AActor* Target) const
{
	if (!Target) return false;

	EGrappleTargetType Type = GetTargetType(Target);

	if (Type == EGrappleTargetType::Enemy && !bCanGrappleEnemies) return false;
	if (Type == EGrappleTargetType::Environment && !bCanGrappleEnvironment) return false;

	return Type != EGrappleTargetType::Invalid;
}

EGrappleTargetType UGrappleComponent::GetTargetType(AActor* Target) const
{
	if (!Target) return EGrappleTargetType::Invalid;

	// Check if it's an enemy
	if (ACombatEntity* CombatActor = Cast<ACombatEntity>(Target))
	{
		// Don't grapple player summons
		if (!CombatActor->bIsPlayerSummon)
		{
			return EGrappleTargetType::Enemy;
		}
	}

	// Check if it's a static mesh or hittable object
	if (Target->GetComponentByClass(UStaticMeshComponent::StaticClass()))
	{
		return EGrappleTargetType::Environment;
	}

	// Check for objects with collision
	if (Target->GetComponentByClass(UPrimitiveComponent::StaticClass()))
	{
		return EGrappleTargetType::Object;
	}

	return EGrappleTargetType::Invalid;
}

// ============================================
// Enemy Manipulation
// ============================================

void UGrappleComponent::SetThrowDirection(EGrappleDirection Direction)
{
	SelectedThrowDirection = Direction;
	UE_LOG(LogTemp, Log, TEXT("Throw direction set to: %d"), (int32)Direction);
}

bool UGrappleComponent::PullEnemyToPlayer(AActor* Enemy)
{
	if (!Enemy || !OwnerCharacter) return false;

	CurrentTarget.TargetActor = Enemy;
	CurrentTarget.TargetLocation = Enemy->GetActorLocation();
	CurrentTarget.TargetType = EGrappleTargetType::Enemy;
	CurrentTarget.bIsValid = true;

	ChangeState(EGrappleState::PullingEnemy);

	OnEnemyPulled(Enemy);

	UE_LOG(LogTemp, Log, TEXT("Pulling enemy to player: %s"), *Enemy->GetName());
	return true;
}

bool UGrappleComponent::ThrowEnemy(AActor* Enemy, EGrappleDirection Direction)
{
	if (!Enemy || !OwnerCharacter) return false;

	CurrentTarget.TargetActor = Enemy;
	SelectedThrowDirection = Direction;

	ChangeState(EGrappleState::ThrowingEnemy);

	// Calculate throw velocity
	FVector ThrowVelocity = CalculateThrowVelocity(Direction);
	ApplyThrowForce(Enemy, ThrowVelocity);

	OnEnemyThrown(Enemy, Direction);

	// Start air combo window if throwing up
	if (Direction == EGrappleDirection::ThrowUp)
	{
		bInAirCombo = true;
		AirComboTimer = AirComboWindow;
		OnAirComboWindowStarted(Enemy);
	}

	UE_LOG(LogTemp, Log, TEXT("Throwing enemy %s in direction %d"), *Enemy->GetName(), (int32)Direction);

	return true;
}

void UGrappleComponent::ExecuteThrow()
{
	if (CurrentTarget.TargetActor)
	{
		ThrowEnemy(CurrentTarget.TargetActor, SelectedThrowDirection);
	}
}

// ============================================
// Combo System Integration
// ============================================

bool UGrappleComponent::IsInAirComboWindow() const
{
	return bInAirCombo && AirComboTimer > 0.0f;
}

void UGrappleComponent::StartAirCombo()
{
	if (!IsInAirComboWindow()) return;

	UE_LOG(LogTemp, Log, TEXT("Air combo started!"));
}

// ============================================
// Internal Logic
// ============================================

void UGrappleComponent::UpdateGrapple(float DeltaTime)
{
	switch (CurrentState)
	{
	case EGrappleState::Shooting:
		ProcessShooting(DeltaTime);
		break;

	case EGrappleState::PullingEnemy:
		ProcessPulling(DeltaTime);
		break;

	case EGrappleState::ThrowingEnemy:
		ProcessThrowing(DeltaTime);
		break;

	default:
		break;
	}

	// Draw grapple line for debugging
	if (bShowGrappleLine && CurrentState != EGrappleState::Idle && OwnerCharacter)
	{
		FVector Start = OwnerCharacter->GetActorLocation();
		FVector End = CurrentTarget.bIsValid ? CurrentTarget.TargetLocation : GrappleEndLocation;

		DrawDebugLine(GetWorld(), Start, End, GrappleLineColor.ToFColor(true), false, -1.0f, 0, 3.0f);
	}
}

void UGrappleComponent::ProcessShooting(float DeltaTime)
{
	GrappleProgress += DeltaTime;

	// Calculate hook position
	float TravelDistance = FVector::Dist(GrappleStartLocation, GrappleEndLocation);
	float TravelTime = TravelDistance / GrappleSpeed;

	if (GrappleProgress >= TravelTime)
	{
		// Hook reached target
		ChangeState(EGrappleState::Attached);
		OnGrappleAttached(CurrentTarget.TargetActor, CurrentTarget.TargetType);

		// Automatically pull enemy or wait for player input
		if (CurrentTarget.TargetType == EGrappleTargetType::Enemy)
		{
			// Player can now choose what to do with the enemy
			UE_LOG(LogTemp, Log, TEXT("Grapple attached to enemy - awaiting throw direction"));
		}
	}
}

void UGrappleComponent::ProcessPulling(float DeltaTime)
{
	if (!CurrentTarget.TargetActor || !OwnerCharacter) return;

	FVector CurrentLocation = CurrentTarget.TargetActor->GetActorLocation();
	FVector TargetLocation = OwnerCharacter->GetActorLocation();
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();

	// Move enemy toward player
	float DistanceThisFrame = PullSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + (Direction * DistanceThisFrame);

	CurrentTarget.TargetActor->SetActorLocation(NewLocation, true);

	// Check if enemy reached player
	float Distance = FVector::Dist(NewLocation, TargetLocation);
	if (Distance <= 150.0f) // Close enough
	{
		ChangeState(EGrappleState::Idle);
		UE_LOG(LogTemp, Log, TEXT("Enemy pulled to player - ready for combo"));
	}
}

void UGrappleComponent::ProcessThrowing(float DeltaTime)
{
	if (!CurrentTarget.TargetActor) return;

	// Check for impact with objects
	CheckThrowImpact(CurrentTarget.TargetActor);

	// Throw completes when enemy lands or after duration
	// For now, just reset after a delay
	// This would be better handled with physics simulation
}

void UGrappleComponent::ChangeState(EGrappleState NewState)
{
	if (CurrentState == NewState) return;

	CurrentState = NewState;
	OnGrappleStateChanged(NewState);

	UE_LOG(LogTemp, Log, TEXT("Grapple state changed to: %d"), (int32)NewState);
}

FVector UGrappleComponent::CalculateThrowVelocity(EGrappleDirection Direction) const
{
	if (!OwnerCharacter) return FVector::ZeroVector;

	FVector Forward = OwnerCharacter->GetActorForwardVector();
	FVector Right = OwnerCharacter->GetActorRightVector();
	FVector Up = FVector::UpVector;

	switch (Direction)
	{
	case EGrappleDirection::PullToPlayer:
		return -Forward * PullSpeed;

	case EGrappleDirection::ThrowUp:
		return Up * ThrowUpForce + Forward * (ThrowHorizontalForce * 0.3f);

	case EGrappleDirection::ThrowLeft:
		return -Right * ThrowHorizontalForce + Up * (ThrowUpForce * 0.2f);

	case EGrappleDirection::ThrowRight:
		return Right * ThrowHorizontalForce + Up * (ThrowUpForce * 0.2f);

	case EGrappleDirection::ThrowDown:
		return -Up * ThrowSlamForce + Forward * (ThrowHorizontalForce * 0.5f);

	case EGrappleDirection::ThrowForward:
		return Forward * ThrowHorizontalForce + Up * (ThrowUpForce * 0.3f);

	case EGrappleDirection::ThrowBackward:
		return -Forward * ThrowHorizontalForce + Up * (ThrowUpForce * 0.3f);

	default:
		return FVector::ZeroVector;
	}
}

void UGrappleComponent::ApplyThrowForce(AActor* Enemy, FVector Velocity)
{
	if (!Enemy) return;

	// If enemy has character movement, use that
	if (ACharacter* EnemyCharacter = Cast<ACharacter>(Enemy))
	{
		if (UCharacterMovementComponent* MovementComp = EnemyCharacter->GetCharacterMovement())
		{
			MovementComp->Launch(Velocity);
			UE_LOG(LogTemp, Log, TEXT("Launched enemy with velocity: %s"), *Velocity.ToString());
		}
	}
	else
	{
		// Apply physics impulse
		if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(Enemy->GetRootComponent()))
		{
			RootComp->AddImpulse(Velocity * RootComp->GetMass(), NAME_None, true);
		}
	}
}

void UGrappleComponent::CheckThrowImpact(AActor* Enemy)
{
	if (!Enemy || !GetWorld()) return;

	FVector EnemyLocation = Enemy->GetActorLocation();
	FVector EnemyVelocity = Enemy->GetVelocity();

	// Sphere trace to check for nearby objects
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Enemy);
	QueryParams.AddIgnoredActor(OwnerCharacter);

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		EnemyLocation,
		EnemyLocation + (EnemyVelocity.GetSafeNormal() * 100.0f),
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeSphere(50.0f),
		QueryParams
	);

	if (bHit && EnemyVelocity.Size() > 100.0f)
	{
		for (const FHitResult& Hit : HitResults)
		{
			// Enemy hit a wall or object!
			if (ACombatEntity* CombatEnemy = Cast<ACombatEntity>(Enemy))
			{
				// Apply impact damage
				CombatEnemy->ApplyDamageFrom(ImpactDamage, OwnerCharacter);

				// Apply stun
				if (UCombatMovementComponent* MovementComp = CombatEnemy->FindComponentByClass<UCombatMovementComponent>())
				{
					MovementComp->ApplyStun(StunDurationOnImpact);
				}

				OnEnemyImpact(Enemy, Hit.GetActor(), ImpactDamage);

				UE_LOG(LogTemp, Log, TEXT("Enemy %s impacted %s - dealt %.1f damage and %.1fs stun"),
					*Enemy->GetName(), *Hit.GetActor()->GetName(), ImpactDamage, StunDurationOnImpact);
			}

			// Return to idle after impact
			ChangeState(EGrappleState::Idle);
			break;
		}
	}
}
