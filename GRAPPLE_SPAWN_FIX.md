# Grapple Hook Not Spawning - Fix Guide

## The Problem

You see "Hello" (input works) but BP_GrappleHook doesn't spawn. This is almost always a **spawn collision failure**.

---

## Quick Fix - Do This Now!

### In BP_NinjaWizardCharacter, IA_Grapple Event:

**Find your Spawn Actor node** and change these settings:

1. **Click on the Spawn Actor node**
2. **In Details panel** (when node is selected):
   - **Collision Handling Override**: Change to **Always Spawn, Ignore Collisions**
   - (NOT "Default" or "Adjust If Possible Else Don't Spawn")

**This alone fixes 90% of spawn failures!**

---

## Full Correct Setup

```
IA_Grapple (Triggered)
├─> Print String ("Grapple Key Pressed") ← You have this (shows "hello")
├─> Get Actor Location (Self)
├─> Get Actor Forward Vector (Self)
├─> Vector * Float (Forward Vector * 100) → SpawnOffset
├─> Vector + Vector (Actor Location + SpawnOffset) → SpawnLocation
├─> Get Control Rotation (Self) → SpawnRotation
└─> Spawn Actor from Class
    ├─ Class: BP_GrappleHook ← MUST be set!
    ├─ Spawn Transform:
    │   ├─ Location: SpawnLocation
    │   └─ Rotation: SpawnRotation
    ├─ Collision Handling Override: Always Spawn, Ignore Collisions ← KEY!
    ├─ Owner: Self
    └─> Return Value → Branch (Is Valid?)
        ├─ Valid:
        │   ├─> Print String ("Hook Spawned!") ← Add this to test
        │   └─> Set Variable: CurrentGrappleHook
        └─ Invalid:
            └─> Print String ("SPAWN FAILED!") ← Add this to debug
```

---

## Detailed Step-by-Step Fix

### Step 1: Check Spawn Actor Settings

1. **Open BP_NinjaWizardCharacter**
2. **Find IA_Grapple event**
3. **Click the "Spawn Actor" node**
4. **Look at Details panel (right side)**
5. **Find "Collision Handling Override"**
6. **Set to**: **Always Spawn, Ignore Collisions**

### Step 2: Add Debug Print

**After Spawn Actor**, add:

```
Spawn Actor from Class
└─> Return Value (drag blue pin)
    └─> Branch (Is Valid?)
        ├─ True → Print String ("HOOK SPAWNED SUCCESSFULLY!")
        └─ False → Print String ("SPAWN FAILED - NULL RETURNED!")
```

**Test**: Press E and check console
- If you see "SPAWN FAILED" → Collision issue
- If you see nothing → Spawn Actor not being reached

### Step 3: Verify BP_GrappleHook Class is Set

In the **Spawn Actor** node:
- **Class** dropdown should show **BP_GrappleHook**
- If it says "None" or "Select Class" → That's the problem!

**To set it**:
1. Click the **Class** dropdown
2. Search for "BP_GrappleHook"
3. Select it
4. Compile

### Step 4: Check BP_GrappleHook Compiles

1. **Open BP_GrappleHook**
2. **Click Compile** (top left)
3. **Check for errors** in the Compiler Results
4. **Fix any errors** before testing

**Common errors in BP_GrappleHook**:
- Missing Cable Component
- Missing Projectile Movement Component
- Event Tick trying to access invalid reference

---

## Alternative: Simple Spawn Test

Replace your entire IA_Grapple with this minimal test:

```
IA_Grapple (Triggered)
├─> Get Actor Location
├─> Vector + Vector (Location + [0,0,100]) → SpawnLoc (spawn above player)
└─> Spawn Actor from Class
    ├─ Class: BP_GrappleHook
    ├─ Spawn Transform → Make Transform
    │   └─ Location: SpawnLoc
    ├─ Collision Handling: Always Spawn, Ignore Collisions
    └─> Print String ("Spawned!")
```

**If this works**: Your original spawn location/rotation was the problem
**If this doesn't work**: BP_GrappleHook has an issue

---

## Check BP_GrappleHook Setup

### Components (in BP_GrappleHook):

**Must have**:
```
Components Panel:
├─ SphereCollision (Root) ← MUST be root!
├─ StaticMesh (child of Sphere)
├─ ProjectileMovementComponent
└─ CableComponent
```

**Common mistake**: No root component!

**To fix**:
1. Open BP_GrappleHook
2. Components panel (top left)
3. **SphereCollision** should be **bold** (root)
4. If not, right-click Sphere → **Set as Root**

### Event BeginPlay Issues

**Your Event BeginPlay might be causing instant destruction**:

```
Event BeginPlay
├─> Print String ("Hook BeginPlay Called") ← Add this!
├─> Get Owner
│   └─> Is Valid?
│       ├─ True:
│       │   └─> Print String ("Owner is Valid")
│       └─ False:
│           └─> Print String ("Owner is NULL!")
└─> Set Lifespan (5.0)
```

**Test**: If you see "Hook BeginPlay Called" then hook spawned but destroyed quickly

---

## Most Common Issues (Checklist)

- [ ] **Collision Handling** = "Always Spawn, Ignore Collisions"
- [ ] **Class** is set to BP_GrappleHook (not None)
- [ ] **BP_GrappleHook compiles** with no errors
- [ ] **SphereCollision is root** component
- [ ] **Projectile Movement** component exists
- [ ] **Event BeginPlay** doesn't have errors
- [ ] **Spawn location** is valid (not [0,0,0] or inside player)

---

## Visual Debug

Add this to BP_GrappleHook Event BeginPlay:

```
Event BeginPlay
├─> Print String ("GRAPPLE HOOK SPAWNED!")
│   ├─ Duration: 10.0
│   ├─ Text Color: RED
│   └─ Font Size: 50
└─> Draw Debug Sphere
    ├─ Center: Get Actor Location
    ├─ Radius: 100
    ├─ Color: Red
    └─ Duration: 5.0
```

**Test**:
- Press E
- Look for **big red sphere** where hook spawns
- Look for **"GRAPPLE HOOK SPAWNED!"** in huge red text

**If you see these**: Hook is spawning! Just not visible/moving
**If you don't see these**: Hook isn't spawning at all

---

## Projectile Movement Not Working

If hook spawns but doesn't move:

### In BP_GrappleHook, check Projectile Movement Component:

**Settings**:
```
Projectile Movement Component:
├─ Initial Speed: 3000 ← Must be > 0
├─ Max Speed: 3000
├─ Velocity: [Leave default, will be set by Initial Speed]
├─ Should Bounce: False
├─ Projectile Gravity Scale: 0.0 ← For straight flight
└─ Is Homing Projectile: False
```

**Event BeginPlay**:
```
Event BeginPlay
└─> Get Component by Class (Projectile Movement)
    └─> Set Velocity
        ├─> New Velocity: Get Actor Forward Vector * 3000
        └─> Print String ("Velocity Set!")
```

---

## The Real Problem (95% of cases)

**Collision Handling Override is NOT set!**

When you don't set this, Unreal does this:
1. Try to spawn hook
2. Hook overlaps with player capsule
3. "Can't spawn here!" → Returns null
4. No error message, just fails silently

**Setting "Always Spawn, Ignore Collisions"** tells Unreal:
1. Spawn it no matter what
2. Ignore collision during spawn
3. Return the spawned actor

---

## Complete Working Example

**In BP_NinjaWizardCharacter**:

```
IA_Grapple (Triggered)
├─> Print String ("E Pressed") ← You have this
│
├─> Get Actor Location → VarLocation
├─> Get Actor Forward Vector → VarForward
├─> VarForward * 100 → VarOffset
├─> VarLocation + VarOffset → VarSpawnLoc
│
├─> Spawn Actor BP_GrappleHook
│   ├─ Spawn Transform:
│   │   └─ Make Transform:
│   │       ├─ Location: VarSpawnLoc
│   │       ├─ Rotation: Get Control Rotation
│   │       └─ Scale: [1,1,1]
│   ├─ Collision Handling: Always Spawn, Ignore Collisions
│   ├─ Owner: Self
│   └─> Return Value → VarHook
│
└─> Branch (Is Valid: VarHook)
    ├─ Valid:
    │   ├─> Print String ("HOOK SPAWNED!")
    │   └─> Promote to Variable: CurrentGrappleHook
    └─ Invalid:
        └─> Print String ("HOOK SPAWN FAILED!")
```

---

## Test Right Now

1. **Open BP_NinjaWizardCharacter**
2. **Find Spawn Actor node**
3. **Click it**
4. **Details panel → Collision Handling Override**
5. **Change to "Always Spawn, Ignore Collisions"**
6. **Compile**
7. **PIE**
8. **Press E**

**You should see the hook spawn 100 units in front of you!**

---

Let me know if you still see "hello" but nothing spawns after changing the Collision Handling! If that doesn't fix it, the issue is in BP_GrappleHook itself.
