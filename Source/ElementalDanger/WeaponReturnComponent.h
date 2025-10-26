// Weapon Return Component - Handles throwing and recalling weapons

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionTypes.h"
#include "WeaponReturnComponent.generated.h"

class USkillTreeComponent;
class UInventoryComponent;

UENUM(BlueprintType)
enum class EWeaponReturnState : uint8
{
	NotThrown     UMETA(DisplayName = "Not Thrown"),
	Throwing      UMETA(DisplayName = "Throwing"),
	Returning     UMETA(DisplayName = "Returning"),
	Recalled      UMETA(DisplayName = "Recalled")
};

USTRUCT(BlueprintType)
struct FThrownWeaponData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	FName WeaponID;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	int32 OriginalSlotIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	FVector ThrowLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	FVector ThrowDirection = FVector::ForwardVector;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	float ThrowSpeed = 2000.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	float Damage = 100.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	AActor* ThrownWeaponActor = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Weapon Return")
	EWeaponReturnState State = EWeaponReturnState::NotThrown;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UWeaponReturnComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponReturnComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	float ThrowSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	float ReturnSpeed = 2500.0f; // Slightly faster return

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	float MaxThrowDistance = 5000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	float DamageMultiplier = 1.0f; // From skill tree

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	float CollisionRadius = 50.0f; // For hitting enemies

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	bool bDamageOnReturn = true; // Damage enemies on the way back

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Return|Config")
	TSubclassOf<AActor> ThrownWeaponActorClass; // Visual representation of thrown weapon

	// ============================================
	// Current State
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Weapon Return|State")
	TArray<FThrownWeaponData> ThrownWeapons;

	// ============================================
	// Weapon Throwing
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Weapon Return")
	bool CanThrowWeapon(int32 WeaponSlotIndex) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Return")
	bool ThrowWeapon(int32 WeaponSlotIndex, FVector Direction);

	UFUNCTION(BlueprintCallable, Category = "Weapon Return")
	bool ThrowCurrentWeapon(FVector Direction);

	// ============================================
	// Weapon Recalling
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Weapon Return")
	bool CanRecallWeapon(FName WeaponID) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Return")
	bool RecallWeapon(FName WeaponID);

	UFUNCTION(BlueprintCallable, Category = "Weapon Return")
	void RecallAllWeapons();

	// ============================================
	// Query Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Weapon Return|Query")
	bool IsWeaponThrown(FName WeaponID) const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Return|Query")
	int32 GetThrownWeaponCount() const { return ThrownWeapons.Num(); }

	UFUNCTION(BlueprintCallable, Category = "Weapon Return|Query")
	TArray<FName> GetThrownWeaponIDs() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon Return|Query")
	FThrownWeaponData* GetThrownWeaponData(FName WeaponID);

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon Return|Events")
	void OnWeaponThrown(FName WeaponID, int32 SlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon Return|Events")
	void OnWeaponRecalled(FName WeaponID, int32 SlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon Return|Events")
	void OnWeaponReturned(FName WeaponID, int32 SlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Weapon Return|Events")
	void OnWeaponHitEnemy(FName WeaponID, AActor* Enemy, float Damage);

	UFUNCTION(BlueprintNativeEvent, Category = "Weapon Return|Events")
	void OnWeaponReturnPathBlocked(FName WeaponID, FVector BlockLocation);

protected:
	// ============================================
	// Internal Logic
	// ============================================

	void UpdateThrownWeapons(float DeltaTime);
	void UpdateThrowingWeapon(FThrownWeaponData& WeaponData, float DeltaTime);
	void UpdateReturningWeapon(FThrownWeaponData& WeaponData, float DeltaTime);

	void CheckWeaponCollisions(FThrownWeaponData& WeaponData, const FVector& StartPos, const FVector& EndPos);
	void DealDamageToEnemy(AActor* Enemy, const FThrownWeaponData& WeaponData);

	void SpawnThrownWeaponActor(FThrownWeaponData& WeaponData);
	void DestroyThrownWeaponActor(FThrownWeaponData& WeaponData);

	void CompleteWeaponReturn(FThrownWeaponData& WeaponData);

	// ============================================
	// References
	// ============================================

	UPROPERTY()
	USkillTreeComponent* SkillTreeComponent;

	UPROPERTY()
	UInventoryComponent* InventoryComponent;

	UPROPERTY()
	AActor* OwnerCharacter;

	// ============================================
	// Helper Functions
	// ============================================

	float GetWeaponReturnDamage(FName WeaponID) const;
	bool HasReturnSkillUnlocked(EWeaponStyle WeaponStyle) const;
};
