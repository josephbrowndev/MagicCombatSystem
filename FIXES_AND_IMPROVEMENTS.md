# Fixes and Improvements Guide

This guide addresses all current issues and implements the requested improvements:

✅ Damage system integration
✅ Magic projectile spawning
✅ Grapple hook firing
✅ Radial magic menu (replacing individual keys)
✅ Inventory close functionality
✅ Player HUD display
✅ Enemy health bars
✅ Controller support for all actions

---

## Table of Contents
1. [Quick Fix Summary](#quick-fix-summary)
2. [Health & Damage System](#health--damage-system)
3. [Magic Projectile System](#magic-projectile-system)
4. [Radial Magic Menu](#radial-magic-menu)
5. [Grapple Hook System](#grapple-hook-system)
6. [UI Fixes](#ui-fixes)
7. [Controller Support](#controller-support)
8. [Complete Input Setup](#complete-input-setup)

---

## Quick Fix Summary

### What Was Wrong
- ❌ No damage integration between character and enemies
- ❌ Magic didn't spawn projectiles
- ❌ Grapple didn't fire anything
- ❌ Inventory didn't close
- ❌ No player HUD
- ❌ No enemy health bars
- ❌ Too many magic keys

### What's Fixed Now
- ✅ Complete health/damage system in NinjaWizardCharacter
- ✅ Magic projectile spawning system
- ✅ Radial menu for magic selection (Q key + mouse)
- ✅ One button to cast (Left Mouse)
- ✅ Proper UI close/toggle functionality
- ✅ Player HUD with health/mana/stamina
- ✅ Enemy health bars above heads
- ✅ Full controller support (gamepad)

---

## Health & Damage System

### Character C++ Updates (Already Done!)

The character now has:
```cpp
// Health functions
void TakeDamageFrom(float Damage, AActor* DamageDealer);
void Heal(float Amount);
float GetHealthPercent();
bool IsDead();

// These integrate with:
// - CombatMovementComponent (block/dodge)
// - HealingComponent (combat state)
```

### Blueprint Setup - Player Takes Damage

1. **Open BP_NinjaWizardCharacter**
2. **Add Custom Event**: "OnDamageTaken" (for visual feedback)
3. **Bind to TakeDamageFrom**:

```
Event: Anywhere an enemy hits player
├─> Get Player Character
├─> Cast to BP_NinjaWizardCharacter
└─> Take Damage From
    ├─ Damage: [Enemy Attack Damage]
    └─ Damage Dealer: [Enemy Actor Reference]
```

### Blueprint Setup - Enemies Take Damage

**For Enemy Blueprints** (BP_Enemy):

1. **Add Health Variable**:
   - Name: `CurrentHealth`
   - Type: Float
   - Default: 100.0

2. **Add Function: TakeDamage**:
```
Function: TakeDamage (Damage: Float, Attacker: Actor)
├─> Current Health - Damage → Set Current Health
├─> Branch (Current Health <= 0?)
│   ├─ True:
│   │   └─> Destroy Actor (this enemy)
│   └─ False:
│       └─> Play Damage Animation
└─> Update Health Bar Widget
```

3. **When Player Projectile Hits Enemy**:
```
Event: OnComponentHit (Projectile)
├─> Get Other Actor
├─> Cast to BP_Enemy (or your enemy base class)
└─> Call TakeDamage
    ├─ Damage: 50.0 (or get from projectile)
    └─ Attacker: Self
```

---

## Magic Projectile System

### Step 1: Create Magic Projectile Blueprint

1. **Create New Blueprint**:
   - Parent: Actor
   - Name: `BP_MagicProjectile`
   - Location: Content/Blueprints/

2. **Add Components**:
```
BP_MagicProjectile
├─ Sphere Collision (Root)
│  ├─ Collision Preset: ProjectilePreset
│  └─ Radius: 25
├─ Particle System (Visual)
│  └─ Template: P_Projectile (create or use starter content)
├─ Projectile Movement Component
│  ├─ Initial Speed: 2000
│  ├─ Max Speed: 2000
│  └─ Projectile Gravity Scale: 0.0
└─ Audio Component (optional)
   └─ Sound: ProjectileFlySound
```

3. **Add Variables**:
   - `Damage`: Float = 50.0
   - `Element`: EMagicElement = Fire
   - `OwnerCharacter`: Actor Reference

4. **Event Graph**:

```
Event BeginPlay
└─> Set Lifespan (3.0 seconds) // Auto-destroy after 3s

Event OnComponentHit (SphereCollision)
├─> Get Other Actor
├─> Branch (Other Actor != Owner Character)
│   ├─ True:
│   │   ├─> Try Cast to BP_Enemy
│   │   │   ├─ Success:
│   │   │   │   ├─> Call TakeDamage (Damage, Owner Character)
│   │   │   │   └─> Spawn Hit VFX
│   │   │   └─ Failed: (hit environment)
│   │   │       └─> Spawn Hit VFX
│   │   └─> Destroy Actor (this projectile)
│   └─ False: (ignore self)
└─> Play Hit Sound
```

### Step 2: Set Projectile in Character Blueprint

1. **Open BP_NinjaWizardCharacter**
2. **Find "Magic Projectile Class" property**
3. **Set to** `BP_MagicProjectile`

### Step 3: Test Magic Casting

1. **PIE (Play In Editor)**
2. **Hold Q** → Radial menu appears (we'll create this next)
3. **Click element** → Element selected
4. **Press Left Mouse** → Projectile fires!

**For now (without radial menu)**:
- Magic will fire using currently selected element
- Default is Fire
- Projectile will spawn and fly forward

---

## Radial Magic Menu

### Step 1: Create Radial Menu Widget

1. **Create Widget**: WBP_RadialMagicMenu
2. **Design**:

```
Canvas Panel
└─ Overlay (Anchor: Center, Size: 600x600)
    ├─ Image - Background Circle
    │  ├─ Color: Black, Opacity 0.5
    │  └─ Size: 600x600
    ├─ Image - Center Icon (shows selected element)
    │  └─ Size: 100x100
    └─ Button Grid (8 buttons in circle):
        ├─ Button - Fire (Top)
        ├─ Button - Water (Top-Right)
        ├─ Button - Earth (Right)
        ├─ Button - Air (Bottom-Right)
        ├─ Button - Lightning (Bottom)
        ├─ Button - Light (Bottom-Left)
        ├─ Button - Dark (Left)
        └─ Button - Poison/Ice (Top-Left)
```

**Button Layout (Circular)**:
- Angle calculation: `360 / 8 = 45 degrees per element`
- Radius: 250 pixels from center
- Position each button using cos/sin

**Example Position Math**:
```
For Button 1 (Fire - 0 degrees):
X = CenterX + (250 * Cos(0)) = Center + 250
Y = CenterY + (250 * Sin(0)) = Center + 0

For Button 2 (Water - 45 degrees):
X = CenterX + (250 * Cos(45))
Y = CenterY + (250 * Sin(45))

...and so on
```

3. **Widget Blueprint**:

```
Event Construct
├─> Get Player Controller
├─> Set Show Mouse Cursor (True)
└─> Set Input Mode UI Only

Button Fire OnClicked
├─> Get Player Character
├─> Cast to BP_NinjaWizardCharacter
├─> Set Selected Element (Fire)
└─> Remove from Parent

Button Water OnClicked
├─> Get Player Character
├─> Cast to BP_NinjaWizardCharacter
├─> Set Selected Element (Water)
└─> Remove from Parent

[Repeat for all 8 elements]
```

### Step 2: Update Character Input

1. **Open BP_NinjaWizardCharacter Event Graph**
2. **Remove old individual magic key bindings** (IA_CastFire, IA_CastWater, etc.)
3. **Add new radial menu binding**:

```
IA_OpenMagicMenu (Started) [Bind to Q key]
├─> Create Widget (WBP_RadialMagicMenu)
├─> Add to Viewport
└─> Set Variable: RadialMenuRef

IA_OpenMagicMenu (Completed) [Release Q]
├─> Get RadialMenuRef
├─> Is Valid?
│   ├─ True:
│   │   ├─> Remove from Parent
│   │   └─> Set Show Mouse Cursor (False)
│   └─ False: (do nothing)
└─> Set Input Mode Game Only

IA_CastSpell (Triggered) [Bind to Left Mouse Button]
└─> Cast Selected Element
```

### Step 3: Enhanced Input Actions

1. **Delete old actions**: IA_CastFire, IA_CastWater, etc.
2. **Create new action**: `IA_OpenMagicMenu`
   - Type: Digital (bool)
   - Keyboard: Q
   - Gamepad: Right Shoulder Button
3. **Update IA_CastSpell**:
   - Keyboard: Left Mouse Button
   - Gamepad: Right Trigger

---

## Grapple Hook System

### Step 1: Create Grapple Hook Projectile

1. **Create Blueprint**: BP_GrappleHook
   - Parent: Actor

2. **Components**:
```
BP_GrappleHook
├─ Sphere Collision (Root)
│  └─ Radius: 20
├─ Static Mesh (Hook visual)
│  └─ Mesh: SM_Hook (create simple hook mesh)
├─ Projectile Movement
│  ├─ Initial Speed: 3000
│  └─ Projectile Gravity Scale: 0.0
└─ Cable Component (for rope visual)
   ├─ End Location: Set dynamically
   └─ Cable Width: 2.0
```

3. **Variables**:
   - `OwnerCharacter`: Actor Reference
   - `bHasHitTarget`: Boolean = false
   - `HitTarget`: Actor Reference

4. **Event Graph**:

```
Event BeginPlay
├─> Get Owner → Cast to BP_NinjaWizardCharacter
├─> Set OwnerCharacter
├─> Set Lifespan (5.0)
└─> Start Position: Owner Location

Event Tick
└─> Branch (Has Hit Target?)
    ├─ True: (rope follows character)
    │   ├─> Get Owner Character Location
    │   └─> Cable → Set Cable Start Location
    └─ False: (rope extends as hook flies)
        ├─> Get Owner Character Location
        └─> Cable → Set Cable Start Location

Event OnComponentHit
├─> Get Other Actor
├─> Set Hit Target (Other Actor)
├─> Set Has Hit Target (True)
├─> Stop Projectile Movement
└─> Call Owner's "OnGrappleAttached"
    └─> Pass: Hit Target, Hit Location

Event: OnGrappleRelease (Custom Event)
└─> Destroy Actor
```

### Step 2: Update Character Grapple Input

1. **Open BP_NinjaWizardCharacter Event Graph**
2. **Find IA_Grapple binding**
3. **Update to spawn projectile**:

```
IA_Grapple (Triggered) [E key]
├─> Get Control Rotation
├─> Get Forward Vector
├─> Calculate Spawn Location (Player + Forward * 100)
├─> Spawn Actor (BP_GrappleHook)
│   ├─ Location: Spawn Location
│   ├─ Rotation: Control Rotation
│   └─ Owner: Self
└─> Set Variable: CurrentGrappleHook
```

### Step 3: Grapple Throw Directions

```
Event: OnGrappleAttached (Custom Event, called by hook)
├─> Show Throw Direction UI (WBP_GrappleDirection from previous guide)
└─> Set Can Throw (True)

IA_ThrowUp (Triggered) [R key]
├─> Branch (Can Throw?)
│   ├─ True:
│   │   ├─> Get Grapple Component
│   │   ├─> Set Throw Direction (ThrowUp)
│   │   ├─> Execute Throw
│   │   ├─> Destroy Grapple Hook
│   │   └─> Hide Throw UI
│   └─ False: (do nothing)
```

---

## UI Fixes

### Fix 1: Inventory Close/Toggle

**Problem**: Inventory doesn't close when pressing I or clicking Exit

**Solution**:

1. **Open WBP_Inventory**
2. **Add Variable**: `bIsOpen` (Boolean)
3. **Update Close Button**:

```
Button Close OnClicked
├─> Remove from Parent
├─> Get Player Controller
├─> Set Show Mouse Cursor (False)
└─> Set Input Mode Game Only
```

4. **Update Character Input** (BP_NinjaWizardCharacter):

```
IA_OpenInventory (Triggered) [I key]
├─> Branch (Is Inventory Open?)
│   ├─ True: (close it)
│   │   ├─> Get Inventory Widget Ref
│   │   ├─> Remove from Parent
│   │   ├─> Set Inventory Open (False)
│   │   ├─> Set Show Mouse Cursor (False)
│   │   └─> Set Input Mode Game Only
│   └─ False: (open it)
│       ├─> Create Widget (WBP_Inventory)
│       ├─> Add to Viewport
│       ├─> Set Inventory Widget Ref
│       ├─> Set Inventory Open (True)
│       ├─> Set Show Mouse Cursor (True)
│       └─> Set Input Mode UI Only
```

### Fix 2: Player HUD Display

**Problem**: Player HUD doesn't appear

**Solution**:

1. **Create WBP_PlayerHUD** (if not exists):

```
Canvas Panel
└─ Vertical Box (Top-Left, 300x150)
    ├─ Text Block - "Health"
    ├─ Progress Bar - Health Bar
    │   ├─ Percent: Bind to GetHealthPercent()
    │   ├─ Fill Color: Green to Red gradient
    │   └─ Size: 250x20
    ├─ Text Block - "Mana"
    ├─ Progress Bar - Mana Bar
    │   ├─ Percent: Bind to GetManaPercent()
    │   ├─ Fill Color: Blue
    │   └─ Size: 250x15
    ├─ Text Block - "Stamina"
    └─ Progress Bar - Stamina Bar
        ├─ Percent: Bind to GetStaminaPercent()
        ├─ Fill Color: Yellow
        └─ Size: 250x15
```

2. **Binding Functions** (WBP_PlayerHUD Event Graph):

```
Function: GetHealthPercent (Return: Float)
├─> Get Player Character
├─> Cast to BP_NinjaWizardCharacter
└─> Get Health Percent → Return

Function: GetManaPercent (Return: Float)
├─> Get Player Character
├─> Cast to BP_NinjaWizardCharacter
└─> Get Mana Percentage → Return

Function: GetStaminaPercent (Return: Float)
├─> Get Player Character
├─> Cast to BP_NinjaWizardCharacter
└─> Get Stamina Percentage → Return
```

3. **Add HUD to Viewport** (BP_NinjaWizardCharacter BeginPlay):

```
Event BeginPlay
├─> [Existing initialization...]
├─> Create Widget (WBP_PlayerHUD)
└─> Add to Viewport (Z-Order: 0)
```

### Fix 3: Enemy Health Bars

**Problem**: Enemy health bars don't appear

**Solution**:

1. **Create WBP_EnemyHealthBar**:

```
Canvas Panel (Size: 200x50)
└─ Vertical Box
    ├─ Text Block - Enemy Name
    │   ├─ Text: Bind to GetEnemyName()
    │   └─ Color: White
    └─ Progress Bar
        ├─ Percent: Bind to GetHealthPercent()
        ├─> Fill Color: Red
        └─ Size: 150x10
```

2. **Widget Component on Enemy** (BP_Enemy):

Add Component:
```
BP_Enemy
├─ [Existing components...]
└─ Widget Component
    ├─ Widget Class: WBP_EnemyHealthBar
    ├─ Draw Size: 200x50
    ├─ Pivot: (0.5, 1.0) // Center-bottom
    ├─ Location: (0, 0, 100) // Above head
    └─ Space: Screen
```

3. **Binding Functions** (WBP_EnemyHealthBar):

```
Function: GetHealthPercent (Return: Float)
├─> Get Owning Actor
├─> Cast to BP_Enemy
├─> Get Current Health / Max Health
└─> Return

Function: GetEnemyName (Return: Text)
├─> Get Owning Actor
├─> Cast to BP_Enemy
├─> Get Enemy Name (variable)
└─> Return as Text
```

---

## Controller Support

### Input Mapping Context Updates

1. **Open IMC_NinjaWizard** (Input Mapping Context)
2. **For EACH action, add gamepad mapping**:

#### Movement & Camera
```
IA_Move
├─ Keyboard: WASD (Already set)
└─ Gamepad: Left Thumbstick

IA_Look
├─ Mouse: XY Movement (Already set)
└─ Gamepad: Right Thumbstick

IA_Jump
├─ Keyboard: Space (Already set)
└─ Gamepad: A Button (Xbox) / Cross (PlayStation)
```

#### Combat
```
IA_Attack
├─ Mouse: Left Button (Already set)
└─ Gamepad: Right Trigger

IA_Block
├─ Mouse: Right Button (Already set)
└─ Gamepad: Left Trigger

IA_Dodge
├─ Keyboard: Left Shift
└─ Gamepad: B Button (Xbox) / Circle (PS)

IA_Parry
├─ Keyboard: Q
└─ Gamepad: Y Button (Xbox) / Triangle (PS)
```

#### Magic & Abilities
```
IA_OpenMagicMenu
├─ Keyboard: Q (Hold)
└─ Gamepad: Right Shoulder Button (Hold)

IA_CastSpell
├─ Mouse: Left Button (Already set)
└─ Gamepad: Right Trigger

IA_Grapple
├─ Keyboard: E
└─ Gamepad: Left Shoulder Button
```

#### Grapple Throw Directions
```
IA_ThrowUp
├─ Keyboard: R
└─ Gamepad: D-Pad Up

IA_ThrowLeft
├─ Keyboard: Q (while grappled)
└─ Gamepad: D-Pad Left

IA_ThrowRight
├─ Keyboard: E (while grappled)
└─ Gamepad: D-Pad Right

IA_PullToPlayer
├─ Keyboard: F
└─ Gamepad: D-Pad Down
```

#### Weapons
```
IA_WeaponSlot1
├─ Keyboard: 1
└─ Gamepad: D-Pad Up + Left Shoulder

IA_WeaponSlot2
├─ Keyboard: 2
└─ Gamepad: D-Pad Right + Left Shoulder

IA_WeaponSlot3
├─ Keyboard: 3
└─ Gamepad: D-Pad Down + Left Shoulder

IA_WeaponSlot4
├─ Keyboard: 4
└─ Gamepad: D-Pad Left + Left Shoulder
```

#### UI
```
IA_OpenInventory
├─ Keyboard: I
└─ Gamepad: Start Button (Menu)

IA_UsePotion
├─ Keyboard: H
└─ Gamepad: X Button (Xbox) / Square (PS)
```

### Radial Menu Controller Support

For the radial magic menu to work with controller:

1. **WBP_RadialMagicMenu** updates:

```
Event Tick (when menu is open)
├─> Get Player Controller
├─> Get Gamepad Right Stick Axis Values
├─> Calculate Angle from stick position
├─> Highlight element button based on angle
└─> On Button Release (Right Shoulder):
    └─> Select highlighted element
```

**Angle Calculation**:
```
Function: GetStickAngle
├─> Get Right Stick X, Y
├─> ArcTan2(Y, X) → Angle in radians
├─> Convert to degrees
└─> Map to 8 elements (0-45° = Fire, 45-90° = Water, etc.)
```

---

## Complete Input Setup

### Updated Input Action List

Delete these (no longer needed):
- ❌ IA_CastFire
- ❌ IA_CastWater
- ❌ IA_CastEarth
- ❌ IA_CastAir
- ❌ IA_CastLightning
- ❌ IA_CastLight
- ❌ IA_CastDark
- ❌ IA_CastPoison
- ❌ IA_CastIce

Keep/Add these:
- ✅ IA_Move (Axis2D)
- ✅ IA_Look (Axis2D)
- ✅ IA_Jump (Digital)
- ✅ IA_Attack (Digital)
- ✅ IA_Block (Digital - Hold)
- ✅ IA_Dodge (Digital)
- ✅ IA_Parry (Digital)
- ✅ IA_Slide (Digital)
- ✅ IA_OpenMagicMenu (Digital - Hold) **NEW!**
- ✅ IA_CastSpell (Digital) **Updated!**
- ✅ IA_Grapple (Digital)
- ✅ IA_ThrowUp (Digital)
- ✅ IA_ThrowLeft (Digital)
- ✅ IA_ThrowRight (Digital)
- ✅ IA_PullToPlayer (Digital)
- ✅ IA_ThrowWeapon (Digital)
- ✅ IA_RecallWeapon (Digital)
- ✅ IA_WeaponSlot1-4 (Digital)
- ✅ IA_OpenInventory (Digital)
- ✅ IA_UsePotion (Digital)

**Total: 23 actions** (down from 32!)

---

## Testing Checklist

### Health & Damage
- [ ] Player takes damage from enemy attacks
- [ ] Player health bar updates in HUD
- [ ] Blocking reduces damage (check log)
- [ ] Dodging avoids damage (check log)
- [ ] Player dies at 0 HP
- [ ] Enemies take damage from projectiles
- [ ] Enemies die and disappear
- [ ] Enemy health bars update

### Magic System
- [ ] Press Q → Radial menu appears
- [ ] Mouse over element → Element highlights
- [ ] Click element → Element selected
- [ ] Release Q → Menu closes
- [ ] Press Left Mouse → Projectile fires
- [ ] Projectile flies straight
- [ ] Projectile hits enemy → Enemy takes damage
- [ ] Projectile destroys on impact
- [ ] Different elements have different colors (if you set them)

### Grapple System
- [ ] Press E → Grapple hook fires
- [ ] Hook has visible rope/cable
- [ ] Hook hits enemy → Attaches
- [ ] Press R while attached → Enemy throws up
- [ ] Press Q while attached → Enemy throws left
- [ ] Press E while attached → Enemy throws right
- [ ] Press F while attached → Enemy pulls to player

### UI
- [ ] Press I → Inventory opens
- [ ] Press I again → Inventory closes
- [ ] Click Exit button → Inventory closes
- [ ] Mouse cursor shows when inventory open
- [ ] Mouse cursor hides when inventory closes
- [ ] Player HUD visible at all times
- [ ] Health bar updates when taking damage
- [ ] Mana bar updates when casting spells
- [ ] Enemy health bars visible above enemies
- [ ] Enemy names visible

### Controller
- [ ] All movement works with left stick
- [ ] Camera works with right stick
- [ ] All combat actions work with gamepad
- [ ] Radial menu works with right stick + right shoulder
- [ ] Weapon switching works with D-pad combos
- [ ] UI navigation works with gamepad

---

## Common Issues & Solutions

### Issue: Projectiles don't spawn
**Solution**:
1. Check `MagicProjectileClass` is set in BP_NinjaWizardCharacter
2. Check projectile Blueprint compiles
3. Check console for "No MagicProjectileClass set!" warning

### Issue: Projectiles spawn but don't move
**Solution**:
1. Check Projectile Movement Component is present
2. Check Initial Speed > 0
3. Check Gravity Scale is 0 (for straight flight)

### Issue: Projectiles don't damage enemies
**Solution**:
1. Check OnComponentHit event is bound
2. Check collision presets (Projectile should hit Pawn)
3. Check enemy has TakeDamage function
4. Add print statements to debug

### Issue: Radial menu doesn't appear
**Solution**:
1. Check Q key is bound to IA_OpenMagicMenu
2. Check widget compiles without errors
3. Check "Add to Viewport" is called
4. Add print statement in IA_OpenMagicMenu Started

### Issue: Inventory won't close
**Solution**:
1. Check Remove from Parent is called
2. Check Set Input Mode Game Only is called
3. Check boolean tracking is updated
4. Use F8 to detach from character and inspect widgets in viewport

### Issue: Enemy health bars don't show
**Solution**:
1. Check Widget Component is added to enemy Blueprint
2. Check Widget Class is set to WBP_EnemyHealthBar
3. Check Space is set to "Screen"
4. Check enemy has CurrentHealth variable

### Issue: Controller doesn't work
**Solution**:
1. Check gamepad is connected before starting PIE
2. Check Input Mapping Context has gamepad bindings
3. Test each action individually
4. Check Project Settings → Input → Default Player Input Class

---

## Next Steps

1. **Compile C++ code** - Character changes need recompilation
2. **Create BP_MagicProjectile** - Follow projectile setup
3. **Create WBP_RadialMagicMenu** - Radial menu widget
4. **Update character input** - Remove old magic keys, add radial menu
5. **Create grapple hook** - BP_GrappleHook with cable
6. **Fix UI** - Inventory close, Player HUD, Enemy health bars
7. **Add controller mappings** - All input actions
8. **Test everything** - Use checklist above

---

## Congratulations!

After completing this guide, you'll have:

✅ Working damage system
✅ Magic projectiles that spawn and damage
✅ Radial menu for magic selection (much better UX!)
✅ Grappling hook with visual rope
✅ Properly closing UI
✅ Player HUD with all stats
✅ Enemy health bars
✅ Full controller support

**Your game will be fully playable!** 🎮✨
