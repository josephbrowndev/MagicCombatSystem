// Grapple Component - Handles grappling hook / elemental swing mechanics

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicTypes.h"
#include "GrappleComponent.generated.h"

UENUM(BlueprintType)
enum class EGrappleTargetType : uint8
{
	Environment    UMETA(DisplayName = "Environment"),
	Enemy          UMETA(DisplayName = "Enemy"),
	Object         UMETA(DisplayName = "Object"),
	Invalid        UMETA(DisplayName = "Invalid")
};

UENUM(BlueprintType)
enum class EGrappleDirection : uint8
{
	PullToPlayer      UMETA(DisplayName = "Pull To Player"),
	ThrowUp           UMETA(DisplayName = "Throw Up"),
	ThrowLeft         UMETA(DisplayName = "Throw Left"),
	ThrowRight        UMETA(DisplayName = "Throw Right"),
	ThrowDown         UMETA(DisplayName = "Throw Down (Slam)"),
	ThrowForward      UMETA(DisplayName = "Throw Forward"),
	ThrowBackward     UMETA(DisplayName = "Throw Backward")
};

UENUM(BlueprintType)
enum class EGrappleState : uint8
{
	Idle              UMETA(DisplayName = "Idle"),
	Shooting          UMETA(DisplayName = "Shooting Hook"),
	Attached          UMETA(DisplayName = "Attached"),
	PullingEnemy      UMETA(DisplayName = "Pulling Enemy"),
	ThrowingEnemy     UMETA(DisplayName = "Throwing Enemy"),
	PlayerSwinging    UMETA(DisplayName = "Player Swinging")
};

USTRUCT(BlueprintType)
struct FGrappleTarget
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
	AActor* TargetActor = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
	EGrappleTargetType TargetType = EGrappleTargetType::Invalid;

	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
	bool bIsValid = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGrappleComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Current State
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Grapple|State")
	EGrappleState CurrentState = EGrappleState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "Grapple|State")
	FGrappleTarget CurrentTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Grapple|State")
	EGrappleDirection SelectedThrowDirection = EGrappleDirection::PullToPlayer;

	// ============================================
	// Grapple Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	float MaxGrappleRange = 3000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	float GrappleSpeed = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	float PullSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	float ThrowSpeed = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	float GrappleCooldown = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	bool bCanGrappleEnemies = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Config")
	bool bCanGrappleEnvironment = true;

	// ============================================
	// Enemy Throw Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Throw")
	float ThrowUpForce = 800.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Throw")
	float ThrowHorizontalForce = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Throw")
	float ThrowSlamForce = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Throw")
	float StunDurationOnImpact = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Throw")
	float ImpactDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Throw")
	float AirComboWindow = 3.0f; // Time window for air combos after throw up

	// ============================================
	// Visual Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Visual")
	TSubclassOf<AActor> GrappleHookActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Visual")
	bool bShowGrappleLine = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Visual")
	FLinearColor GrappleLineColor = FLinearColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple|Visual")
	EMagicType GrappleElement = EMagicType::Lightning; // Visual element effect

	// ============================================
	// Grapple Shooting
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	bool ShootGrapple(FVector TargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	bool ShootGrappleAtActor(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	bool CanShootGrapple() const;

	UFUNCTION(BlueprintCallable, Category = "Grapple")
	void CancelGrapple();

	// ============================================
	// Target Selection
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Grapple|Target")
	FGrappleTarget FindGrappleTarget(FVector Direction, float MaxDistance);

	UFUNCTION(BlueprintCallable, Category = "Grapple|Target")
	bool IsValidGrappleTarget(AActor* Target) const;

	UFUNCTION(BlueprintCallable, Category = "Grapple|Target")
	EGrappleTargetType GetTargetType(AActor* Target) const;

	// ============================================
	// Enemy Manipulation
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Grapple|Enemy")
	void SetThrowDirection(EGrappleDirection Direction);

	UFUNCTION(BlueprintCallable, Category = "Grapple|Enemy")
	bool PullEnemyToPlayer(AActor* Enemy);

	UFUNCTION(BlueprintCallable, Category = "Grapple|Enemy")
	bool ThrowEnemy(AActor* Enemy, EGrappleDirection Direction);

	UFUNCTION(BlueprintCallable, Category = "Grapple|Enemy")
	void ExecuteThrow();

	// ============================================
	// Combo System Integration
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Grapple|Combo")
	bool IsInAirComboWindow() const;

	UFUNCTION(BlueprintCallable, Category = "Grapple|Combo")
	void StartAirCombo();

	UFUNCTION(BlueprintCallable, Category = "Grapple|Combo")
	AActor* GetGrappledEnemy() const { return CurrentTarget.TargetActor; }

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnGrappleShot(FVector TargetLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnGrappleAttached(AActor* Target, EGrappleTargetType TargetType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnGrappleFailed(FVector AttemptedLocation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnEnemyPulled(AActor* Enemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnEnemyThrown(AActor* Enemy, EGrappleDirection Direction);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnEnemyImpact(AActor* Enemy, AActor* ImpactObject, float Damage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnAirComboWindowStarted(AActor* Enemy);

	UFUNCTION(BlueprintImplementableEvent, Category = "Grapple|Events")
	void OnGrappleStateChanged(EGrappleState NewState);

protected:
	// ============================================
	// Internal Logic
	// ============================================

	void UpdateGrapple(float DeltaTime);
	void ProcessShooting(float DeltaTime);
	void ProcessPulling(float DeltaTime);
	void ProcessThrowing(float DeltaTime);

	void ChangeState(EGrappleState NewState);

	FVector CalculateThrowVelocity(EGrappleDirection Direction) const;
	void ApplyThrowForce(AActor* Enemy, FVector Velocity);
	void CheckThrowImpact(AActor* Enemy);

	// ============================================
	// References
	// ============================================

	UPROPERTY()
	AActor* OwnerCharacter;

	UPROPERTY()
	AActor* GrappleHookActor;

	// ============================================
	// Tracking
	// ============================================

	float CooldownTimer = 0.0f;
	float AirComboTimer = 0.0f;
	bool bInAirCombo = false;

	FVector GrappleStartLocation = FVector::ZeroVector;
	FVector GrappleEndLocation = FVector::ZeroVector;
	float GrappleProgress = 0.0f;
};
