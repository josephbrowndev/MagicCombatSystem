// AI Behavior Component Implementation

#include "AIBehaviorComponent.h"
#include "CombatEntity.h"
#include "NinjaWizardCharacter.h"
#include "SafeZoneVolume.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"
#include "DrawDebugHelpers.h"

UAIBehaviorComponent::UAIBehaviorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	BehaviorType = EAIBehaviorType::Passive;
	CurrentState = EAIState::Idle;

	CurrentStamina = 100.0f;
	TimeSinceLastAction = 0.0f;
	TimeSinceLastWander = 0.0f;
	TimeSinceLastEat = 0.0f;

	bIsInCombat = false;
	bIsFleeing = false;
	bIsExhausted = false;
	bCheesingSeen = false;
	bInSafeZone = false;
}

void UAIBehaviorComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerEntity = Cast<ACombatEntity>(GetOwner());
	if (OwnerEntity)
	{
		AIController = Cast<AAIController>(OwnerEntity->GetController());
		SpawnLocation = OwnerEntity->GetActorLocation();
	}

	// Set territory center to spawn location for area guards
	if (BehaviorType == EAIBehaviorType::AreaGuard)
	{
		if (AreaGuardSettings.TerritoryCenter == FVector::ZeroVector)
		{
			AreaGuardSettings.TerritoryCenter = SpawnLocation;
		}
	}

	// Initialize stamina for chasing mobs
	if (BehaviorType == EAIBehaviorType::Chasing)
	{
		CurrentStamina = ChasingSettings.MaxStamina;
	}
}

void UAIBehaviorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!OwnerEntity) return;

	// Update behavior based on type
	switch (BehaviorType)
	{
		case EAIBehaviorType::Passive:
			UpdatePassiveBehavior(DeltaTime);
			break;
		case EAIBehaviorType::Chasing:
			UpdateChasingBehavior(DeltaTime);
			break;
		case EAIBehaviorType::Neutral:
			UpdateNeutralBehavior(DeltaTime);
			break;
		case EAIBehaviorType::AreaGuard:
			UpdateAreaGuardBehavior(DeltaTime);
			break;
		case EAIBehaviorType::Aggressive:
			UpdateAggressiveBehavior(DeltaTime);
			break;
	}

	TimeSinceLastAction += DeltaTime;
}

// ============================================
// State Management
// ============================================

void UAIBehaviorComponent::SetBehaviorType(EAIBehaviorType NewType)
{
	BehaviorType = NewType;
	SetState(EAIState::Idle);
}

void UAIBehaviorComponent::SetState(EAIState NewState)
{
	EAIState OldState = CurrentState;
	CurrentState = NewState;
	OnStateChanged(OldState, NewState);
}

// ============================================
// Passive Behavior Implementation
// ============================================

void UAIBehaviorComponent::UpdatePassiveBehavior(float DeltaTime)
{
	if (!OwnerEntity) return;

	// Check for threats if can flee
	if (PassiveSettings.bCanFlee)
	{
		ANinjaWizardCharacter* Player = FindNearestPlayer();
		if (Player && GetDistanceToPlayer(Player) < PassiveSettings.FleeDistance)
		{
			FleeFromThreat(Player);
			return;
		}
	}

	// State machine for passive behavior
	switch (CurrentState)
	{
		case EAIState::Idle:
			TimeSinceLastWander += DeltaTime;
			if (TimeSinceLastWander >= PassiveSettings.WanderInterval)
			{
				StartWandering();
				TimeSinceLastWander = 0.0f;
			}
			break;

		case EAIState::Wandering:
			// Check if reached destination
			if (FVector::Dist(OwnerEntity->GetActorLocation(), WanderTarget) < 100.0f)
			{
				SetState(EAIState::Idle);
			}
			break;

		case EAIState::Eating:
			TimeSinceLastEat += DeltaTime;
			if (TimeSinceLastEat >= 5.0f) // Eat for 5 seconds
			{
				SetState(EAIState::Idle);
				TimeSinceLastEat = 0.0f;
			}
			break;

		case EAIState::Resting:
			// Auto-return to idle after resting
			break;

		case EAIState::Fleeing:
			// Continue fleeing
			break;
	}
}

void UAIBehaviorComponent::StartWandering()
{
	if (!OwnerEntity) return;

	WanderTarget = GetRandomLocationInRadius(SpawnLocation, PassiveSettings.WanderRadius);
	MoveToLocation(WanderTarget);
	SetState(EAIState::Wandering);
}

void UAIBehaviorComponent::LookForVegetation()
{
	CurrentVegetation = FindNearestVegetation();
	if (CurrentVegetation)
	{
		MoveToLocation(CurrentVegetation->GetActorLocation());
	}
}

void UAIBehaviorComponent::StartEating(AActor* Vegetation)
{
	if (!Vegetation) return;

	CurrentVegetation = Vegetation;
	StopMovement();
	SetState(EAIState::Eating);
	RotateTowards(Vegetation, 1.0f);
}

void UAIBehaviorComponent::StartResting()
{
	StopMovement();
	SetState(EAIState::Resting);
}

void UAIBehaviorComponent::FleeFromThreat(AActor* Threat)
{
	if (!Threat || !OwnerEntity) return;

	// Calculate flee direction (away from threat)
	FVector FleeDirection = (OwnerEntity->GetActorLocation() - Threat->GetActorLocation()).GetSafeNormal();
	FVector FleeLocation = OwnerEntity->GetActorLocation() + (FleeDirection * PassiveSettings.FleeDistance);

	MoveToLocation(FleeLocation);
	SetState(EAIState::Fleeing);
	bIsFleeing = true;
}

// ============================================
// Chasing Behavior Implementation (Catchable)
// ============================================

void UAIBehaviorComponent::UpdateChasingBehavior(float DeltaTime)
{
	if (!OwnerEntity) return;

	DetectPlayer();

	if (TargetPlayer)
	{
		float DistanceToPlayer = GetDistanceToPlayer(TargetPlayer);

		// Check if caught
		if (DistanceToPlayer <= ChasingSettings.CatchRadius)
		{
			OnCaught(TargetPlayer);
			return;
		}

		// Flee from player
		if (DistanceToPlayer <= ChasingSettings.DetectionRadius)
		{
			if (!bIsExhausted)
			{
				FleeFromPlayer(TargetPlayer);
				DrainStamina(DeltaTime);
			}
			else
			{
				// Too exhausted to run
				StopMovement();
				SetState(EAIState::Idle);
			}
		}
	}
	else
	{
		// Wander when no player nearby
		if (CurrentState == EAIState::Idle)
		{
			TimeSinceLastWander += DeltaTime;
			if (TimeSinceLastWander >= 3.0f)
			{
				StartWandering();
				TimeSinceLastWander = 0.0f;
			}
		}
	}

	// Restore stamina when not fleeing
	if (CurrentState != EAIState::Fleeing && CurrentStamina < ChasingSettings.MaxStamina)
	{
		CurrentStamina = FMath::Min(CurrentStamina + (DeltaTime * 5.0f), ChasingSettings.MaxStamina);
		if (CurrentStamina >= ChasingSettings.MaxStamina * 0.5f)
		{
			bIsExhausted = false;
		}
	}
}

void UAIBehaviorComponent::DetectPlayer()
{
	TargetPlayer = FindNearestPlayer();
}

void UAIBehaviorComponent::FleeFromPlayer(ANinjaWizardCharacter* Player)
{
	if (!Player || !OwnerEntity) return;

	FVector FleeDirection = (OwnerEntity->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
	FVector FleeLocation = OwnerEntity->GetActorLocation() + (FleeDirection * 500.0f);

	MoveToLocation(FleeLocation);
	SetState(EAIState::Fleeing);

	// Increase movement speed while fleeing
	if (UCharacterMovementComponent* Movement = OwnerEntity->GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = ChasingSettings.FleeSpeed;
	}
}

void UAIBehaviorComponent::OnCaught(ANinjaWizardCharacter* Player)
{
	if (!Player) return;

	// Spawn reward item
	if (ChasingSettings.RewardItemClass)
	{
		FVector SpawnLocation = OwnerEntity->GetActorLocation();
		GetWorld()->SpawnActor<AActor>(ChasingSettings.RewardItemClass, SpawnLocation, FRotator::ZeroRotator);
	}

	OnCaughtByPlayer(Player);

	// Destroy this creature (it's been caught)
	if (OwnerEntity)
	{
		OwnerEntity->Destroy();
	}
}

void UAIBehaviorComponent::DrainStamina(float DeltaTime)
{
	CurrentStamina = FMath::Max(CurrentStamina - (ChasingSettings.StaminaDrainRate * DeltaTime), 0.0f);

	if (CurrentStamina <= 0.0f)
	{
		bIsExhausted = true;
	}
}

bool UAIBehaviorComponent::IsExhausted() const
{
	return bIsExhausted;
}

// ============================================
// Neutral Behavior Implementation
// ============================================

void UAIBehaviorComponent::UpdateNeutralBehavior(float DeltaTime)
{
	if (!OwnerEntity) return;

	// Maintain group cohesion if in a group
	if (NeutralSettings.GroupType != EGroupBehavior::Solo && GroupMembers.Num() > 0)
	{
		MaintainGroupCohesion();
	}

	// Check for aggro only if attacked
	if (bIsInCombat)
	{
		if (TargetPlayer)
		{
			SetState(EAIState::Attacking);
		}
	}
	else
	{
		// Peaceful wandering
		if (CurrentState == EAIState::Idle)
		{
			TimeSinceLastWander += DeltaTime;
			if (TimeSinceLastWander >= 5.0f)
			{
				StartWandering();
				TimeSinceLastWander = 0.0f;
			}
		}
	}
}

void UAIBehaviorComponent::FormGroup(TArray<AActor*> NearbyAllies)
{
	GroupMembers = NearbyAllies;

	// First in array becomes leader
	if (GroupMembers.Num() > 0 && !GroupLeader)
	{
		GroupLeader = GroupMembers[0];
	}
}

void UAIBehaviorComponent::MaintainGroupCohesion()
{
	if (!GroupLeader || !OwnerEntity) return;

	float DistanceToLeader = FVector::Dist(OwnerEntity->GetActorLocation(), GroupLeader->GetActorLocation());

	// Stay within cohesion radius
	if (DistanceToLeader > NeutralSettings.GroupCohesionRadius)
	{
		MoveToLocation(GroupLeader->GetActorLocation());
	}
}

void UAIBehaviorComponent::OnAttackedByPlayer(ANinjaWizardCharacter* Player)
{
	if (!Player) return;

	TargetPlayer = Player;
	bIsInCombat = true;
	SetState(EAIState::Attacking);
	OnEnteredCombat(Player);

	// Call for help if enabled
	if (NeutralSettings.bCallForHelp)
	{
		CallForHelp();
	}
}

void UAIBehaviorComponent::CallForHelp()
{
	if (!OwnerEntity) return;

	TArray<FHitResult> HitResults;
	FVector Location = OwnerEntity->GetActorLocation();

	FCollisionShape Sphere = FCollisionShape::MakeSphere(NeutralSettings.HelpCallRadius);
	GetWorld()->SweepMultiByChannel(HitResults, Location, Location, FQuat::Identity,
		ECC_Pawn, Sphere);

	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* Ally = Hit.GetActor())
		{
			if (Ally != OwnerEntity && Ally->IsA<ACombatEntity>())
			{
				if (UAIBehaviorComponent* AllyBehavior = Ally->FindComponentByClass<UAIBehaviorComponent>())
				{
					AllyBehavior->RespondToHelpCall(OwnerEntity);
				}
			}
		}
	}
}

void UAIBehaviorComponent::RespondToHelpCall(AActor* Caller)
{
	if (!Caller || bIsInCombat) return;

	// Join the fight
	if (UAIBehaviorComponent* CallerBehavior = Caller->FindComponentByClass<UAIBehaviorComponent>())
	{
		TargetPlayer = CallerBehavior->TargetPlayer;
		if (TargetPlayer)
		{
			bIsInCombat = true;
			SetState(EAIState::Attacking);
			OnEnteredCombat(TargetPlayer);
		}
	}
}

// ============================================
// Area Guard Behavior Implementation
// ============================================

void UAIBehaviorComponent::UpdateAreaGuardBehavior(float DeltaTime)
{
	if (!OwnerEntity) return;

	ANinjaWizardCharacter* Player = FindNearestPlayer();

	// Check if player is in aggro range
	if (Player && IsPlayerInAggroRange(Player))
	{
		// Check for cheesing (attacking from outside territory)
		if (AreaGuardSettings.bEnableAntiCheese)
		{
			DetectCheesing(Player);
		}

		// Check if we're outside territory
		if (!IsInTerritory(OwnerEntity->GetActorLocation()))
		{
			ReturnToTerritory();
		}
		else
		{
			// Chase and attack
			TargetPlayer = Player;
			bIsInCombat = true;
			SetState(EAIState::Attacking);
		}
	}
	else
	{
		// Guard territory when no threats
		if (CurrentState != EAIState::Guarding)
		{
			GuardTerritory();
		}

		// Return to territory if wandered out
		if (!IsInTerritory(OwnerEntity->GetActorLocation()))
		{
			ReturnToTerritory();
		}
	}
}

void UAIBehaviorComponent::SetTerritory(FVector Center, float Radius)
{
	AreaGuardSettings.TerritoryCenter = Center;
	AreaGuardSettings.TerritoryRadius = Radius;
}

bool UAIBehaviorComponent::IsInTerritory(FVector Location) const
{
	return FVector::Dist(Location, AreaGuardSettings.TerritoryCenter) <= AreaGuardSettings.TerritoryRadius;
}

bool UAIBehaviorComponent::IsPlayerInAggroRange(ANinjaWizardCharacter* Player) const
{
	if (!Player || !OwnerEntity) return false;

	float Distance = GetDistanceToPlayer(Player);
	return Distance <= AreaGuardSettings.AggroRadius;
}

void UAIBehaviorComponent::GuardTerritory()
{
	SetState(EAIState::Guarding);

	// Patrol around territory center
	FVector PatrolPoint = GetRandomLocationInRadius(AreaGuardSettings.TerritoryCenter,
		AreaGuardSettings.TerritoryRadius * 0.5f);
	MoveToLocation(PatrolPoint);
}

void UAIBehaviorComponent::ReturnToTerritory()
{
	SetState(EAIState::Returning);
	MoveToLocation(AreaGuardSettings.TerritoryCenter);

	// Stop combat when returning
	bIsInCombat = false;
	TargetPlayer = nullptr;
}

void UAIBehaviorComponent::DetectCheesing(ANinjaWizardCharacter* Player)
{
	if (!Player || !OwnerEntity || bCheesingSeen) return;

	// Check if player is attacking but staying outside territory
	float DistanceToCenter = FVector::Dist(Player->GetActorLocation(), AreaGuardSettings.TerritoryCenter);
	float DistanceToGuard = GetDistanceToPlayer(Player);

	// Player is attacking from outside the territory boundary
	if (DistanceToCenter > AreaGuardSettings.TerritoryRadius &&
		DistanceToGuard < AreaGuardSettings.AntiCheeseDetectionRadius)
	{
		// Check if owner is taking damage (being attacked)
		if (OwnerEntity->GetHealthPercentage() < 0.95f)
		{
			bCheesingSeen = true;
			SpawnPunishmentMobs(Player);
			OnCheesingDetected(Player);
		}
	}
}

void UAIBehaviorComponent::SpawnPunishmentMobs(ANinjaWizardCharacter* Player)
{
	if (!Player || !AreaGuardSettings.PunishmentMobClass) return;

	for (int32 i = 0; i < AreaGuardSettings.PunishmentMobCount; i++)
	{
		// Spawn around the player
		FVector SpawnLocation = Player->GetActorLocation() +
			FVector(FMath::RandRange(-300.0f, 300.0f), FMath::RandRange(-300.0f, 300.0f), 0.0f);

		AActor* PunishmentMob = GetWorld()->SpawnActor<AActor>(
			AreaGuardSettings.PunishmentMobClass,
			SpawnLocation,
			FRotator::ZeroRotator);

		// Make punishment mobs stronger
		if (ACombatEntity* PunishmentEntity = Cast<ACombatEntity>(PunishmentMob))
		{
			PunishmentEntity->BaseDamage *= AreaGuardSettings.PunishmentMobStrengthMultiplier;
			PunishmentEntity->MaxHealth *= AreaGuardSettings.PunishmentMobStrengthMultiplier;
			PunishmentEntity->CurrentHealth = PunishmentEntity->MaxHealth;
		}
	}
}

void UAIBehaviorComponent::DropUniqueLoot()
{
	if (!OwnerEntity) return;

	FVector DropLocation = OwnerEntity->GetActorLocation();

	// Drop unique weapons
	for (TSubclassOf<AActor> WeaponClass : AreaGuardSettings.UniqueWeaponDrops)
	{
		if (WeaponClass)
		{
			GetWorld()->SpawnActor<AActor>(WeaponClass, DropLocation, FRotator::ZeroRotator);
		}
	}

	// Drop unique armor
	for (TSubclassOf<AActor> ArmorClass : AreaGuardSettings.UniqueArmorDrops)
	{
		if (ArmorClass)
		{
			GetWorld()->SpawnActor<AActor>(ArmorClass, DropLocation + FVector(50, 0, 0), FRotator::ZeroRotator);
		}
	}
}

// ============================================
// Aggressive Behavior Implementation (Zone Bosses)
// ============================================

void UAIBehaviorComponent::UpdateAggressiveBehavior(float DeltaTime)
{
	if (!OwnerEntity) return;

	DetectPlayerInZone();

	if (TargetPlayer)
	{
		// Check if player is in safe zone
		if (AggressiveSettings.bRespectSafeZones && IsPlayerInSafeZone(TargetPlayer))
		{
			StareAtPlayerInSafeZone(TargetPlayer);
			bInSafeZone = true;
		}
		else
		{
			// Chase the player
			if (bInSafeZone)
			{
				ResumeChaseWhenPlayerLeavesSafeZone(TargetPlayer);
				bInSafeZone = false;
			}

			ChasePlayerIndefinitely(TargetPlayer);
		}
	}
	else
	{
		// Return to patrol/idle
		if (CurrentState != EAIState::Patrolling)
		{
			SetState(EAIState::Patrolling);
			StartWandering();
		}
	}
}

void UAIBehaviorComponent::DetectPlayerInZone()
{
	ANinjaWizardCharacter* Player = FindNearestPlayer();

	if (Player && GetDistanceToPlayer(Player) <= AggressiveSettings.DetectionRadius)
	{
		if (!TargetPlayer)
		{
			TargetPlayer = Player;
			OnDetectedPlayer(Player);
		}
	}
	else
	{
		if (TargetPlayer && !AggressiveSettings.bChaseIndefinitely)
		{
			TargetPlayer = nullptr;
			OnLostPlayer(Player);
		}
	}
}

void UAIBehaviorComponent::ChasePlayerIndefinitely(ANinjaWizardCharacter* Player)
{
	if (!Player || !OwnerEntity) return;

	SetState(EAIState::Chasing);
	MoveToLocation(Player->GetActorLocation());

	// Increase movement speed for boss chases
	if (UCharacterMovementComponent* Movement = OwnerEntity->GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = AggressiveSettings.ChaseSpeed;
	}

	// Attack when in range
	float Distance = GetDistanceToPlayer(Player);
	if (Distance <= 200.0f) // Attack range
	{
		SetState(EAIState::Attacking);
	}
}

bool UAIBehaviorComponent::IsPlayerInSafeZone(ANinjaWizardCharacter* Player) const
{
	if (!Player || !GetWorld()) return false;

	// Find all SafeZoneVolumes in the world
	TArray<AActor*> SafeZones;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASafeZoneVolume::StaticClass(), SafeZones);

	// Check if player is in any safe zone
	for (AActor* Zone : SafeZones)
	{
		if (ASafeZoneVolume* SafeZone = Cast<ASafeZoneVolume>(Zone))
		{
			if (SafeZone->IsActorInSafeZone(Player))
			{
				return true;
			}
		}
	}

	return false;
}

void UAIBehaviorComponent::StareAtPlayerInSafeZone(ANinjaWizardCharacter* Player)
{
	if (!Player || !OwnerEntity) return;

	StopMovement();
	SetState(EAIState::Idle);

	// Rotate to face player menacingly
	RotateTowards(Player, 1.0f);

	// Move to edge of safe zone
	FVector DirectionToPlayer = (Player->GetActorLocation() - OwnerEntity->GetActorLocation()).GetSafeNormal();
	FVector EdgeLocation = Player->GetActorLocation() - (DirectionToPlayer * AggressiveSettings.SafeZoneStareDistance);
	MoveToLocation(EdgeLocation);
}

void UAIBehaviorComponent::ResumeChaseWhenPlayerLeavesSafeZone(ANinjaWizardCharacter* Player)
{
	if (!Player) return;

	OnDetectedPlayer(Player);
	SetState(EAIState::Chasing);
}

// ============================================
// Group Management
// ============================================

void UAIBehaviorComponent::RegisterWithGroup(AActor* Leader)
{
	GroupLeader = Leader;
}

void UAIBehaviorComponent::LeaveGroup()
{
	GroupLeader = nullptr;
	GroupMembers.Empty();
}

// ============================================
// Utility Functions
// ============================================

ANinjaWizardCharacter* UAIBehaviorComponent::FindNearestPlayer() const
{
	if (!GetWorld()) return nullptr;

	return Cast<ANinjaWizardCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

float UAIBehaviorComponent::GetDistanceToPlayer(ANinjaWizardCharacter* Player) const
{
	if (!Player || !OwnerEntity) return 9999999.0f;

	return FVector::Dist(OwnerEntity->GetActorLocation(), Player->GetActorLocation());
}

FVector UAIBehaviorComponent::GetRandomLocationInRadius(FVector Origin, float Radius) const
{
	FVector RandomDirection = FMath::VRand();
	RandomDirection.Z = 0; // Keep on ground level
	RandomDirection.Normalize();

	float RandomDistance = FMath::RandRange(0.0f, Radius);
	return Origin + (RandomDirection * RandomDistance);
}

void UAIBehaviorComponent::MoveToLocation(FVector Location)
{
	if (!AIController) return;

	AIController->MoveToLocation(Location, 50.0f);
}

void UAIBehaviorComponent::StopMovement()
{
	if (!AIController) return;

	AIController->StopMovement();
}

bool UAIBehaviorComponent::CanSeeActor(AActor* Target) const
{
	if (!Target || !OwnerEntity) return false;

	FHitResult HitResult;
	FVector Start = OwnerEntity->GetActorLocation();
	FVector End = Target->GetActorLocation();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerEntity);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams);

	return !bHit || HitResult.GetActor() == Target;
}

void UAIBehaviorComponent::RotateTowards(AActor* Target, float DeltaTime)
{
	if (!Target || !OwnerEntity) return;

	FVector Direction = (Target->GetActorLocation() - OwnerEntity->GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = Direction.Rotation();
	FRotator NewRotation = FMath::RInterpTo(OwnerEntity->GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
	OwnerEntity->SetActorRotation(NewRotation);
}

AActor* UAIBehaviorComponent::FindNearestVegetation() const
{
	// This would search for actors with vegetation tags
	// Implementation depends on how you tag vegetation in your world
	return nullptr;
}
