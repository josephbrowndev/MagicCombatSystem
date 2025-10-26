// Weapon Return Component Implementation

#include "WeaponReturnComponent.h"
#include "SkillTreeComponent.h"
#include "InventoryComponent.h"
#include "NinjaWizardCharacter.h"
#include "CombatEntity.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

UWeaponReturnComponent::UWeaponReturnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ThrowSpeed = 2000.0f;
	ReturnSpeed = 2500.0f;
	MaxThrowDistance = 5000.0f;
	DamageMultiplier = 1.0f;
	CollisionRadius = 50.0f;
	bDamageOnReturn = true;
}

void UWeaponReturnComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = GetOwner();
	if (OwnerCharacter)
	{
		SkillTreeComponent = OwnerCharacter->FindComponentByClass<USkillTreeComponent>();
		InventoryComponent = OwnerCharacter->FindComponentByClass<UInventoryComponent>();
	}
}

void UWeaponReturnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateThrownWeapons(DeltaTime);
}

// ============================================
// Weapon Throwing
// ============================================

bool UWeaponReturnComponent::CanThrowWeapon(int32 WeaponSlotIndex) const
{
	if (!InventoryComponent || !SkillTreeComponent) return false;

	// Get the weapon in this slot
	FWeaponData* Weapon = InventoryComponent->GetEquippedWeapon(WeaponSlotIndex);
	if (!Weapon) return false;

	// Check if weapon has Return skill unlocked
	if (!HasReturnSkillUnlocked(Weapon->WeaponStyle))
	{
		return false;
	}

	// Check if this weapon is already thrown
	if (IsWeaponThrown(Weapon->ItemID))
	{
		return false;
	}

	return true;
}

bool UWeaponReturnComponent::ThrowWeapon(int32 WeaponSlotIndex, FVector Direction)
{
	if (!CanThrowWeapon(WeaponSlotIndex)) return false;
	if (!OwnerCharacter) return false;

	FWeaponData* Weapon = InventoryComponent->GetEquippedWeapon(WeaponSlotIndex);
	if (!Weapon) return false;

	// Create thrown weapon data
	FThrownWeaponData ThrownData;
	ThrownData.WeaponID = Weapon->ItemID;
	ThrownData.OriginalSlotIndex = WeaponSlotIndex;
	ThrownData.ThrowLocation = OwnerCharacter->GetActorLocation();
	ThrownData.ThrowDirection = Direction.GetSafeNormal();
	ThrownData.ThrowSpeed = ThrowSpeed;
	ThrownData.Damage = GetWeaponReturnDamage(Weapon->ItemID);
	ThrownData.State = EWeaponReturnState::Throwing;

	// Spawn visual representation
	SpawnThrownWeaponActor(ThrownData);

	// Add to thrown weapons array
	ThrownWeapons.Add(ThrownData);

	// Temporarily unequip the weapon
	// (Don't remove from inventory, just mark as thrown)

	OnWeaponThrown(Weapon->ItemID, WeaponSlotIndex);

	UE_LOG(LogTemp, Log, TEXT("Weapon thrown: %s"), *Weapon->ItemName.ToString());

	return true;
}

bool UWeaponReturnComponent::ThrowCurrentWeapon(FVector Direction)
{
	if (!InventoryComponent) return false;

	int32 CurrentSlot = InventoryComponent->GetCurrentWeaponSlot();
	return ThrowWeapon(CurrentSlot, Direction);
}

// ============================================
// Weapon Recalling
// ============================================

bool UWeaponReturnComponent::CanRecallWeapon(FName WeaponID) const
{
	return IsWeaponThrown(WeaponID);
}

bool UWeaponReturnComponent::RecallWeapon(FName WeaponID)
{
	FThrownWeaponData* WeaponData = GetThrownWeaponData(WeaponID);
	if (!WeaponData) return false;

	// Only recall if currently throwing (not already returning)
	if (WeaponData->State != EWeaponReturnState::Throwing)
	{
		return false;
	}

	// Change state to returning
	WeaponData->State = EWeaponReturnState::Returning;

	OnWeaponRecalled(WeaponID, WeaponData->OriginalSlotIndex);

	UE_LOG(LogTemp, Log, TEXT("Weapon recalled: %s"), *WeaponID.ToString());

	return true;
}

void UWeaponReturnComponent::RecallAllWeapons()
{
	for (FThrownWeaponData& WeaponData : ThrownWeapons)
	{
		if (WeaponData.State == EWeaponReturnState::Throwing)
		{
			RecallWeapon(WeaponData.WeaponID);
		}
	}
}

// ============================================
// Query Functions
// ============================================

bool UWeaponReturnComponent::IsWeaponThrown(FName WeaponID) const
{
	for (const FThrownWeaponData& WeaponData : ThrownWeapons)
	{
		if (WeaponData.WeaponID == WeaponID)
		{
			return true;
		}
	}
	return false;
}

TArray<FName> UWeaponReturnComponent::GetThrownWeaponIDs() const
{
	TArray<FName> WeaponIDs;
	for (const FThrownWeaponData& WeaponData : ThrownWeapons)
	{
		WeaponIDs.Add(WeaponData.WeaponID);
	}
	return WeaponIDs;
}

FThrownWeaponData* UWeaponReturnComponent::GetThrownWeaponData(FName WeaponID)
{
	for (FThrownWeaponData& WeaponData : ThrownWeapons)
	{
		if (WeaponData.WeaponID == WeaponID)
		{
			return &WeaponData;
		}
	}
	return nullptr;
}

// ============================================
// Internal Logic
// ============================================

void UWeaponReturnComponent::UpdateThrownWeapons(float DeltaTime)
{
	// Update all thrown weapons
	for (int32 i = ThrownWeapons.Num() - 1; i >= 0; i--)
	{
		FThrownWeaponData& WeaponData = ThrownWeapons[i];

		if (WeaponData.State == EWeaponReturnState::Throwing)
		{
			UpdateThrowingWeapon(WeaponData, DeltaTime);
		}
		else if (WeaponData.State == EWeaponReturnState::Returning)
		{
			UpdateReturningWeapon(WeaponData, DeltaTime);
		}
		else if (WeaponData.State == EWeaponReturnState::Recalled)
		{
			// Weapon has returned, remove from array
			CompleteWeaponReturn(WeaponData);
			ThrownWeapons.RemoveAt(i);
		}
	}
}

void UWeaponReturnComponent::UpdateThrowingWeapon(FThrownWeaponData& WeaponData, float DeltaTime)
{
	if (!WeaponData.ThrownWeaponActor) return;

	FVector CurrentLocation = WeaponData.ThrownWeaponActor->GetActorLocation();
	FVector Movement = WeaponData.ThrowDirection * WeaponData.ThrowSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + Movement;

	// Check for collisions along the path
	CheckWeaponCollisions(WeaponData, CurrentLocation, NewLocation);

	// Move the weapon
	WeaponData.ThrownWeaponActor->SetActorLocation(NewLocation);

	// Check if weapon has traveled too far
	float DistanceTraveled = FVector::Dist(WeaponData.ThrowLocation, NewLocation);
	if (DistanceTraveled >= MaxThrowDistance)
	{
		// Auto-recall at max distance
		RecallWeapon(WeaponData.WeaponID);
	}

	// Debug visualization
	DrawDebugLine(GetWorld(), CurrentLocation, NewLocation, FColor::Yellow, false, 0.1f, 0, 2.0f);
}

void UWeaponReturnComponent::UpdateReturningWeapon(FThrownWeaponData& WeaponData, float DeltaTime)
{
	if (!WeaponData.ThrownWeaponActor || !OwnerCharacter) return;

	FVector CurrentLocation = WeaponData.ThrownWeaponActor->GetActorLocation();
	FVector OwnerLocation = OwnerCharacter->GetActorLocation();
	FVector Direction = (OwnerLocation - CurrentLocation).GetSafeNormal();
	FVector Movement = Direction * ReturnSpeed * DeltaTime;
	FVector NewLocation = CurrentLocation + Movement;

	// Check for collisions along the return path (if enabled)
	if (bDamageOnReturn)
	{
		CheckWeaponCollisions(WeaponData, CurrentLocation, NewLocation);
	}

	// Move the weapon
	WeaponData.ThrownWeaponActor->SetActorLocation(NewLocation);

	// Check if weapon has returned to owner
	float DistanceToOwner = FVector::Dist(NewLocation, OwnerLocation);
	if (DistanceToOwner <= 100.0f) // Close enough to catch
	{
		WeaponData.State = EWeaponReturnState::Recalled;
	}

	// Debug visualization
	DrawDebugLine(GetWorld(), CurrentLocation, NewLocation, FColor::Green, false, 0.1f, 0, 2.0f);
}

void UWeaponReturnComponent::CheckWeaponCollisions(FThrownWeaponData& WeaponData, const FVector& StartPos, const FVector& EndPos)
{
	if (!GetWorld()) return;

	// Sphere trace for enemies
	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);
	if (WeaponData.ThrownWeaponActor)
	{
		QueryParams.AddIgnoredActor(WeaponData.ThrownWeaponActor);
	}

	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		StartPos,
		EndPos,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(CollisionRadius),
		QueryParams
	);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			if (ACombatEntity* Enemy = Cast<ACombatEntity>(Hit.GetActor()))
			{
				// Don't damage player summons
				if (!Enemy->bIsPlayerSummon)
				{
					DealDamageToEnemy(Enemy, WeaponData);
				}
			}
		}
	}
}

void UWeaponReturnComponent::DealDamageToEnemy(AActor* Enemy, const FThrownWeaponData& WeaponData)
{
	if (!Enemy) return;

	if (ACombatEntity* CombatEnemy = Cast<ACombatEntity>(Enemy))
	{
		CombatEnemy->ApplyDamageFrom(WeaponData.Damage, OwnerCharacter);
		OnWeaponHitEnemy(WeaponData.WeaponID, Enemy, WeaponData.Damage);

		UE_LOG(LogTemp, Log, TEXT("Thrown weapon hit enemy: %s for %.1f damage"),
			*CombatEnemy->EntityName.ToString(), WeaponData.Damage);
	}
}

void UWeaponReturnComponent::SpawnThrownWeaponActor(FThrownWeaponData& WeaponData)
{
	if (!GetWorld() || !ThrownWeaponActorClass) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = Cast<APawn>(OwnerCharacter);

	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		ThrownWeaponActorClass,
		WeaponData.ThrowLocation,
		WeaponData.ThrowDirection.Rotation(),
		SpawnParams
	);

	WeaponData.ThrownWeaponActor = SpawnedActor;
}

void UWeaponReturnComponent::DestroyThrownWeaponActor(FThrownWeaponData& WeaponData)
{
	if (WeaponData.ThrownWeaponActor)
	{
		WeaponData.ThrownWeaponActor->Destroy();
		WeaponData.ThrownWeaponActor = nullptr;
	}
}

void UWeaponReturnComponent::CompleteWeaponReturn(FThrownWeaponData& WeaponData)
{
	// Destroy the visual actor
	DestroyThrownWeaponActor(WeaponData);

	// Re-equip the weapon (if inventory still has it)
	// This is automatic since we never actually removed it from inventory

	OnWeaponReturned(WeaponData.WeaponID, WeaponData.OriginalSlotIndex);

	UE_LOG(LogTemp, Log, TEXT("Weapon returned: %s"), *WeaponData.WeaponID.ToString());
}

// ============================================
// Helper Functions
// ============================================

float UWeaponReturnComponent::GetWeaponReturnDamage(FName WeaponID) const
{
	if (!InventoryComponent) return 100.0f;

	// Get the weapon's base damage
	FWeaponData* Weapon = InventoryComponent->FindWeaponByID(WeaponID);
	if (!Weapon) return 100.0f;

	// Apply damage multiplier from skill tree
	float BaseDamage = Weapon->BaseDamage;
	return BaseDamage * DamageMultiplier;
}

bool UWeaponReturnComponent::HasReturnSkillUnlocked(EWeaponStyle WeaponStyle) const
{
	if (!SkillTreeComponent) return false;

	return SkillTreeComponent->HasReturnWeaponSkill(WeaponStyle);
}

void UWeaponReturnComponent::OnWeaponReturnPathBlocked_Implementation(FName WeaponID, FVector BlockLocation)
{
	// Default implementation - can be overridden in Blueprint
	UE_LOG(LogTemp, Warning, TEXT("Weapon return path blocked: %s"), *WeaponID.ToString());
}
