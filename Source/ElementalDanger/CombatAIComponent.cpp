// Combat AI Component Implementation

#include "CombatAIComponent.h"
#include "CombatEntity.h"
#include "NinjaWizardCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UCombatAIComponent::UCombatAIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	CombatRole = EAICombatRole::Warrior;
	bIsAttacking = false;
	bIsInCombo = false;
	CurrentComboStep = 0;
	CurrentBossPhase = 0;

	TimeSinceLastAttack = 0.0f;
	ComboTimer = 0.0f;
	AttackCooldownTimer = 0.0f;

	bIsEnraged = false;
	bIsBlocking = false;
	bIsDodging = false;

	CurrentCombo = nullptr;
	CurrentAttack = nullptr;
}

void UCombatAIComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerEntity = Cast<ACombatEntity>(GetOwner());
}

void UCombatAIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCombatAI(DeltaTime);

	// Update cooldown timers
	TimeSinceLastAttack += DeltaTime;
	AttackCooldownTimer = FMath::Max(AttackCooldownTimer - DeltaTime, 0.0f);

	// Update spell cooldowns
	TArray<FName> Keys;
	SpellCooldowns.GetKeys(Keys);
	for (FName Key : Keys)
	{
		float& Cooldown = SpellCooldowns[Key];
		Cooldown = FMath::Max(Cooldown - DeltaTime, 0.0f);
	}

	// Update combo timer
	if (bIsInCombo)
	{
		ComboTimer -= DeltaTime;
		if (ComboTimer <= 0.0f)
		{
			EndCombo();
		}
	}
}

// ============================================
// Combat State Management
// ============================================

void UCombatAIComponent::UpdateCombatAI(float DeltaTime)
{
	if (!CurrentTarget || !OwnerEntity) return;

	// Update based on combat role
	switch (CombatRole)
	{
		case EAICombatRole::Warrior:
			UpdateWarriorAI(DeltaTime);
			break;
		case EAICombatRole::Mage:
			UpdateMageAI(DeltaTime);
			break;
		case EAICombatRole::Archer:
			UpdateArcherAI(DeltaTime);
			break;
		case EAICombatRole::Boss:
			UpdateBossAI(DeltaTime);
			break;
	}

	// Always face target
	RotateTowardsTarget(CurrentTarget, DeltaTime);
}

void UCombatAIComponent::StartCombat(AActor* Enemy)
{
	if (!Enemy) return;

	CurrentTarget = Enemy;
	OnCombatStarted(Enemy);

	// Boss phase initialization
	if (bIsBoss && BossPhases.Num() > 0)
	{
		CurrentBossPhase = 0;
	}
}

void UCombatAIComponent::EndCombat()
{
	CurrentTarget = nullptr;
	bIsAttacking = false;
	bIsInCombo = false;
	CurrentComboStep = 0;
	OnCombatEnded();
}

void UCombatAIComponent::PerformAttack(AActor* Target)
{
	if (!CanAttack() || !Target) return;

	// Dispatch to appropriate attack method based on role
	switch (CombatRole)
	{
		case EAICombatRole::Warrior:
			StartMeleeCombo(Target);
			break;
		case EAICombatRole::Mage:
			CastSpell(Target);
			break;
		case EAICombatRole::Archer:
			FireArrow(Target);
			break;
		case EAICombatRole::Boss:
			SelectRandomBossPattern();
			ExecuteBossPattern(CurrentBossPattern, Target);
			break;
	}
}

bool UCombatAIComponent::CanAttack() const
{
	return !bIsAttacking && AttackCooldownTimer <= 0.0f && !bIsDodging;
}

float UCombatAIComponent::GetAttackRange() const
{
	if (CombatRole == EAICombatRole::Warrior)
	{
		return 150.0f; // Melee range
	}
	else if (CombatRole == EAICombatRole::Archer || CombatRole == EAICombatRole::Mage)
	{
		return 1000.0f; // Ranged
	}
	return 200.0f;
}

// ============================================
// Warrior AI (Melee Combos)
// ============================================

void UCombatAIComponent::UpdateWarriorAI(float DeltaTime)
{
	if (!CurrentTarget) return;

	float Distance = GetDistanceToTarget(CurrentTarget);

	// Check combat decisions
	EvaluateCombatSituation(CurrentTarget);

	// Attack when in range
	if (Distance <= GetAttackRange())
	{
		if (CanAttack())
		{
			StartMeleeCombo(CurrentTarget);
		}
	}
}

void UCombatAIComponent::StartMeleeCombo(AActor* Target)
{
	if (!Target || MeleeComboChains.Num() == 0) return;

	SelectRandomCombo();

	if (CurrentCombo && CurrentCombo->ComboAttacks.Num() > 0)
	{
		bIsInCombo = true;
		CurrentComboStep = 0;
		ComboTimer = CurrentCombo->ComboWindow;

		ExecuteComboAttack(CurrentCombo->ComboAttacks[CurrentComboStep], Target);
		OnComboStarted();
	}
}

void UCombatAIComponent::ContinueCombo()
{
	if (!bIsInCombo || !CurrentCombo || !CurrentTarget) return;

	CurrentComboStep++;

	if (CurrentComboStep < CurrentCombo->ComboAttacks.Num())
	{
		ExecuteComboAttack(CurrentCombo->ComboAttacks[CurrentComboStep], CurrentTarget);
		ComboTimer = CurrentCombo->ComboWindow; // Reset combo window
	}
	else
	{
		EndCombo();
	}
}

void UCombatAIComponent::EndCombo()
{
	bIsInCombo = false;
	CurrentComboStep = 0;
	CurrentCombo = nullptr;
	OnComboEnded();
}

void UCombatAIComponent::ExecuteComboAttack(const FAIAttackData& Attack, AActor* Target)
{
	if (!Target) return;

	bIsAttacking = true;

	// Windup time
	FTimerHandle WindupTimer;
	GetWorld()->GetTimerManager().SetTimer(WindupTimer, [this, Attack, Target]()
	{
		// Deal damage
		DealDamageToTarget(Target, Attack.Damage);
		OnAttackExecuted(Attack);

		// Recovery time
		FTimerHandle RecoveryTimer;
		GetWorld()->GetTimerManager().SetTimer(RecoveryTimer, [this]()
		{
			bIsAttacking = false;

			// Continue combo if still in combo state
			if (bIsInCombo)
			{
				float ComboDelay = FMath::RandRange(0.3f, 0.6f);
				FTimerHandle ComboDelayTimer;
				GetWorld()->GetTimerManager().SetTimer(ComboDelayTimer, [this]()
				{
					ContinueCombo();
				}, ComboDelay, false);
			}

		}, Attack.RecoveryTime, false);

	}, Attack.WindupTime, false);

	AttackCooldownTimer = Attack.Cooldown;
	TimeSinceLastAttack = 0.0f;
}

void UCombatAIComponent::SelectRandomCombo()
{
	if (MeleeComboChains.Num() == 0) return;

	int32 RandomIndex = FMath::RandRange(0, MeleeComboChains.Num() - 1);
	CurrentCombo = &MeleeComboChains[RandomIndex];
}

// ============================================
// Mage AI (Magic Casting)
// ============================================

void UCombatAIComponent::UpdateMageAI(float DeltaTime)
{
	if (!CurrentTarget) return;

	float Distance = GetDistanceToTarget(CurrentTarget);

	// Keep optimal distance (mid-range)
	if (Distance < 400.0f)
	{
		// Too close, back away
		Retreat(CurrentTarget);
	}
	else if (Distance <= 1000.0f && CanAttack())
	{
		CastSpell(CurrentTarget);
	}
}

void UCombatAIComponent::CastSpell(AActor* Target)
{
	if (!Target) return;

	SelectBestSpell(Target);

	if (CurrentAttack && !IsSpellOnCooldown(*CurrentAttack))
	{
		bIsAttacking = true;

		// Casting time (windup)
		FTimerHandle CastTimer;
		GetWorld()->GetTimerManager().SetTimer(CastTimer, [this, Target]()
		{
			if (CurrentAttack->bIsRanged)
			{
				SpawnProjectile(*CurrentAttack, Target);
			}
			else
			{
				CastAreaOfEffectSpell(*CurrentAttack);
			}

			OnSpellCast(*CurrentAttack);

			// Set spell on cooldown
			SpellCooldowns.Add(CurrentAttack->AttackName, CurrentAttack->Cooldown);

			bIsAttacking = false;

		}, CurrentAttack->WindupTime, false);

		AttackCooldownTimer = 1.0f; // Global cooldown between casts
	}
}

void UCombatAIComponent::SelectBestSpell(AActor* Target)
{
	if (MagicSpells.Num() == 0) return;

	// Find available spell (not on cooldown)
	for (FAIAttackData& Spell : MagicSpells)
	{
		if (!IsSpellOnCooldown(Spell))
		{
			CurrentAttack = &Spell;
			return;
		}
	}

	// All on cooldown, use basic attack
	if (MagicSpells.Num() > 0)
	{
		CurrentAttack = &MagicSpells[0];
	}
}

void UCombatAIComponent::SpawnProjectile(const FAIAttackData& Spell, AActor* Target)
{
	if (!Spell.ProjectileClass || !OwnerEntity || !Target) return;

	FVector SpawnLocation = OwnerEntity->GetActorLocation() + (OwnerEntity->GetActorForwardVector() * 100.0f);
	FVector TargetLocation = PredictTargetLocation(Target, 1000.0f); // Projectile speed
	FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation();

	AActor* Projectile = GetWorld()->SpawnActor<AActor>(Spell.ProjectileClass, SpawnLocation, SpawnRotation);

	// TODO: Set projectile damage and owner
}

void UCombatAIComponent::CastAreaOfEffectSpell(const FAIAttackData& Spell)
{
	if (!OwnerEntity) return;

	// AOE spell around caster or at target location
	FVector AOECenter = CurrentTarget ? CurrentTarget->GetActorLocation() : OwnerEntity->GetActorLocation();

	// Deal damage to all nearby enemies
	TArray<FHitResult> HitResults;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(Spell.Range);
	GetWorld()->SweepMultiByChannel(HitResults, AOECenter, AOECenter, FQuat::Identity,
		ECC_Pawn, Sphere);

	for (const FHitResult& Hit : HitResults)
	{
		if (AActor* HitActor = Hit.GetActor())
		{
			if (HitActor != OwnerEntity)
			{
				DealDamageToTarget(HitActor, Spell.Damage);
			}
		}
	}
}

bool UCombatAIComponent::IsSpellOnCooldown(const FAIAttackData& Spell) const
{
	if (SpellCooldowns.Contains(Spell.AttackName))
	{
		return SpellCooldowns[Spell.AttackName] > 0.0f;
	}
	return false;
}

// ============================================
// Archer AI (Ranged Attacks)
// ============================================

void UCombatAIComponent::UpdateArcherAI(float DeltaTime)
{
	if (!CurrentTarget) return;

	// Maintain optimal range
	if (!IsAtOptimalRange(CurrentTarget))
	{
		MaintainDistance(CurrentTarget);
	}

	// Attack when ready and in range
	if (CanAttack() && IsAtOptimalRange(CurrentTarget) && HasLineOfSight(CurrentTarget))
	{
		FireArrow(CurrentTarget);
	}
}

void UCombatAIComponent::FireArrow(AActor* Target)
{
	if (!Target || RangedAttacks.Num() == 0) return;

	FAIAttackData& Arrow = RangedAttacks[0]; // Basic arrow

	bIsAttacking = true;

	// Draw bow (windup)
	FTimerHandle DrawTimer;
	GetWorld()->GetTimerManager().SetTimer(DrawTimer, [this, Arrow, Target]()
	{
		SpawnProjectile(Arrow, Target);
		OnAttackExecuted(Arrow);

		bIsAttacking = false;

	}, Arrow.WindupTime, false);

	AttackCooldownTimer = Arrow.Cooldown;
}

void UCombatAIComponent::ChargeShot(AActor* Target)
{
	// Charged arrow with more damage
	// Similar to FireArrow but with longer windup and more damage
}

void UCombatAIComponent::FireMultiShot(AActor* Target)
{
	// Fire multiple arrows in a spread pattern
}

void UCombatAIComponent::MaintainDistance(AActor* Target)
{
	if (!Target || !OwnerEntity) return;

	float Distance = GetDistanceToTarget(Target);
	float OptimalRange = 600.0f;

	if (Distance < OptimalRange - 100.0f)
	{
		// Too close, back away
		Retreat(Target);
	}
	else if (Distance > OptimalRange + 200.0f)
	{
		// Too far, move closer (but not too close)
		FVector MoveLocation = Target->GetActorLocation();
		// Move towards but stop at optimal range
	}
}

bool UCombatAIComponent::IsAtOptimalRange(AActor* Target) const
{
	if (!Target) return false;

	float Distance = GetDistanceToTarget(Target);
	return Distance >= 400.0f && Distance <= 800.0f;
}

// ============================================
// Boss AI (Complex Patterns)
// ============================================

void UCombatAIComponent::UpdateBossAI(float DeltaTime)
{
	if (!OwnerEntity || !CurrentTarget) return;

	// Update boss phase based on health
	UpdateBossPhase();

	// Execute attack patterns
	if (CanAttack())
	{
		SelectRandomBossPattern();
		ExecuteBossPattern(CurrentBossPattern, CurrentTarget);
	}

	// Boss-specific behavior
	if (bIsEnraged)
	{
		// Faster attacks, more aggressive
	}
}

void UCombatAIComponent::UpdateBossPhase()
{
	if (!bIsBoss || !OwnerEntity || BossPhases.Num() == 0) return;

	float HealthPercentage = OwnerEntity->GetHealthPercentage();

	// Check if should transition to next phase
	for (int32 i = BossPhases.Num() - 1; i >= 0; i--)
	{
		if (HealthPercentage <= BossPhases[i].HealthThreshold)
		{
			if (CurrentBossPhase != BossPhases[i].PhaseNumber)
			{
				CurrentBossPhase = BossPhases[i].PhaseNumber;
				OnBossPhaseChanged(CurrentBossPhase);

				// Summon minions if phase requires it
				if (BossPhases[i].bSummonMinionsOnStart)
				{
					SummonMinions();
				}
			}
			break;
		}
	}
}

void UCombatAIComponent::ExecuteBossPattern(EBossAttackPattern Pattern, AActor* Target)
{
	if (!Target) return;

	switch (Pattern)
	{
		case EBossAttackPattern::BasicCombo:
			StartMeleeCombo(Target);
			break;
		case EBossAttackPattern::AreaOfEffect:
			PerformAreaOfEffectAttack(Target);
			break;
		case EBossAttackPattern::Charge:
			PerformChargeAttack(Target);
			break;
		case EBossAttackPattern::SummonMinions:
			SummonMinions();
			break;
		case EBossAttackPattern::EnragedMode:
			EnterEnragedMode();
			break;
		case EBossAttackPattern::TeleportStrike:
			PerformTeleportStrike(Target);
			break;
		case EBossAttackPattern::GroundSlam:
			PerformGroundSlam(Target);
			break;
		case EBossAttackPattern::RangedBarrage:
			PerformRangedBarrage(Target);
			break;
		case EBossAttackPattern::DefensiveStance:
			EnterDefensiveStance();
			break;
		case EBossAttackPattern::ElementalBurst:
			PerformElementalBurst(Target);
			break;
	}
}

void UCombatAIComponent::PerformAreaOfEffectAttack(AActor* Target)
{
	if (!OwnerEntity) return;

	bIsAttacking = true;

	// Windup animation
	FTimerHandle AOETimer;
	GetWorld()->GetTimerManager().SetTimer(AOETimer, [this]()
	{
		// Deal damage in radius around boss
		TArray<FHitResult> HitResults;
		FVector Location = OwnerEntity->GetActorLocation();
		FCollisionShape Sphere = FCollisionShape::MakeSphere(500.0f); // AOE radius

		GetWorld()->SweepMultiByChannel(HitResults, Location, Location, FQuat::Identity,
			ECC_Pawn, Sphere);

		for (const FHitResult& Hit : HitResults)
		{
			if (AActor* HitActor = Hit.GetActor())
			{
				if (HitActor != OwnerEntity)
				{
					DealDamageToTarget(HitActor, OwnerEntity->BaseDamage * 1.5f);
				}
			}
		}

		bIsAttacking = false;

	}, 1.5f, false); // 1.5 second windup

	AttackCooldownTimer = 5.0f; // Long cooldown for powerful attack
}

void UCombatAIComponent::PerformChargeAttack(AActor* Target)
{
	if (!Target || !OwnerEntity) return;

	// Dash towards target and deal heavy damage
	bIsAttacking = true;

	FVector ChargeDirection = (Target->GetActorLocation() - OwnerEntity->GetActorLocation()).GetSafeNormal();
	FVector ChargeEnd = OwnerEntity->GetActorLocation() + (ChargeDirection * 1000.0f);

	// TODO: Implement charge movement
	// Launch character towards target

	AttackCooldownTimer = 3.0f;
	bIsAttacking = false;
}

void UCombatAIComponent::SummonMinions()
{
	if (!OwnerEntity || BossPhases.Num() == 0) return;

	FBossPhaseData& CurrentPhase = BossPhases[CurrentBossPhase];
	if (!CurrentPhase.MinionClass) return;

	for (int32 i = 0; i < CurrentPhase.MinionCount; i++)
	{
		FVector SpawnLocation = OwnerEntity->GetActorLocation() +
			FVector(FMath::RandRange(-300.0f, 300.0f), FMath::RandRange(-300.0f, 300.0f), 0.0f);

		GetWorld()->SpawnActor<AActor>(CurrentPhase.MinionClass, SpawnLocation, FRotator::ZeroRotator);
	}

	AttackCooldownTimer = 10.0f; // Long cooldown
}

void UCombatAIComponent::EnterEnragedMode()
{
	bIsEnraged = true;
	OnEnragedModeActivated();

	// Increase attack speed and damage
	if (OwnerEntity)
	{
		OwnerEntity->AttackSpeed *= 1.5f;
		OwnerEntity->BaseDamage *= 1.3f;
	}
}

void UCombatAIComponent::PerformTeleportStrike(AActor* Target)
{
	if (!Target || !OwnerEntity) return;

	// Teleport behind target and strike
	FVector TargetLocation = Target->GetActorLocation();
	FVector TargetForward = Target->GetActorForwardVector();
	FVector TeleportLocation = TargetLocation - (TargetForward * 200.0f);

	OwnerEntity->SetActorLocation(TeleportLocation);

	// Immediate attack after teleport
	DealDamageToTarget(Target, OwnerEntity->BaseDamage * 2.0f);

	AttackCooldownTimer = 4.0f;
}

void UCombatAIComponent::PerformGroundSlam(AActor* Target)
{
	if (!OwnerEntity) return;

	// Jump up then slam down dealing AOE damage
	bIsAttacking = true;

	FTimerHandle SlamTimer;
	GetWorld()->GetTimerManager().SetTimer(SlamTimer, [this]()
	{
		PerformAreaOfEffectAttack(CurrentTarget);
		bIsAttacking = false;

	}, 1.0f, false);

	AttackCooldownTimer = 6.0f;
}

void UCombatAIComponent::PerformRangedBarrage(AActor* Target)
{
	if (!Target || !OwnerEntity) return;

	// Fire multiple projectiles in quick succession
	for (int32 i = 0; i < 5; i++)
	{
		FTimerHandle BarrageTimer;
		GetWorld()->GetTimerManager().SetTimer(BarrageTimer, [this, Target, i]()
		{
			if (RangedAttacks.Num() > 0)
			{
				SpawnProjectile(RangedAttacks[0], Target);
			}

		}, i * 0.3f, false); // 0.3 second intervals
	}

	AttackCooldownTimer = 8.0f;
}

void UCombatAIComponent::EnterDefensiveStance()
{
	bIsBlocking = true;

	// Reduce damage taken
	if (OwnerEntity)
	{
		OwnerEntity->Defense *= 2.0f;
	}

	// Exit defensive stance after duration
	FTimerHandle DefenseTimer;
	GetWorld()->GetTimerManager().SetTimer(DefenseTimer, [this]()
	{
		bIsBlocking = false;
		if (OwnerEntity)
		{
			OwnerEntity->Defense /= 2.0f;
		}

	}, 5.0f, false);

	AttackCooldownTimer = 10.0f;
}

void UCombatAIComponent::PerformElementalBurst(AActor* Target)
{
	// Powerful elemental attack based on entity's element type
	if (!OwnerEntity) return;

	PerformAreaOfEffectAttack(Target);
	AttackCooldownTimer = 12.0f;
}

void UCombatAIComponent::SelectRandomBossPattern()
{
	if (!bIsBoss || BossPhases.Num() == 0) return;

	FBossPhaseData& CurrentPhase = BossPhases[FMath::Min(CurrentBossPhase, BossPhases.Num() - 1)];
	if (CurrentPhase.AvailablePatterns.Num() == 0) return;

	int32 RandomIndex = FMath::RandRange(0, CurrentPhase.AvailablePatterns.Num() - 1);
	CurrentBossPattern = CurrentPhase.AvailablePatterns[RandomIndex];
}

// ============================================
// Combat Decision Making
// ============================================

void UCombatAIComponent::EvaluateCombatSituation(AActor* Target)
{
	if (!OwnerEntity || !Target) return;

	// Check health percentage
	float HealthPercentage = OwnerEntity->GetHealthPercentage();

	if (ShouldDodge())
	{
		FVector DodgeDirection = FMath::VRand();
		DodgeDirection.Z = 0;
		DodgeDirection.Normalize();
		PerformDodge(DodgeDirection);
	}
	else if (ShouldRetreat())
	{
		Retreat(Target);
	}
	else if (ShouldBlock())
	{
		PerformBlock();
	}
}

bool UCombatAIComponent::ShouldDodge() const
{
	// Random chance to dodge, higher if low health
	float DodgeChance = FMath::RandRange(0.0f, 1.0f);
	return DodgeChance < 0.15f; // 15% chance to dodge
}

bool UCombatAIComponent::ShouldBlock() const
{
	if (!OwnerEntity) return false;
	return OwnerEntity->GetHealthPercentage() < 0.3f; // Block when low health
}

bool UCombatAIComponent::ShouldRetreat() const
{
	if (!OwnerEntity) return false;
	return OwnerEntity->GetHealthPercentage() < 0.2f && CombatRole != EAICombatRole::Boss;
}

void UCombatAIComponent::PerformDodge(FVector DodgeDirection)
{
	if (!OwnerEntity) return;

	bIsDodging = true;

	// Quick dash in dodge direction
	FVector DodgeLocation = OwnerEntity->GetActorLocation() + (DodgeDirection * 300.0f);
	OwnerEntity->SetActorLocation(DodgeLocation);

	FTimerHandle DodgeTimer;
	GetWorld()->GetTimerManager().SetTimer(DodgeTimer, [this]()
	{
		bIsDodging = false;
	}, 0.5f, false);
}

void UCombatAIComponent::PerformBlock()
{
	bIsBlocking = true;

	FTimerHandle BlockTimer;
	GetWorld()->GetTimerManager().SetTimer(BlockTimer, [this]()
	{
		bIsBlocking = false;
	}, 2.0f, false);
}

void UCombatAIComponent::Retreat(AActor* Target)
{
	if (!Target || !OwnerEntity) return;

	FVector RetreatDirection = (OwnerEntity->GetActorLocation() - Target->GetActorLocation()).GetSafeNormal();
	FVector RetreatLocation = OwnerEntity->GetActorLocation() + (RetreatDirection * 400.0f);

	// TODO: Use AI Controller to move to retreat location
}

// ============================================
// Utility Functions
// ============================================

float UCombatAIComponent::GetDistanceToTarget(AActor* Target) const
{
	if (!Target || !OwnerEntity) return 9999999.0f;

	return FVector::Dist(OwnerEntity->GetActorLocation(), Target->GetActorLocation());
}

bool UCombatAIComponent::IsTargetInRange(AActor* Target, float Range) const
{
	return GetDistanceToTarget(Target) <= Range;
}

FVector UCombatAIComponent::PredictTargetLocation(AActor* Target, float ProjectileSpeed) const
{
	if (!Target || !OwnerEntity) return Target->GetActorLocation();

	// Simple prediction based on target velocity
	// TODO: Get actual velocity from target
	FVector TargetVelocity = FVector::ZeroVector;
	float Distance = GetDistanceToTarget(Target);
	float TimeToReach = Distance / ProjectileSpeed;

	return Target->GetActorLocation() + (TargetVelocity * TimeToReach);
}

void UCombatAIComponent::RotateTowardsTarget(AActor* Target, float DeltaTime)
{
	if (!Target || !OwnerEntity) return;

	FVector Direction = (Target->GetActorLocation() - OwnerEntity->GetActorLocation()).GetSafeNormal();
	FRotator TargetRotation = Direction.Rotation();
	FRotator NewRotation = FMath::RInterpTo(OwnerEntity->GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
	OwnerEntity->SetActorRotation(NewRotation);
}

void UCombatAIComponent::DealDamageToTarget(AActor* Target, float Damage)
{
	if (!Target) return;

	if (ACombatEntity* TargetEntity = Cast<ACombatEntity>(Target))
	{
		TargetEntity->TakeDamage(Damage, OwnerEntity);
	}
	else if (ANinjaWizardCharacter* Player = Cast<ANinjaWizardCharacter>(Target))
	{
		// Deal damage to player
		// Player->TakeDamage(Damage);
	}
}

bool UCombatAIComponent::HasLineOfSight(AActor* Target) const
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
