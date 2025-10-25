// Main player character implementation

#include "NinjaWizardCharacter.h"
#include "MagicComponent.h"
#include "WeaponComponent.h"
#include "MasteryManagerComponent.h"
#include "PlayerAttributeComponent.h"
#include "SummonManagerComponent.h"
#include "InteractableInterface.h"
#include "NinjaWizardHUD.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/KismetSystemLibrary.h"

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

	// Camera settings
	MouseSensitivity = 1.0f;
	bInvertCameraY = false;

	// Interaction settings
	InteractionRange = 300.0f;
	InteractionCheckFrequency = 0.1f;
	InteractionCheckTimer = 0.0f;
	FocusedInteractable = nullptr;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->JumpZVelocity = 700.0f;
	GetCharacterMovement()->AirControl = 0.35f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create components
	MagicComponent = CreateDefaultSubobject<UMagicComponent>(TEXT("MagicComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	MasteryManager = CreateDefaultSubobject<UMasteryManagerComponent>(TEXT("MasteryManager"));
	AttributeComponent = CreateDefaultSubobject<UPlayerAttributeComponent>(TEXT("AttributeComponent"));
	SummonManager = CreateDefaultSubobject<USummonManagerComponent>(TEXT("SummonManager"));
}

void ANinjaWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize resources
	CurrentMana = MaxMana;
	CurrentStamina = MaxStamina;

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
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

	// Perform interaction checks
	InteractionCheckTimer += DeltaTime;
	if (InteractionCheckTimer >= InteractionCheckFrequency)
	{
		PerformInteractionCheck();
		InteractionCheckTimer = 0.0f;
	}
}

void ANinjaWizardCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Moving
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANinjaWizardCharacter::Move);
		}

		// Looking
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANinjaWizardCharacter::Look);
		}

		// Jumping
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANinjaWizardCharacter::StartJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ANinjaWizardCharacter::StopJump);
		}

		// Interacting
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ANinjaWizardCharacter::Interact);
		}

		// Casting spells
		if (CastSpellAction)
		{
			EnhancedInputComponent->BindAction(CastSpellAction, ETriggerEvent::Started, this, &ANinjaWizardCharacter::StartCastSpell);
		}

		// Attacking
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ANinjaWizardCharacter::StartAttack);
		}
	}
}

// ============================================
// Input Callbacks
// ============================================

void ANinjaWizardCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANinjaWizardCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * MouseSensitivity);
		AddControllerPitchInput(LookAxisVector.Y * MouseSensitivity * (bInvertCameraY ? 1.0f : -1.0f));
	}
}

void ANinjaWizardCharacter::StartJump()
{
	Jump();
}

void ANinjaWizardCharacter::StopJump()
{
	StopJumping();
}

void ANinjaWizardCharacter::Interact()
{
	BeginInteract();
}

void ANinjaWizardCharacter::StartCastSpell()
{
	// This is a placeholder - you'll implement specific spell selection in Blueprint or UI
	// For now, just log that the player wants to cast a spell
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Cast Spell Action Pressed"));
	}
}

void ANinjaWizardCharacter::StartAttack()
{
	// Perform attack with current weapon
	if (CurrentWeaponStyle != EWeaponStyle::None)
	{
		PerformWeaponAttack(CurrentWeaponStyle);
	}
}

// ============================================
// Interaction System
// ============================================

void ANinjaWizardCharacter::PerformInteractionCheck()
{
	if (!Controller)
	{
		return;
	}

	FVector StartLocation = FollowCamera->GetComponentLocation();
	FVector EndLocation = StartLocation + (FollowCamera->GetForwardVector() * InteractionRange);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Perform line trace
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		QueryParams
	);

	// Check if we hit an interactable
	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;
	AActor* NewFocusedInteractable = nullptr;

	if (HitActor && HitActor->Implements<UInteractableInterface>())
	{
		if (IInteractableInterface::Execute_CanInteract(HitActor, this))
		{
			NewFocusedInteractable = HitActor;
		}
	}

	// Update focused interactable
	if (NewFocusedInteractable != FocusedInteractable)
	{
		// Unfocus old
		if (FocusedInteractable && FocusedInteractable->Implements<UInteractableInterface>())
		{
			IInteractableInterface::Execute_OnInteractionUnfocus(FocusedInteractable);
		}

		FocusedInteractable = NewFocusedInteractable;

		// Focus new
		if (FocusedInteractable)
		{
			IInteractableInterface::Execute_OnInteractionFocus(FocusedInteractable);
		}
	}
}

void ANinjaWizardCharacter::BeginInteract()
{
	if (FocusedInteractable && FocusedInteractable->Implements<UInteractableInterface>())
	{
		if (IInteractableInterface::Execute_CanInteract(FocusedInteractable, this))
		{
			IInteractableInterface::Execute_Interact(FocusedInteractable, this);
		}
	}
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
