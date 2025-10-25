// Safe Zone Volume Implementation

#include "SafeZoneVolume.h"
#include "Components/BoxComponent.h"
#include "NinjaWizardCharacter.h"
#include "CombatEntity.h"
#include "AIBehaviorComponent.h"
#include "DrawDebugHelpers.h"

ASafeZoneVolume::ASafeZoneVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create box component for safe zone
	SafeZoneBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeZoneBox"));
	RootComponent = SafeZoneBox;

	SafeZoneBox->SetBoxExtent(FVector(1000.0f, 1000.0f, 500.0f)); // Default size
	SafeZoneBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SafeZoneBox->SetCollisionResponseToAllChannels(ECR_Overlap);
	SafeZoneBox->SetGenerateOverlapEvents(true);

	bShowDebugBounds = true;
	DebugColor = FLinearColor::Green;
	bPreventCombat = true;
	bPreventEnemyEntry = true;
	bHealPlayers = false;
	HealRate = 5.0f;
}

void ASafeZoneVolume::BeginPlay()
{
	Super::BeginPlay();

	// Bind overlap events
	SafeZoneBox->OnComponentBeginOverlap.AddDynamic(this, &ASafeZoneVolume::OnBeginOverlap);
	SafeZoneBox->OnComponentEndOverlap.AddDynamic(this, &ASafeZoneVolume::OnEndOverlap);
}

void ASafeZoneVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Draw debug bounds if enabled
	if (bShowDebugBounds)
	{
		DrawDebugBounds();
	}

	// Heal players in zone
	if (bHealPlayers)
	{
		for (AActor* Player : PlayersInZone)
		{
			if (ANinjaWizardCharacter* PlayerChar = Cast<ANinjaWizardCharacter>(Player))
			{
				// TODO: Heal player
				// PlayerChar->Heal(HealRate * DeltaTime);
			}
		}
	}
}

// ============================================
// Safe Zone Functions
// ============================================

bool ASafeZoneVolume::IsActorInSafeZone(AActor* Actor) const
{
	if (!Actor || !SafeZoneBox) return false;

	return SafeZoneBox->IsOverlappingActor(Actor);
}

void ASafeZoneVolume::DrawDebugBounds()
{
	if (!SafeZoneBox) return;

	FVector Center = GetActorLocation();
	FVector Extent = SafeZoneBox->GetScaledBoxExtent();
	FQuat Rotation = GetActorQuat();

	DrawDebugBox(GetWorld(), Center, Extent, Rotation, DebugColor.ToFColor(true), false, -1.0f, 0, 2.0f);

	// Draw safe zone label
	DrawDebugString(GetWorld(), Center + FVector(0, 0, Extent.Z),
		*ZoneName, nullptr, DebugColor.ToFColor(true), 0.0f, true, 1.5f);
}

// ============================================
// Overlap Events
// ============================================

void ASafeZoneVolume::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	// Check if player
	if (ANinjaWizardCharacter* Player = Cast<ANinjaWizardCharacter>(OtherActor))
	{
		PlayersInZone.AddUnique(Player);
		OnPlayerEntered(Player);

		UE_LOG(LogTemp, Log, TEXT("Player entered safe zone: %s"), *ZoneName);
	}
	// Check if enemy
	else if (ACombatEntity* Enemy = Cast<ACombatEntity>(OtherActor))
	{
		if (bPreventEnemyEntry)
		{
			// Push enemy out of safe zone
			FVector PushDirection = (Enemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			FVector PushLocation = GetActorLocation() + (PushDirection * (SafeZoneBox->GetScaledBoxExtent().Size() + 100.0f));
			Enemy->SetActorLocation(PushLocation);

			OnEnemyAttemptedEntry(Enemy);

			UE_LOG(LogTemp, Warning, TEXT("Enemy attempted to enter safe zone: %s"), *Enemy->EntityName.ToString());
		}
	}
}

void ASafeZoneVolume::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor) return;

	if (ANinjaWizardCharacter* Player = Cast<ANinjaWizardCharacter>(OtherActor))
	{
		PlayersInZone.Remove(Player);
		OnPlayerExited(Player);

		UE_LOG(LogTemp, Log, TEXT("Player exited safe zone: %s"), *ZoneName);
	}
}
