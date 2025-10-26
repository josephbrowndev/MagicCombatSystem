# Ninja Wizard Combat Game - Complete Implementation Guide

## Table of Contents
1. [Overview](#overview)
2. [Component Setup](#component-setup)
3. [Player Character Integration](#player-character-integration)
4. [Input Configuration](#input-configuration)
5. [System Integration](#system-integration)
6. [Blueprint Setup](#blueprint-setup)
7. [Testing Checklist](#testing-checklist)
8. [Common Issues & Solutions](#common-issues--solutions)

---

## Overview

This guide covers the implementation of all combat, progression, and gameplay systems for your ninja wizard game. You now have:

### **Core Systems** (9 Components)
1. **MagicComponent** - Element magic casting and mastery
2. **WeaponComponent** - 4 weapon styles with combos
3. **AttributeComponent** - STR/WIS/PER/AGI with 10x summon scaling
4. **SoulBondComponent** - Universal enemy bonding (all enemies)
5. **AIBehaviorComponent** - 5 mob types with anti-cheese
6. **CombatAIComponent** - Enemy combat AI with boss phases
7. **ProgressionTypes** - Type definitions for items and skills
8. **SkillTreeComponent** - 13 skill trees with discovery system
9. **InventoryComponent** - Unlimited inventory with equipment

### **Combat Systems** (4 Components)
10. **DamageCalculationComponent** - Elemental weaknesses and scaling
11. **CombatMovementComponent** - Block, parry, dodge, slide, stun
12. **GrappleComponent** - Grappling hook with directional throws
13. **HealingComponent** - Out-of-combat regen + restrictions
14. **WeaponReturnComponent** - Throw/recall weapon mechanic

### **Environment**
15. **SafeZoneVolume** - Protected areas with healing

---

## Component Setup

### Step 1: Add Components to NinjaWizardCharacter Blueprint

Open your `BP_NinjaWizardCharacter` Blueprint and add these components:

#### **Essential Components** (Add these to your character)
```
NinjaWizardCharacter
├── MagicComponent
├── WeaponComponent
├── AttributeComponent
├── SoulBondComponent
├── SkillTreeComponent
├── InventoryComponent
├── DamageCalculationComponent
├── CombatMovementComponent
├── GrappleComponent
├── HealingComponent
└── WeaponReturnComponent
```

**How to Add**:
1. Open `BP_NinjaWizardCharacter` in Blueprint Editor
2. Click "Add Component"
3. Search for each component name
4. Add all 11 components listed above

#### **Enemy Components** (Add these to enemy Blueprints)
```
EnemyCharacter (CombatEntity)
├── AIBehaviorComponent
├── CombatAIComponent
├── DamageCalculationComponent
└── CombatMovementComponent
```

---

## Player Character Integration

### Step 2: Update NinjaWizardCharacter.h

Add references to your new components:

```cpp
// NinjaWizardCharacter.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NinjaWizardCharacter.generated.h"

class UMagicComponent;
class UWeaponComponent;
class UAttributeComponent;
class USoulBondComponent;
class USkillTreeComponent;
class UInventoryComponent;
class UDamageCalculationComponent;
class UCombatMovementComponent;
class UGrappleComponent;
class UHealingComponent;
class UWeaponReturnComponent;

UCLASS()
class ELEMENTALDANGER_API ANinjaWizardCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ANinjaWizardCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ============================================
	// Component References
	// ============================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMagicComponent* MagicComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponComponent* WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAttributeComponent* AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USoulBondComponent* SoulBondComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkillTreeComponent* SkillTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDamageCalculationComponent* DamageCalculationComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCombatMovementComponent* CombatMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGrappleComponent* GrappleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealingComponent* HealingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWeaponReturnComponent* WeaponReturnComponent;

	// ============================================
	// Health System
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 100.0f;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void TakeDamage(float Damage, AActor* DamageDealer);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

	// ============================================
	// Input Actions
	// ============================================

	// Combat Movement
	void StartBlock();
	void StopBlock();
	void PerformParry();
	void PerformDodge(FVector Direction);
	void PerformSlide(FVector Direction);

	// Grappling
	void ShootGrapple();
	void SelectThrowDirection(EGrappleDirection Direction);
	void ExecuteThrow();

	// Magic
	void CastSpell(EMagicType Element);
	void CastDualSpell(EMagicType Element1, EMagicType Element2);

	// Weapon
	void PerformWeaponAttack();
	void SwitchWeaponSlot(int32 SlotIndex);
	void ThrowWeapon();
	void RecallWeapon();

	// Inventory
	void OpenInventory();
	void UsePotion();

	// Soul Bond
	void AttemptSoulBond(AActor* Target);
	void SummonBondedCreature(int32 Index);
};
```

### Step 3: Update NinjaWizardCharacter.cpp

Initialize all components:

```cpp
// NinjaWizardCharacter.cpp

#include "NinjaWizardCharacter.h"
#include "MagicComponent.h"
#include "WeaponComponent.h"
#include "AttributeComponent.h"
#include "SoulBondComponent.h"
#include "SkillTreeComponent.h"
#include "InventoryComponent.h"
#include "DamageCalculationComponent.h"
#include "CombatMovementComponent.h"
#include "GrappleComponent.h"
#include "HealingComponent.h"
#include "WeaponReturnComponent.h"

ANinjaWizardCharacter::ANinjaWizardCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create all components
	MagicComponent = CreateDefaultSubobject<UMagicComponent>(TEXT("MagicComponent"));
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	AttributeComponent = CreateDefaultSubobject<UAttributeComponent>(TEXT("AttributeComponent"));
	SoulBondComponent = CreateDefaultSubobject<USoulBondComponent>(TEXT("SoulBondComponent"));
	SkillTreeComponent = CreateDefaultSubobject<USkillTreeComponent>(TEXT("SkillTreeComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	DamageCalculationComponent = CreateDefaultSubobject<UDamageCalculationComponent>(TEXT("DamageCalculationComponent"));
	CombatMovementComponent = CreateDefaultSubobject<UCombatMovementComponent>(TEXT("CombatMovementComponent"));
	GrappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("GrappleComponent"));
	HealingComponent = CreateDefaultSubobject<UHealingComponent>(TEXT("HealingComponent"));
	WeaponReturnComponent = CreateDefaultSubobject<UWeaponReturnComponent>(TEXT("WeaponReturnComponent"));

	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
}

void ANinjaWizardCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize systems
	CurrentHealth = MaxHealth;
}

void ANinjaWizardCharacter::TakeDamage(float Damage, AActor* DamageDealer)
{
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - Damage);

	// Enter combat state
	if (HealingComponent)
	{
		HealingComponent->EnterCombat();
	}

	if (CurrentHealth <= 0.0f)
	{
		// Handle death
		UE_LOG(LogTemp, Warning, TEXT("Player died!"));
	}
}

void ANinjaWizardCharacter::Heal(float Amount)
{
	CurrentHealth = FMath::Min(MaxHealth, CurrentHealth + Amount);
}

// Combat Movement Actions
void ANinjaWizardCharacter::StartBlock()
{
	if (CombatMovementComponent)
	{
		CombatMovementComponent->StartBlocking();
	}
}

void ANinjaWizardCharacter::StopBlock()
{
	if (CombatMovementComponent)
	{
		CombatMovementComponent->StopBlocking();
	}
}

void ANinjaWizardCharacter::PerformParry()
{
	if (CombatMovementComponent)
	{
		CombatMovementComponent->StartParry();
	}
}

void ANinjaWizardCharacter::PerformDodge(FVector Direction)
{
	if (CombatMovementComponent)
	{
		CombatMovementComponent->StartDodge(Direction);
	}
}

void ANinjaWizardCharacter::PerformSlide(FVector Direction)
{
	if (CombatMovementComponent)
	{
		CombatMovementComponent->StartSlide(Direction);
	}
}

// Grappling Actions
void ANinjaWizardCharacter::ShootGrapple()
{
	if (GrappleComponent)
	{
		// Get camera forward direction
		FVector CameraForward = GetControlRotation().Vector();
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = StartLocation + (CameraForward * 3000.0f);

		GrappleComponent->ShootGrapple(EndLocation);
	}
}

void ANinjaWizardCharacter::SelectThrowDirection(EGrappleDirection Direction)
{
	if (GrappleComponent)
	{
		GrappleComponent->SetThrowDirection(Direction);
	}
}

void ANinjaWizardCharacter::ExecuteThrow()
{
	if (GrappleComponent)
	{
		GrappleComponent->ExecuteThrow();
	}
}

// Weapon Actions
void ANinjaWizardCharacter::ThrowWeapon()
{
	if (WeaponReturnComponent)
	{
		FVector ThrowDirection = GetControlRotation().Vector();
		WeaponReturnComponent->ThrowCurrentWeapon(ThrowDirection);
	}
}

void ANinjaWizardCharacter::RecallWeapon()
{
	if (WeaponReturnComponent)
	{
		// Recall all thrown weapons
		WeaponReturnComponent->RecallAllWeapons();
	}
}
```

---

## Input Configuration

### Step 4: Set Up Enhanced Input Actions

Create these Input Actions in your project:

#### **Combat Movement Actions**
- `IA_Block` - Action (Hold)
- `IA_Parry` - Action (Pressed)
- `IA_Dodge` - Action (Pressed)
- `IA_Slide` - Action (Pressed)

#### **Grapple Actions**
- `IA_Grapple` - Action (Pressed)
- `IA_ThrowUp` - Action (Pressed)
- `IA_ThrowLeft` - Action (Pressed)
- `IA_ThrowRight` - Action (Pressed)
- `IA_ThrowForward` - Action (Pressed)
- `IA_PullToPlayer` - Action (Pressed)

#### **Weapon Actions**
- `IA_Attack` - Action (Pressed)
- `IA_ThrowWeapon` - Action (Pressed)
- `IA_RecallWeapon` - Action (Pressed)
- `IA_WeaponSlot1` - Action (Pressed)
- `IA_WeaponSlot2` - Action (Pressed)
- `IA_WeaponSlot3` - Action (Pressed)
- `IA_WeaponSlot4` - Action (Pressed)

#### **Magic Actions**
- `IA_CastFire` - Action (Pressed)
- `IA_CastWater` - Action (Pressed)
- `IA_CastEarth` - Action (Pressed)
- `IA_CastAir` - Action (Pressed)
- `IA_CastLightning` - Action (Pressed)
- `IA_CastLight` - Action (Pressed)
- `IA_CastDark` - Action (Pressed)
- `IA_CastPoison` - Action (Pressed)
- `IA_CastIce` - Action (Pressed)

#### **Inventory Actions**
- `IA_OpenInventory` - Action (Pressed)
- `IA_UsePotion` - Action (Pressed)

#### **Soul Bond Actions**
- `IA_SoulBond` - Action (Hold)
- `IA_Summon1` - Action (Pressed)
- `IA_Summon2` - Action (Pressed)

### Step 5: Bind Input Actions in Blueprint

In your `BP_NinjaWizardCharacter` Event Graph:

```
Event BeginPlay
└─> Enhanced Input Local Player Subsystem
    └─> Add Mapping Context (YourMappingContext, Priority: 0)
```

Add Input Action bindings:
```
IA_Block (Started)
└─> Call "Start Block"

IA_Block (Completed)
└─> Call "Stop Block"

IA_Parry (Triggered)
└─> Call "Perform Parry"

IA_Dodge (Triggered)
├─> Get Input Axis Value (Move Forward/Right)
├─> Make Vector (X: Forward, Y: Right, Z: 0)
└─> Call "Perform Dodge" (Direction)

IA_Grapple (Triggered)
└─> Call "Shoot Grapple"

IA_ThrowUp (Triggered)
├─> Select Throw Direction (ThrowUp)
└─> Execute Throw

IA_ThrowWeapon (Triggered)
└─> Call "Throw Weapon"

IA_RecallWeapon (Triggered)
└─> Call "Recall Weapon"
```

---

## System Integration

### Step 6: Combat Damage Integration

When player attacks an enemy, use the damage calculation system:

```cpp
void ANinjaWizardCharacter::DealDamageToEnemy(ACombatEntity* Enemy, float BaseDamage, EMagicType AttackElement)
{
	if (!Enemy || !DamageCalculationComponent || !AttributeComponent) return;

	// Get player's level and wisdom
	int32 PlayerLevel = AttributeComponent->GetDerivedStat(EDerivedStat::Level);
	int32 PlayerWisdom = AttributeComponent->GetAttribute(EAttribute::Wisdom);

	// Get enemy's element (you'll need to add this to CombatEntity)
	EMagicType EnemyElement = EMagicType::Fire; // Get from enemy

	// Check if this is a dual element attack
	bool bIsDualElement = false; // Set based on your spell system

	// Calculate final damage
	FDamageCalculationResult Result = DamageCalculationComponent->CalculateDamage(
		BaseDamage,
		AttackElement,
		EnemyElement,
		PlayerLevel,
		PlayerWisdom,
		bIsDualElement
	);

	// Apply damage to enemy
	Enemy->ApplyDamageFrom(Result.FinalDamage, this);

	// Log damage breakdown for debugging
	UE_LOG(LogTemp, Log, TEXT("%s"), *Result.DamageBreakdown);
}
```

### Step 7: Combat State Management

Track when player enters/exits combat:

```cpp
void ANinjaWizardCharacter::TakeDamage(float Damage, AActor* DamageDealer)
{
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - Damage);

	// Enter combat - stops out-of-combat regen
	if (HealingComponent)
	{
		HealingComponent->EnterCombat();
	}
}

void ANinjaWizardCharacter::OnEnemyKilled(ACombatEntity* Enemy)
{
	// Check if all enemies are dead
	TArray<AActor*> FoundEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatEntity::StaticClass(), FoundEnemies);

	bool bAllEnemiesDead = true;
	for (AActor* Actor : FoundEnemies)
	{
		if (ACombatEntity* EnemyActor = Cast<ACombatEntity>(Actor))
		{
			if (!EnemyActor->IsDead() && FVector::Dist(GetActorLocation(), EnemyActor->GetActorLocation()) < 3000.0f)
			{
				bAllEnemiesDead = false;
				break;
			}
		}
	}

	// Exit combat if no enemies nearby - starts 8 second timer for regen
	if (bAllEnemiesDead && HealingComponent)
	{
		HealingComponent->ExitCombat();
	}
}
```

### Step 8: Parry System Integration

In `CombatMovementComponent`, connect parry events to enemy AI:

```cpp
// In your character's Blueprint or C++
void ANinjaWizardCharacter::OnParrySuccessful(AActor* ParriedAttacker)
{
	// Parry already applied stun to attacker via CombatMovementComponent
	// Now apply counter damage with 2x multiplier

	if (ACombatEntity* Enemy = Cast<ACombatEntity>(ParriedAttacker))
	{
		// Deal counter damage
		float CounterDamage = 100.0f * 2.0f; // 2x multiplier from parry
		Enemy->ApplyDamageFrom(CounterDamage, this);

		UE_LOG(LogTemp, Log, TEXT("PARRY COUNTER: Dealt %.1f damage!"), CounterDamage);
	}
}
```

Bind this event in your character's `BeginPlay`:
```cpp
if (CombatMovementComponent)
{
	CombatMovementComponent->OnParrySuccessful.AddDynamic(this, &ANinjaWizardCharacter::OnParrySuccessful);
}
```

### Step 9: Grapple Combo System

Set up the grappling combo flow:

**Blueprint Event Graph Example**:
```
Grapple -> Attached to Enemy
├─> Show Throw Direction UI (Up, Left, Right, Pull, etc.)
└─> Wait for player input

Player selects "Throw Up"
├─> Execute Throw (Up)
├─> Wait 0.5 seconds
├─> Check if in air combo window
└─> If yes: Enable special air attacks

Player uses air attack
└─> Deal bonus damage while enemy is airborne

Enemy lands
└─> End air combo window
```

**C++ Implementation**:
```cpp
void ANinjaWizardCharacter::OnGrappleAttached(AActor* Target, EGrappleTargetType TargetType)
{
	if (TargetType == EGrappleTargetType::Enemy)
	{
		// Show throw direction UI
		bShowThrowDirectionUI = true;

		// Wait for player to select throw direction
		// (Handle in input bindings)
	}
}

void ANinjaWizardCharacter::OnEnemyThrown(AActor* Enemy, EGrappleDirection Direction)
{
	if (Direction == EGrappleDirection::ThrowUp)
	{
		// Start air combo window
		bCanPerformAirCombo = true;

		// Set timer to end air combo window
		GetWorldTimerManager().SetTimer(
			AirComboTimerHandle,
			this,
			&ANinjaWizardCharacter::EndAirComboWindow,
			3.0f, // 3 second window
			false
		);
	}
}
```

---

## Blueprint Setup

### Step 10: Create UI Widgets

#### **Health Bar Widget** (WBP_HealthBar)
- Bind health bar progress to `GetHealthPercent()`
- Show out-of-combat regen indicator
- Display when in combat vs regenerating

#### **Skill Tree Widget** (WBP_SkillTree)
- 13 tabs (9 elements + 4 weapons)
- Show question marks for undiscovered trees
- Show skill point count
- Enable skill unlock buttons when prerequisites met

#### **Inventory Widget** (WBP_Inventory)
- Tabs: Skills, Items, Weapons, Armor, Summons
- Equipment slots: 4 weapons, 8 armor pieces
- Show rarity colors for items
- Potion use button (disabled below 10% health)

#### **Grapple Direction Widget** (WBP_GrappleDirection)
- Radial menu showing throw directions
- Appears when grappled to enemy
- Icons for: Up, Left, Right, Forward, Back, Pull, Slam

#### **Combat State Widget** (WBP_CombatState)
- Show current combat movement state (Blocking, Dodging, etc.)
- Display parry window timing indicator
- Show stamina bar for actions

### Step 11: Configure Safe Zones

1. Place `SafeZoneVolume` actors in your level
2. Set box collision size to cover the safe area
3. Configure in Details panel:
   - ✅ Prevent Enemy Entry
   - ✅ Heal Player Inside
   - Heal Rate: 5.0 HP/sec
   - ✅ Show Safe Zone Indicator

4. The safe zone will:
   - Push enemies out if they try to enter
   - Heal player at configured rate
   - Stop aggressive mob AI at boundary

### Step 12: Configure Enemy AI

For each enemy Blueprint:

**Passive Animal**:
```
AIBehaviorComponent:
- Behavior Type: Passive
- Movement Speed: 300
- Detection Range: 1500
- Food Spawn Rate: 30s
```

**Catchable Creature**:
```
AIBehaviorComponent:
- Behavior Type: Chasing
- Movement Speed: 450
- Max Stamina: 100
- Catch Distance: 150
- Rare Item Drop Chance: 15%
```

**Area Guard Boss**:
```
AIBehaviorComponent:
- Behavior Type: Area Guard
- Territory Center: (Set in level)
- Territory Radius: 2000
- Enable Anti-Cheese: ✅
- Punishment Mob Count: 5
- Punishment Multiplier: 2.0x

CombatAIComponent:
- Is Boss: ✅
- Boss Phase 1: Health 100% - Basic attacks
- Boss Phase 2: Health 50% - Summon minions
- Boss Phase 3: Health 25% - Enraged mode
```

---

## Testing Checklist

### Combat Movement Tests
- [ ] **Block**: Hold block, take damage, verify 70% reduction
- [ ] **Block Magic**: Block magic attack, verify 50% reduction
- [ ] **Parry**: Time parry perfectly, verify stun + no damage
- [ ] **Parry Counter**: Attack during stun, verify 2x damage
- [ ] **Dodge**: Dodge through attack, verify invulnerability
- [ ] **Dodge Cooldown**: Try dodging twice quickly, verify 1s cooldown
- [ ] **Slide**: Slide under attack, verify longer distance than dodge

### Damage Calculation Tests
- [ ] **Weakness**: Fire attacks Ice enemy, verify 1.5x damage
- [ ] **Weakness**: Ice attacks Fire enemy, verify 1.5x damage
- [ ] **Dual Element**: Water+Lightning attacks Fire enemy, verify no weakness bonus
- [ ] **Level Scaling**: Level 20 player deals 2x damage vs Level 1
- [ ] **Wisdom Scaling**: 50 Wisdom deals 2x damage vs 0 Wisdom

### Grappling Tests
- [ ] **Grapple Enemy**: Shoot grapple, verify attach
- [ ] **Pull to Player**: Pull enemy, start combo
- [ ] **Throw Up**: Launch enemy, verify air combo window opens
- [ ] **Air Combo**: Perform air attacks during window
- [ ] **Throw Against Wall**: Slam enemy into wall, verify stun + damage
- [ ] **Environment Grapple**: Grapple to ceiling/wall (if enabled)

### Healing Tests
- [ ] **Combat State**: Take damage, verify no regen during combat
- [ ] **Out of Combat**: Wait 8 seconds after combat, verify regen starts
- [ ] **Regen Rate**: Verify 2 HP/sec regen
- [ ] **Safe Zone**: Enter safe zone, verify 5 HP/sec healing
- [ ] **Potion**: Use potion above 10% HP, verify healing
- [ ] **Potion Blocked**: Try potion below 10% HP, verify rejection

### Weapon Return Tests
- [ ] **Unlock Skill**: Unlock Return skill in weapon tree
- [ ] **Throw Weapon**: Throw weapon, verify flight
- [ ] **Damage on Throw**: Verify weapon hits enemies while flying
- [ ] **Switch Weapon**: Switch to slot 2 while weapon 1 is thrown
- [ ] **Recall**: Recall weapon, verify return path
- [ ] **Damage on Return**: Verify weapon hits enemies on return
- [ ] **Max Range**: Throw weapon far, verify auto-recall

### Inventory Tests
- [ ] **Add Item**: Pick up item, verify appears in inventory
- [ ] **Equip Weapon**: Equip weapon to slot 1-4
- [ ] **Switch Slots**: Switch between equipped weapons
- [ ] **Equip Armor**: Equip all 8 armor pieces
- [ ] **Stat Calculation**: Verify defense increases with armor
- [ ] **Rarity Colors**: Verify items show correct rarity colors
- [ ] **Sorting**: Sort by rarity/name/type

### Skill Tree Tests
- [ ] **Hidden Trees**: Verify undiscovered trees show question marks
- [ ] **Discover Element**: Learn element, verify tree appears
- [ ] **Unlock Skill**: Spend skill points, unlock skill
- [ ] **Prerequisites**: Try unlocking skill without prerequisite, verify blocked
- [ ] **Return Skill**: Unlock Return skill, verify weapon can be thrown
- [ ] **Reset Tree**: Reset tree, verify points refunded

### AI Behavior Tests
- [ ] **Passive Mob**: Verify wanders and flees from player
- [ ] **Chasing Mob**: Get close, verify it flees, chase and catch
- [ ] **Neutral Mob**: Verify ignores player until attacked
- [ ] **Area Guard**: Attack from outside territory, verify punishment spawns
- [ ] **Aggressive Boss**: Verify chases player, stops at safe zone boundary

---

## Common Issues & Solutions

### Issue: Pointers to USTRUCT compilation errors
**Solution**: These functions are already marked as C++ only. They work in code but won't show in Blueprint. Use the array-returning alternatives in Blueprints.

### Issue: Combat doesn't trigger healing stop
**Solution**: Make sure to call `HealingComponent->EnterCombat()` whenever player attacks or takes damage.

### Issue: Grapple direction UI doesn't show
**Solution**: Bind to `OnGrappleAttached` event in your character Blueprint and create UI widget manually.

### Issue: Weapon return skill not working
**Solution**: Check if the Return skill is unlocked in the weapon's skill tree. Use `SkillTreeComponent->HasReturnWeaponSkill(WeaponStyle)`.

### Issue: Parry timing feels impossible
**Solution**: Adjust `ParryWindowDuration` in CombatMovementComponent (default 0.3s). Try 0.4-0.5s for easier timing.

### Issue: Enemy AI not using abilities
**Solution**: Make sure `CombatAIComponent` is added to enemy and `CombatRole` is set (Warrior/Mage/Archer/Boss).

### Issue: Elemental weaknesses not working
**Solution**: Make sure both attacker and defender have a `DamageCalculationComponent` and are using `CalculateDamage()` function.

### Issue: Summons count doesn't scale with wisdom
**Solution**: Check that `AttributeComponent` has correct scaling: `MaxActiveCreatures = Wisdom * 2`, `SoulBondCapacity = Wisdom * 10`.

---

## Next Steps

### Immediate Actions
1. ✅ Add all components to `BP_NinjaWizardCharacter`
2. ✅ Set up Enhanced Input with all actions
3. ✅ Create basic UI widgets (Health, Inventory, Skills)
4. ✅ Place SafeZoneVolumes in starting area
5. ✅ Configure at least one enemy of each AI type

### Short Term Goals
- Create weapon and armor items with stats
- Design skill trees for each element/weapon
- Create boss encounters with phases
- Build tutorial area with safe zone
- Implement inventory UI with tabs

### Long Term Goals
- Balance damage scaling for endgame (Level 100+, Wisdom 100+)
- Create unique Dragon weapons (Dragon rarity tier)
- Design 200 summon army combat scenarios
- Build progression from tutorial to endgame bosses
- Create element combination spell effects

---

## Quick Reference: Key Functions

### Dealing Damage
```cpp
// Calculate and apply damage with elemental scaling
FDamageCalculationResult Result = DamageCalcComponent->CalculateDamage(
    BaseDamage, AttackElement, DefenderElement, Level, Wisdom, bIsDualElement);
Enemy->ApplyDamageFrom(Result.FinalDamage, this);
```

### Combat Movement
```cpp
CombatMovementComponent->StartBlocking();  // Block attacks
CombatMovementComponent->StartParry();     // Perfect timing defense
CombatMovementComponent->StartDodge(Dir);  // I-frames dodge
CombatMovementComponent->StartSlide(Dir);  // Fast slide
```

### Grappling
```cpp
GrappleComponent->ShootGrapple(TargetLoc);           // Shoot hook
GrappleComponent->SetThrowDirection(Direction);      // Choose throw
GrappleComponent->ExecuteThrow();                    // Throw enemy
bool bCanAirCombo = GrappleComponent->IsInAirComboWindow();  // Check window
```

### Healing
```cpp
HealingComponent->EnterCombat();   // Stop regen
HealingComponent->ExitCombat();    // Start 8s timer for regen
HealingComponent->TryUsePotion(PotionID);  // Use potion (10% minimum)
```

### Inventory
```cpp
InventoryComponent->AddWeapon(WeaponData);           // Add weapon
InventoryComponent->EquipWeapon(WeaponID, SlotIndex); // Equip to slot
InventoryComponent->SwitchToWeaponSlot(2);           // Switch to slot 2
bool bHas = InventoryComponent->HasItem(ItemID);     // Check inventory
```

### Skill Trees
```cpp
SkillTreeComponent->DiscoverTree(TreeType);              // Reveal tree
SkillTreeComponent->UnlockSkill(TreeType, NodeID);       // Unlock skill
bool bHasReturn = SkillTreeComponent->HasReturnWeaponSkill(WeaponStyle);
```

### Weapon Return
```cpp
WeaponReturnComponent->ThrowWeapon(SlotIndex, Direction);  // Throw
WeaponReturnComponent->RecallWeapon(WeaponID);            // Recall specific
WeaponReturnComponent->RecallAllWeapons();                // Recall all
```

---

## Congratulations!

You now have a complete foundation for an amazing action RPG! This system includes:

✅ **9 Element Magic System** with dual combinations
✅ **4 Weapon Styles** with mastery progression
✅ **Advanced Combat** with parry/dodge/block/grapple
✅ **Elemental Weaknesses** with level/wisdom scaling
✅ **Massive Army System** (20-200 summons)
✅ **Universal Soul Bonding** (all enemies)
✅ **13 Skill Trees** with discovery system
✅ **Unlimited Inventory** with 7 rarity tiers
✅ **Intelligent Enemy AI** with 5 behavior types
✅ **Boss Phase System** with adaptive difficulty
✅ **Grappling Hook** with directional throws and air combos
✅ **Smart Healing System** with out-of-combat regen

**This is an incredibly solid foundation for a phenomenal game!** 🎮✨

Happy developing! 🚀
