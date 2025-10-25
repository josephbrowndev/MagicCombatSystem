# Attribute and Summon System - Blueprint Setup Guide

This guide provides step-by-step instructions for implementing the player attribute system (STR, WIS, PER, AGI) and the enemy/ally summon system in Unreal Engine 5 Blueprints.

## Table of Contents
1. [Player Attribute Setup](#player-attribute-setup)
2. [Creating Enemy Blueprints](#creating-enemy-blueprints)
3. [Soul Bonding Implementation](#soul-bonding-implementation)
4. [Summon Management UI](#summon-management-ui)
5. [Attribute UI Display](#attribute-ui-display)
6. [Testing the Systems](#testing-the-systems)
7. [Example Configurations](#example-configurations)

---

## Player Attribute Setup

### Step 1: Configure Starting Attributes in BP_NinjaWizardCharacter

1. Open `BP_NinjaWizardCharacter` in the Blueprint Editor
2. Select the **AttributeComponent** in the Components panel
3. In the Details panel, find the **Attributes** category
4. Set starting values for each attribute:

   **Strength (STR):**
   - Base Value: `10` (default starting strength)
   - Bonus Value: `0` (will be modified by equipment)
   - Multiplier: `1.0` (will be modified by buffs)

   **Wisdom (WIS):**
   - Base Value: `15` (higher starting wisdom for spellcasters)
   - Bonus Value: `0`
   - Multiplier: `1.0`

   **Perception (PER):**
   - Base Value: `10`
   - Bonus Value: `0`
   - Multiplier: `1.0`

   **Agility (AGI):**
   - Base Value: `12` (slightly higher for ninja archetype)
   - Bonus Value: `0`
   - Multiplier: `1.0`

### Step 2: Create Attribute Increase System

Create a function in BP_NinjaWizardCharacter to allow players to increase attributes when leveling up.

1. Create a new function called `IncreasePlayerAttribute`
2. Add inputs:
   - `Attribute` (EPlayerAttribute enum)
   - `Amount` (Integer, default = 1)

3. Add nodes:
   ```
   [Attribute Component] -> [Increase Attribute]
      Input: Attribute (from function input)
      Input: Amount (from function input)

   [Attribute Component] -> [Recalculate Derived Stats]

   [Attribute Component] -> [Apply Attribute Effects]
   ```

### Step 3: Create Level-Up Attribute Selection UI

Create a widget blueprint called `WBP_AttributeSelection`:

**Widget Hierarchy:**
```
Canvas Panel
├── Background Image
├── Title Text ("Choose Attribute to Increase")
├── Attribute Buttons Container (Vertical Box)
│   ├── Button_Strength
│   │   ├── Text: "Strength (+1)"
│   │   └── Text: "Current: [StrengthValue]"
│   ├── Button_Wisdom
│   │   ├── Text: "Wisdom (+1)"
│   │   └── Text: "Current: [WisdomValue]"
│   ├── Button_Perception
│   │   ├── Text: "Perception (+1)"
│   │   └── Text: "Current: [PerceptionValue]"
│   └── Button_Agility
│       ├── Text: "Agility (+1)"
│       └── Text: "Current: [AgilityValue]"
└── Stats Preview Panel
    └── Derived Stats Display
```

**Button Click Implementation (Example for Strength):**

1. Select `Button_Strength` and scroll to Events
2. Add **On Clicked** event
3. Add these nodes:

```
On Clicked
   |
   V
[Get Player Character]
   |
   V
[Cast to NinjaWizardCharacter]
   |
   V
[Increase Player Attribute]
   Input: Attribute = Strength
   Input: Amount = 1
   |
   V
[Remove from Parent] (close the UI)
```

Repeat for all four attribute buttons.

### Step 4: Display Derived Stats

Create a widget blueprint called `WBP_AttributeDisplay`:

**Widget Hierarchy:**
```
Canvas Panel
├── Attributes Panel
│   ├── STR: [10] -> Block: 5%, Weapon Dmg: +20%
│   ├── WIS: [15] -> Magic Dmg: +45%, Summons: 2
│   ├── PER: [10] -> Crit: 10%, Slow-Mo: 10%
│   └── AGI: [12] -> Dodge: 9.6%, Move Speed: +18%
└── Detailed Stats Panel (expandable)
```

**Blueprint Implementation:**

1. Create a function `UpdateAttributeDisplay`
2. Get reference to PlayerAttributeComponent
3. Read all attribute values:

```
[Get Player Character]
   |
   V
[Cast to NinjaWizardCharacter]
   |
   V
[Get Attribute Component]
   |
   +---> [Get Attribute Value] (Strength)
   +---> [Get Attribute Value] (Wisdom)
   +---> [Get Attribute Value] (Perception)
   +---> [Get Attribute Value] (Agility)
   |
   V
[Get Derived Stats]
   |
   V
[Format Text] -> Display in UI
```

4. Call `UpdateAttributeDisplay` on Event Construct
5. Bind to `OnAttributeChanged` event from AttributeComponent for real-time updates

---

## Creating Enemy Blueprints

### Step 1: Create Base Enemy Blueprint

1. Right-click in Content Browser -> Blueprint Class
2. Search for and select `CombatEntity` as parent class
3. Name it `BP_EnemyBase`

### Step 2: Configure Enemy Properties

Open BP_EnemyBase and set default values:

**Entity Properties:**
- Entity Name: `"Base Enemy"`
- Rank: `Common`
- Level: `1`
- Element Type: `Fire` (or appropriate element)
- Threat Level: `Aggressive`
- Is Summonable Ally: `false` (will be set to true on soul bond)

**Combat Stats:**
- Max Health: `100.0`
- Base Damage: `10.0`
- Attack Speed: `1.0`
- Movement Speed: `300.0`
- Defense: `5.0`

**Summon Properties:**
- Summon Cost: `25` (mana required to summon)
- Summon Capacity Usage: `1` (capacity required)

### Step 3: Create Specific Enemy Types

Create child blueprints for each element:

**BP_FireElemental (Child of BP_EnemyBase):**
- Entity Name: `"Fire Elemental"`
- Element Type: `Fire`
- Max Health: `80.0`
- Base Damage: `15.0` (higher damage, lower health)
- Add fire visual effects

**BP_WaterElemental (Child of BP_EnemyBase):**
- Entity Name: `"Water Elemental"`
- Element Type: `Water`
- Max Health: `120.0` (tankier)
- Base Damage: `8.0`
- Add water visual effects

**BP_EarthTitan (Child of BP_EnemyBase):**
- Entity Name: `"Earth Titan"`
- Rank: `Soldier` (starts at higher rank)
- Element Type: `Earth`
- Max Health: `200.0`
- Base Damage: `12.0`
- Defense: `15.0`
- Add earth/rock visual effects

### Step 4: Create Boss/Unique Enemies

**BP_LightningDragon (Child of BP_EnemyBase):**
- Entity Name: `"Lightning Dragon"`
- Rank: `Unique`
- Level: `10`
- Element Type: `Lightning`
- Threat Level: `Boss`
- Max Health: `500.0` (will be 2000 with Unique 4x multiplier)
- Base Damage: `30.0` (will be 120 with multiplier)
- Summon Capacity Usage: `5`

### Step 5: Add Special Abilities to Enemies

In each enemy blueprint:

1. Find **Special Abilities** array in Details panel
2. Add array elements (FSpellData)
3. Configure each ability:

**Example - Fire Elemental Special Ability:**
- Spell Name: `"Fireball"`
- Element: `Fire`
- Combined Element: `None`
- Mana Cost: `0` (enemies don't use mana)
- Mastery Required: `0`

**Example - Lightning Dragon Special Ability:**
- Spell Name: `"Chain Lightning"`
- Element: `Lightning`
- Combined Element: `Storm` (Lightning + Air)
- Description: `"Strikes multiple targets with arcing lightning"`

### Step 6: Configure Challenges for Rank-Ups

For each enemy that can be bonded, add challenges to the **Available Challenges** array:

**Common -> Soldier Challenge:**
1. Add array element to Available Challenges
2. Configure:
   - Challenge Type: `Kill Multiple Enemies`
   - Challenge Name: `"Prove Your Worth"`
   - Challenge Description: `"Defeat 10 enemies"`
   - Target Count: `10`
   - Time Limit: `0.0` (no time limit)
   - Required Rank To Unlock: `Common`
   - Reward Rank: `Soldier`

**Soldier -> Captain Challenge:**
- Challenge Type: `Kill Within Time Limit`
- Challenge Name: `"Speed Trial"`
- Challenge Description: `"Defeat 5 enemies in 60 seconds"`
- Target Count: `5`
- Time Limit: `60.0`
- Required Rank To Unlock: `Soldier`
- Reward Rank: `Captain`

**Captain -> General Challenge:**
- Challenge Type: `Defeat Elite Enemy`
- Challenge Name: `"Elite Hunter"`
- Challenge Description: `"Defeat a Captain-rank enemy or higher"`
- Required Rank To Unlock: `Captain`
- Reward Rank: `General`

---

## Soul Bonding Implementation

### Step 1: Detect Enemy Death

In BP_NinjaWizardCharacter, create a function called `OnEnemyDefeated`:

**Function Inputs:**
- `DefeatedEnemy` (CombatEntity reference)

**Function Logic:**
```
Event OnEnemyDefeated
   |
   V
[DefeatedEnemy] -> [Can Be Bonded]
   |
   V
[Branch] (if Can Be Bonded = true)
   |
   V
[Get Summon Manager]
   |
   V
[Try Bond With Entity]
   Input: Entity = DefeatedEnemy
   Output: Success (boolean)
   |
   V
[Branch] (if Success = true)
   |
   V
[Show Soul Bond Notification]
   Text: "Soul bonded with [Enemy Name]!"
   |
   V
[Play Soul Bond Effect] (visual/audio feedback)
```

### Step 2: Call Soul Bonding from Enemy Death

In each enemy blueprint (or BP_EnemyBase):

1. Override the `Die` function (from CombatEntity)
2. Add this logic:

```
Event Die
   |
   V
[Get Last Damage Dealer]
   |
   V
[Cast to NinjaWizardCharacter]
   |
   V
[Branch] (if cast successful)
   |
   V
[Call OnEnemyDefeated]
   Input: DefeatedEnemy = Self
   |
   V
[Parent: Die] (call parent implementation)
   |
   V
[Spawn Death Effects]
   |
   V
[Destroy Actor] (after delay)
```

### Step 3: Create Soul Bond Visual Effect

Create a widget blueprint `WBP_SoulBondNotification`:

**Widget Hierarchy:**
```
Canvas Panel
├── Background (animated glow)
├── Icon (soul/spirit icon)
├── Enemy Name Text
├── Element Icon
└── Rank Badge
```

**Animation:**
1. Create animation called `FadeInOut`
2. Animate from bottom to top with fade in/out
3. Duration: 3 seconds

**Blueprint:**
```
Event Construct
   |
   V
[Play Animation] -> FadeInOut
   |
   V
[Delay] 3.0 seconds
   |
   V
[Remove from Parent]
```

### Step 4: Soul Bonding Limitations

Add logic to prevent bonding the same enemy class multiple times:

```
[Try Bond With Entity]
   |
   V
[Branch] (check if already bonded)
   If True:
      [Show Message] "Already bonded with this creature type"
      [Return]
   If False:
      [Continue with bonding]
```

---

## Summon Management UI

### Step 1: Create Summon Collection Widget

Create widget blueprint `WBP_SummonCollection`:

**Widget Hierarchy:**
```
Canvas Panel
├── Header
│   ├── Title: "Summon Collection"
│   ├── Capacity Display: "[Used]/[Max] Capacity"
│   └── Active Summons: "[Count]/[MaxCount]"
├── Filter Buttons (Horizontal Box)
│   ├── All
│   ├── By Element (Fire, Water, Earth, etc.)
│   └── By Rank (Common, Soldier, etc.)
├── Summon List (Scroll Box)
│   └── WBP_SummonListItem (repeated for each summon)
└── Footer
    └── Close Button
```

### Step 2: Create Summon List Item Widget

Create widget blueprint `WBP_SummonListItem`:

**Widget Hierarchy:**
```
Horizontal Box
├── Element Icon
├── Info Panel (Vertical Box)
│   ├── Name Text
│   ├── Rank and Level: "[Rank] Lv. [Level]"
│   ├── Stats: "HP: [Max] | DMG: [Base]"
│   └── Progress: "Kills: [Count] | XP: [Current]/[ToNext]"
├── Summon Button
│   ├── Text: "Summon"
│   └── Mana Cost: "[Cost] Mana"
└── Challenges Button
    └── Text: "View Challenges"
```

### Step 3: Populate Summon List

In WBP_SummonCollection Event Graph:

```
Event Construct
   |
   V
[Get Player Character]
   |
   V
[Cast to NinjaWizardCharacter]
   |
   V
[Get Summon Manager]
   |
   V
[Get All Collected Summons]
   Output: Array of FStoredSummon
   |
   V
[ForEachLoop]
   |
   V
[Create Widget] WBP_SummonListItem
   |
   V
[Initialize Summon Item]
   Input: Summon Data (from array)
   |
   V
[Add Child to Scroll Box]
```

### Step 4: Implement Summon Button

In WBP_SummonListItem, for the Summon Button On Clicked event:

```
On Clicked
   |
   V
[Get Player Character]
   |
   V
[Cast to NinjaWizardCharacter]
   |
   V
[Check Mana] (CurrentMana >= SummonCost)
   |
   V
[Branch]
   If False:
      [Show Message] "Not enough mana!"
      [Return]
   If True:
      [Continue]
   |
   V
[Get Summon Manager]
   |
   V
[Can Summon] (check capacity and count)
   |
   V
[Branch]
   If False:
      [Show Message] "Summon capacity full!"
      [Return]
   If True:
      [Summon Entity]
         Input: Summon Class
         Input: Location (near player)
   |
   V
[Consume Mana]
   Amount: Summon Cost
   |
   V
[Close UI]
```

### Step 5: Display Summon Capacity

Create a function `UpdateCapacityDisplay`:

```
[Get Player Character]
   |
   V
[Get Attribute Component]
   |
   V
[Get Summon Capacity]
   Output: Max Capacity
   |
   V
[Get Summon Manager]
   |
   V
[Get Current Capacity Used]
   Output: Used Capacity
   |
   V
[Format Text] "{0}/{1} Capacity"
   Param 0: Used Capacity
   Param 1: Max Capacity
   |
   V
[Set Text] -> Capacity Display
```

### Step 6: Active Summon Display

Create widget `WBP_ActiveSummons` for the HUD:

**Widget Hierarchy:**
```
Vertical Box
├── WBP_ActiveSummonIcon (repeated for each active summon)
│   ├── Element Icon
│   ├── Health Bar
│   ├── Level Badge
│   └── Dismiss Button (X)
```

**Update Logic:**
```
Event Tick
   |
   V
[Get Summon Manager]
   |
   V
[Get Active Summons]
   Output: Array of CombatEntity references
   |
   V
[Clear Children] (from Vertical Box)
   |
   V
[ForEachLoop]
   |
   V
[Create Widget] WBP_ActiveSummonIcon
   |
   V
[Update Health Bar]
   Progress: CurrentHealth / MaxHealth
   |
   V
[Add Child]
```

### Step 7: Dismiss Summon

In WBP_ActiveSummonIcon, for Dismiss Button:

```
On Clicked
   |
   V
[Get Summon Reference]
   |
   V
[Get Summon Manager]
   |
   V
[Dismiss Summon]
   Input: Summon Entity
   |
   V
[Remove from Parent]
```

---

## Attribute UI Display

### Step 1: Create Main Attribute Widget

Create widget blueprint `WBP_PlayerAttributes`:

**Widget Hierarchy:**
```
Canvas Panel
├── Compact View (default)
│   ├── STR: 10
│   ├── WIS: 15
│   ├── PER: 10
│   └── AGI: 12
└── Detailed View (on hover/click)
    ├── Strength Panel
    │   ├── Value: 10 (+0) x1.0 = 10
    │   ├── Block Chance: 5.0%
    │   ├── Weapon Damage: +20%
    │   ├── Max Stamina: +50
    │   └── Max Health: +100
    ├── Wisdom Panel
    │   ├── Value: 15 (+0) x1.0 = 15
    │   ├── Magic Damage: +45%
    │   ├── Spell Size: +30%
    │   ├── Max Summons: 2
    │   ├── Summon Capacity: 1
    │   └── Max Mana: +120
    ├── Perception Panel
    │   ├── Value: 10 (+0) x1.0 = 10
    │   ├── Danger Sense: 500 units
    │   ├── Slow-Motion: 10% chance
    │   ├── Critical Hit: 10%
    │   └── Threat Detection: 500 units
    └── Agility Panel
        ├── Value: 12 (+0) x1.0 = 12
        ├── Attack Speed: +24%
        ├── Movement Speed: +18%
        ├── Dodge Chance: 9.6%
        └── Jump Height: +12%
```

### Step 2: Create Stat Tooltip Widget

Create `WBP_StatTooltip` to show detailed information on hover:

**Example for Strength:**
```
Title: "Strength (STR)"
Description: "Physical power and endurance"

Effects per point:
• Block Chance: +0.5% (max 50%)
• Weapon Damage: +2%
• Max Stamina: +5
• Max Health: +10
• Carry Weight: +5
• Knockback Resist: +1%

Current Total: 10
  Base: 10
  Bonus: 0
  Multiplier: x1.0
```

### Step 3: Real-Time Stat Updates

Add Event Binding in WBP_PlayerAttributes:

```
Event Construct
   |
   V
[Get Player Character]
   |
   V
[Get Attribute Component]
   |
   V
[Bind Event to OnAttributeChanged]
   |
   V
[Create Custom Event] "OnAttributeUpdated"
   |
   V
[Update All Displays]
```

### Step 4: Attribute Comparison (Before/After)

When hovering over attribute increase buttons, show comparison:

```
Widget: "Preview Stats"

Current -> After +1 STR
━━━━━━━━━━━━━━━━━━━━━
Strength: 10 -> 11
Block Chance: 5% -> 5.5%
Weapon Damage: +20% -> +22%
Max Stamina: 100 -> 105
Max Health: 200 -> 210
```

**Implementation:**
```
On Button Hovered
   |
   V
[Get Current Attribute Value]
   |
   V
[Calculate Derived Stats] (current)
   |
   V
[Calculate Derived Stats] (current + 1)
   |
   V
[Format Comparison Text]
   |
   V
[Show Tooltip]
```

---

## Testing the Systems

### Test 1: Attribute Stat Calculations

1. Open BP_NinjaWizardCharacter
2. Play in Editor (PIE)
3. Open Debug Console (` key)
4. Type: `showdebug attributes` (if you implement debug display)
5. Verify base attributes are correct
6. Call `IncreasePlayerAttribute` with each attribute
7. Verify derived stats update correctly

**Expected Results:**
- STR 10->11: Block +0.5%, Weapon Dmg +2%, Stamina +5, Health +10
- WIS 15->16: Magic Dmg +3%, Mana +8
- PER 10->11: Crit +1%, Slow-motion +1%
- AGI 12->13: Move speed +1.5%, Dodge +0.8%

### Test 2: Soul Bonding

1. Place a test enemy in the level (BP_FireElemental)
2. Set enemy health to low value for quick testing
3. Defeat the enemy
4. Verify soul bond notification appears
5. Open summon collection UI
6. Verify Fire Elemental appears in list

**Expected Results:**
- Enemy dies
- "Soul bonded with Fire Elemental!" message
- Enemy added to CollectedSummons array
- Can be seen in summon UI

### Test 3: Summoning System

1. Bond with an enemy (as in Test 2)
2. Open summon collection UI
3. Click "Summon" on the bonded enemy
4. Verify mana cost is deducted
5. Verify summon appears near player
6. Check active summon count increases

**Expected Mana Costs:**
- Common Lv1: 25 mana
- Common Lv5: 87 mana
- Soldier Lv1: 50 mana
- Captain Lv1: 75 mana

### Test 4: Summon Capacity

1. Set player Wisdom to 25 (Capacity = 2, Max Count = 2)
2. Bond with 2 Common enemies
3. Summon both enemies
4. Try to summon a third
5. Verify rejection message

**Expected Results:**
- Can summon 2 Common summons (2 capacity used)
- Cannot summon more (capacity full)
- Message: "Summon capacity full!"

### Test 5: Summon Rank Challenges

1. Summon a Common-rank enemy
2. Use it to kill 10 enemies
3. Verify challenge progress updates
4. Complete the challenge
5. Verify summon ranks up to Soldier

**Expected Results:**
- Kill count increases each kill
- Challenge progress: 1/10, 2/10, etc.
- At 10/10, challenge completes
- Summon becomes Soldier rank
- Stats multiply by 1.5x

### Test 6: Perception Slow-Motion

1. Set Perception to 50 (50% chance)
2. Place several aggressive enemies in level
3. Walk near them to trigger danger sense
4. Observe slow-motion effect triggering

**Expected Results:**
- ~50% chance when enemies detected
- Time slows to 0.3 speed
- Duration: 1.0 + (50 * 0.1) = 6 seconds
- Time returns to normal after duration

### Test 7: Wisdom-Based Summon Limits

**Test Case 1: Low Wisdom (15)**
- Max Summon Count: 1 + (15/15) = 2
- Summon Capacity: 15/10 = 1
- Can summon: 1 Common OR cannot summon Soldier (needs 2 capacity)

**Test Case 2: Medium Wisdom (50)**
- Max Summon Count: 1 + (50/15) = 4
- Summon Capacity: 50/10 = 5
- Can summon: 5 Common OR 2 Soldier + 1 Common OR 1 Captain + 2 Common

**Test Case 3: High Wisdom (100)**
- Max Summon Count: 1 + (100/15) = 7
- Summon Capacity: 100/10 = 10
- Can summon: 10 Common OR 5 Soldier OR 2 General + 2 Common

---

## Example Configurations

### Example 1: Tank Build (High Strength)

**Attributes:**
- STR: 50
- WIS: 10
- PER: 10
- AGI: 10

**Derived Stats:**
- Block Chance: 25% (50 * 0.5)
- Weapon Damage: +100% (50 * 2%)
- Max Stamina: +250 (50 * 5)
- Max Health: +500 (50 * 10)
- Knockback Resistance: 50% (50 * 1%)

**Playstyle:**
- High survivability
- Strong melee damage
- Can block often
- Low magic power
- Only 1-2 summons

### Example 2: Summoner Build (High Wisdom)

**Attributes:**
- STR: 10
- WIS: 75
- PER: 10
- AGI: 10

**Derived Stats:**
- Magic Damage: +225% (75 * 3%)
- Spell Size: +150% (75 * 2%)
- Max Summons: 6 (1 + 75/15)
- Summon Capacity: 7 (75/10)
- Max Mana: +600 (75 * 8)

**Possible Summon Combinations:**
- 7 Common summons
- 3 Soldier + 1 Common
- 2 Captain + 1 Common
- 1 General + 3 Common
- 1 General + 1 Soldier + 1 Common

**Playstyle:**
- Army of summons
- Powerful magic
- Large spell AoE
- Weak physical defense

### Example 3: Assassin Build (High Perception + Agility)

**Attributes:**
- STR: 10
- WIS: 10
- PER: 40
- AGI: 40

**Derived Stats (Perception):**
- Danger Sense: 2000 units (40 * 50)
- Slow-Motion: 40% chance
- Slow-Motion Duration: 5 seconds (1 + 40*0.1)
- Critical Hit: 40%

**Derived Stats (Agility):**
- Attack Speed: +80% (40 * 2%)
- Movement Speed: +60% (40 * 1.5%)
- Dodge Chance: 32% (40 * 0.8%)
- Jump Height: +40%

**Playstyle:**
- Frequent slow-motion
- High mobility
- High dodge and crit
- Fast attacks
- Low survivability if hit

### Example 4: Balanced Build

**Attributes:**
- STR: 20
- WIS: 20
- PER: 20
- AGI: 20

**Key Stats:**
- Block: 10%, Dodge: 16%, Crit: 20%
- Weapon Damage: +40%, Magic Damage: +60%
- Max Summons: 2, Capacity: 2
- Attack Speed: +40%, Movement: +30%
- Slow-Motion: 20% chance

**Playstyle:**
- Jack-of-all-trades
- Can adapt to situations
- Moderate in all areas
- Good for learning the game

### Example 5: Elite Enemy Configuration

**BP_ShadowAssassin (Unique Rank Boss):**

**Properties:**
- Entity Name: "Shadow Assassin"
- Rank: Unique
- Level: 15
- Element Type: Dark
- Threat Level: Boss

**Base Combat Stats (before multipliers):**
- Max Health: 400
- Base Damage: 50
- Attack Speed: 1.5
- Movement Speed: 600
- Defense: 20

**Actual Stats (with 4x Unique multiplier):**
- Max Health: 1600
- Base Damage: 200
- Defense: 80

**Special Abilities:**
1. "Shadow Step" (Dark)
   - Teleport behind player
2. "Void Strike" (Dark + Curse)
   - High damage + poison DoT
3. "Assassinate" (Dark)
   - Instant kill if player below 20% health

**Summon Properties:**
- Summon Cost: 400 mana (25 * 5 * 15 * 0.5)
- Summon Capacity Usage: 5

**Challenges:**
1. Common -> Soldier: Kill 50 enemies
2. Soldier -> Captain: Kill 10 enemies in 120 seconds
3. Captain -> General: Defeat a boss-tier enemy
4. General -> Unique: Protect player for 300 seconds without taking damage

---

## Advanced: Custom Challenge System

### Creating a Custom Challenge in Blueprint

1. In BP_CombatEntity, create function `CreateCustomChallenge`

**Inputs:**
- Challenge Type (ESummonChallengeType)
- Challenge Name (Text)
- Description (Text)
- Required Count (Integer)
- Time Limit (Float)
- Reward Rank (EEntityRank)

**Implementation:**
```
[Make FSummonChallenge]
   Challenge Type: from input
   Challenge Name: from input
   Challenge Description: from input
   Required Count: from input
   Time Limit: from input
   Required Rank To Unlock: Current Rank
   Reward Rank: from input
   Current Progress: 0
   Completed: false
   |
   V
[Add to Available Challenges array]
```

### Tracking Custom Challenge Progress

Create a function `UpdateCustomChallenge`:

```
Event OnKill
   |
   V
[ForEachLoop] (iterate Available Challenges)
   |
   V
[Switch on Challenge Type]
   |
   +-> Kill Multiple Enemies:
   |      [Increment Current Progress]
   |      [Check if >= Required Count]
   |      [If true: Mark Completed]
   |
   +-> Kill Within Time Limit:
   |      [Check if Timer Active]
   |      [Increment Current Progress]
   |      [Check completion]
   |
   +-> Use Special Ability:
   |      (handled in OnAbilityUsed event)
   |
   +-> Deal Damage Threshold:
          [Track Total Damage Dealt]
          [Update Progress]
```

---

## Tips and Best Practices

### Attribute System:
1. **Start Balanced**: Begin with 10-15 in each attribute for new players
2. **Attribute Points**: Give 3-5 points per level up
3. **Soft Caps**: Consider adding diminishing returns after certain thresholds (e.g., block capped at 50%)
4. **Respec Option**: Allow players to reset attributes for a cost

### Summon System:
1. **Clear Visual Feedback**: Use different colored outlines for different ranks
2. **Summon UI Shortcuts**: Add hotkeys for quick summon access
3. **Auto-Dismiss**: Dismiss summons when out of mana
4. **Summon AI**: Make sure summon AI follows player and attacks enemies
5. **Death Handling**: Save summon XP/level when dismissed, even if they die

### Soul Bonding:
1. **Bonding Animation**: Add visual effect (soul leaving body, flying to player)
2. **Collection Progress**: Show "10/50 enemy types bonded" tracker
3. **Rare Enemies**: Make unique enemies harder to find for rewarding bonding
4. **First Bond Tutorial**: Explain the system when player bonds first enemy

### Challenge System:
1. **Clear Progress**: Show challenge progress in summon UI
2. **Notifications**: Alert player when challenge completes
3. **Reward Ceremony**: Play animation/effect when summon ranks up
4. **Challenge Variety**: Mix easy and hard challenges for each rank

### Performance:
1. **Summon Limit**: Hard cap active summons (even with high wisdom) to prevent lag
2. **Despawn Inactive**: Despawn summons if player gets too far away
3. **LOD for Summons**: Use simpler models/AI when summons are far from camera
4. **Pool Summons**: Use object pooling for frequently summoned entities

---

## Troubleshooting

### Issue: Attributes not updating stats
**Solution:**
- Make sure `RecalculateDerivedStats()` is called after attribute changes
- Verify `ApplyAttributeEffects()` is called to apply to character
- Check that AttributeComponent is properly initialized in BeginPlay

### Issue: Can't summon enemies
**Solution:**
- Check mana is sufficient (cost = 25 * (Rank+1) * (Level*0.5))
- Verify summon capacity isn't full
- Check max summon count hasn't been reached
- Ensure the entity is marked as bonded (`bIsSummonableAlly = true`)

### Issue: Slow-motion not triggering
**Solution:**
- Verify Perception is high enough (10 PER = 10% chance)
- Check danger sense range covers enemy distance
- Make sure `CanDetectThreat()` is being called when enemies approach
- Verify time dilation is being set (check with `slomo` console command)

### Issue: Soul bonding not working
**Solution:**
- Ensure enemy is actually dead (`IsAlive() == false`)
- Check `CanBeBonded()` returns true
- Verify you haven't already bonded with this exact instance
- Make sure the enemy class is marked as bondable

### Issue: Challenge progress not updating
**Solution:**
- Verify `UpdateChallengeProgress()` is being called
- Check challenge type matches the action (kills for kill challenges, etc.)
- Ensure challenge isn't already completed
- Verify the summon is active when progress should be tracked

---

## Next Steps

After implementing this system:

1. **Create Enemy Variety**: Design 20+ enemy types across all elements
2. **Balance Stats**: Playtest and adjust attribute scaling
3. **Add Visual Effects**: Create particle effects for soul bonding, summons, slow-motion
4. **Summon AI**: Implement follow behavior, combat AI, and special abilities
5. **Save System**: Save bonded summons, levels, and challenges to player save file
6. **Achievements**: Add achievements for bonding all enemies, reaching max rank summons, etc.

Refer to `DESIGN.md` for overall system architecture and `IMPLEMENTATION_GUIDE.md` for content creation guidelines.
