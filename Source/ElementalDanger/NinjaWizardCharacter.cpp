// Main player character implementation

#include "NinjaWizardCharacter.h"
#include "MagicComponent.h"
#include "WeaponComponent.h"
#include "MasteryManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANinjaWizardCharacter::ANinjaWizardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set default values
	MaxMana = 100.0f;
	CurrentMana = MaxMana;
	ManaRegenRate = 5.0f; // 5 mana per second

	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
	StaminaRegenRate = 10.0f; // 10 stamina per second

	CurrentWeaponStyle = EWeaponStyle::None;
	bIsCasting = false;
	CastingTimeRemaining = 0.0f;

	// Create components
	MagicComponent = CreateDefaultSubobject<UMagicComponent>(TEXT("MagicComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	MasteryManager = CreateDefaultSubobject<UMasteryManagerComponent>(TEXT("MasteryManager"));

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 700.0f;
}

void ANinjaWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize resources
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;
}

void ANinjaWizardCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Regenerate resources
	RegenerateMana(DeltaTime);
	RegenerateStamina(DeltaTime);

	// Handle casting timer
	if (bIsCasting)
	{
		CastingTimeRemaining -= DeltaTime;
		if (CastingTimeRemaining <= 0.0f)
		{
			bIsCasting = false;
			// Spell cast complete - handled by MagicComponent
		}
	}
}

void ANinjaWizardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Input bindings will be set up here
	// Example:
	// PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	// PlayerInputComponent->BindAxis("MoveForward", this, &ANinjaWizardCharacter::MoveForward);
}

// ============================================
// Resource Management
// ============================================

bool ANinjaWizardCharacter::ConsumeMana(float Amount)
{
	if (CurrentMana >= Amount)
	{
		CurrentMana -= Amount;
		return true;
	}
	return false;
}

void ANinjaWizardCharacter::RestoreMana(float Amount)
{
	CurrentMana = FMath::Min(CurrentMana + Amount, MaxMana);
}

bool ANinjaWizardCharacter::ConsumeStamina(float Amount)
{
	if (CurrentStamina >= Amount)
	{
		CurrentStamina -= Amount;
		return true;
	}
	return false;
}

void ANinjaWizardCharacter::RestoreStamina(float Amount)
{
	CurrentStamina = FMath::Min(CurrentStamina + Amount, MaxStamina);
}

float ANinjaWizardCharacter::GetManaPercentage() const
{
	return MaxMana > 0 ? (CurrentMana / MaxMana) : 0.0f;
}

float ANinjaWizardCharacter::GetStaminaPercentage() const
{
	return MaxStamina > 0 ? (CurrentStamina / MaxStamina) : 0.0f;
}

void ANinjaWizardCharacter::RegenerateMana(float DeltaTime)
{
	if (CurrentMana < MaxMana && !bIsCasting)
	{
		RestoreMana(ManaRegenRate * DeltaTime);
	}
}

void ANinjaWizardCharacter::RegenerateStamina(float DeltaTime)
{
	if (CurrentStamina < MaxStamina)
	{
		RestoreStamina(StaminaRegenRate * DeltaTime);
	}
}

// ============================================
// Combat Functions
// ============================================

void ANinjaWizardCharacter::CastSpell(const FSpellData& SpellData)
{
	if (bIsCasting)
	{
		return; // Already casting
	}

	if (MagicComponent)
	{
		// Magic component will handle the actual spell casting
		bool bSuccess = MagicComponent->CastSpell(SpellData);
		if (bSuccess)
		{
			bIsCasting = true;
			CastingTimeRemaining = SpellData.CastTime;
		}
	}
}

void ANinjaWizardCharacter::PerformWeaponAttack(EWeaponStyle WeaponStyle)
{
	if (WeaponComponent && !bIsCasting)
	{
		WeaponComponent->PerformAttack(WeaponStyle);
	}
}

void ANinjaWizardCharacter::EquipWeapon(EWeaponStyle WeaponStyle)
{
	CurrentWeaponStyle = WeaponStyle;
	if (WeaponComponent)
	{
		WeaponComponent->EquipWeapon(WeaponStyle);
	}
}
