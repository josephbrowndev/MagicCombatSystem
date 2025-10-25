// Combat AI Component - Handles enemy combat behaviors and attack patterns

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBehaviorTypes.h"
#include "CombatAIComponent.generated.h"

class ACombatEntity;
class ANinjaWizardCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UCombatAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatAIComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat AI")
	EAICombatRole CombatRole = EAICombatRole::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat AI|Warrior")
	TArray<FMeleeComboChain> MeleeComboChains;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat AI|Mage")
	TArray<FAIAttackData> MagicSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat AI|Archer")
	TArray<FAIAttackData> RangedAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat AI|Boss")
	TArray<FBossPhaseData> BossPhases;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat AI|Boss")
	bool bIsBoss = false;

	// ============================================
	// Combat State
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Combat AI")
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat AI")
	bool bIsInCombo = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat AI")
	int32 CurrentComboStep = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Combat AI|Boss")
	int32 CurrentBossPhase = 0;

	// ============================================
	// Combat Actions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void StartCombat(AActor* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void EndCombat();

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void PerformAttack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	bool CanAttack() const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	float GetAttackRange() const;

	// ============================================
	// Warrior Combat (Melee Combos)
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Warrior")
	void StartMeleeCombo(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Warrior")
	void ContinueCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Warrior")
	void EndCombo();

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Warrior")
	void ExecuteComboAttack(const FAIAttackData& Attack, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Warrior")
	void SelectRandomCombo();

	// ============================================
	// Mage Combat (Magic Casting)
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Mage")
	void CastSpell(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Mage")
	void SelectBestSpell(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Mage")
	void SpawnProjectile(const FAIAttackData& Spell, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Mage")
	void CastAreaOfEffectSpell(const FAIAttackData& Spell);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Mage")
	bool IsSpellOnCooldown(const FAIAttackData& Spell) const;

	// ============================================
	// Archer Combat (Ranged Attacks)
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Archer")
	void FireArrow(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Archer")
	void ChargeShot(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Archer")
	void FireMultiShot(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Archer")
	void MaintainDistance(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Archer")
	bool IsAtOptimalRange(AActor* Target) const;

	// ============================================
	// Boss Combat (Complex Patterns)
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void UpdateBossPhase();

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void ExecuteBossPattern(EBossAttackPattern Pattern, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void PerformAreaOfEffectAttack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void PerformChargeAttack(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void SummonMinions();

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void EnterEnragedMode();

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void PerformTeleportStrike(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void PerformGroundSlam(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void PerformRangedBarrage(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void EnterDefensiveStance();

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void PerformElementalBurst(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI|Boss")
	void SelectRandomBossPattern();

	// ============================================
	// Combat Decision Making
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void EvaluateCombatSituation(AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	bool ShouldDodge() const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	bool ShouldBlock() const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	bool ShouldRetreat() const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void PerformDodge(FVector DodgeDirection);

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void PerformBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void Retreat(AActor* Target);

	// ============================================
	// Utility Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	float GetDistanceToTarget(AActor* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	bool IsTargetInRange(AActor* Target, float Range) const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	FVector PredictTargetLocation(AActor* Target, float ProjectileSpeed) const;

	UFUNCTION(BlueprintCallable, Category = "Combat AI")
	void RotateTowardsTarget(AActor* Target, float DeltaTime);

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnCombatStarted(AActor* Enemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnCombatEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnAttackExecuted(const FAIAttackData& Attack);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnComboStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnComboEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnSpellCast(const FAIAttackData& Spell);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnBossPhaseChanged(int32 NewPhase);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat AI|Events")
	void OnEnragedModeActivated();

protected:
	// References
	UPROPERTY()
	ACombatEntity* OwnerEntity;

	UPROPERTY()
	AActor* CurrentTarget;

	// Combat state tracking
	FMeleeComboChain* CurrentCombo;
	FAIAttackData* CurrentAttack;
	EBossAttackPattern CurrentBossPattern;

	float TimeSinceLastAttack;
	float ComboTimer;
	float AttackCooldownTimer;

	bool bIsEnraged;
	bool bIsBlocking;
	bool bIsDodging;

	TMap<FName, float> SpellCooldowns;

	// Timers
	FTimerHandle AttackTimerHandle;
	FTimerHandle ComboTimerHandle;
	FTimerHandle PatternTimerHandle;

	// Helper functions
	void UpdateCombatAI(float DeltaTime);
	void UpdateWarriorAI(float DeltaTime);
	void UpdateMageAI(float DeltaTime);
	void UpdateArcherAI(float DeltaTime);
	void UpdateBossAI(float DeltaTime);

	void DealDamageToTarget(AActor* Target, float Damage);
	bool HasLineOfSight(AActor* Target) const;
};
