# AI Behavior and Combat System - Complete Guide

This guide covers the complete AI behavior and combat systems for your open-world ninja wizard game.

## Table of Contents
1. [System Overview](#system-overview)
2. [Mob Behavior Types](#mob-behavior-types)
3. [Combat AI Roles](#combat-ai-roles)
4. [Safe Zone System](#safe-zone-system)
5. [Blueprint Setup Guide](#blueprint-setup-guide)
6. [Example Configurations](#example-configurations)
7. [Advanced Features](#advanced-features)

---

## System Overview

The AI system is built with two main components:

- **AIBehaviorComponent**: Controls mob behavior (wandering, fleeing, guarding, chasing)
- **CombatAIComponent**: Controls combat behavior (attacks, combos, spells, boss patterns)

Both components attach to `CombatEntity` (your enemy base class) and work together to create dynamic, challenging enemies.

---

## Mob Behavior Types

### 1. Passive Mobs

**Use Case**: Peaceful animals, critters, ambient wildlife

**Behavior:**
- Wander randomly within a radius
- Look for and eat vegetation
- Rest periodically
- Flee from threats (optional)
- Never initiate combat

**Configuration:**
```
Behavior Type: Passive

Passive Settings:
- Wander Radius: 1000.0 (how far from spawn they roam)
- Wander Interval: 5.0 (seconds between wanders)
- Eating Interval: 30.0 (seconds between eating)
- Resting Interval: 60.0 (seconds between resting)
- Can Flee: true
- Flee Distance: 500.0 (flee when player within this distance)
- Preferred Vegetation Tags: ["Grass", "Flowers", "Berries"]
```

**Example Enemies:**
- Deer, rabbits, birds
- Peaceful slimes
- Butterflies, fireflies
- Grazing creatures

**Blueprint Setup:**
1. Create enemy blueprint (child of BP_CombatEntity)
2. Add AIBehaviorComponent
3. Set Behavior Type = Passive
4. Configure Passive Settings
5. Optional: Tag vegetation actors with preferred tags

---

### 2. Chasing Mobs (Catchable Creatures)

**Use Case**: Collectible creatures that reward special items when caught

**Behavior:**
- Detect player within radius
- Flee at high speed when detected
- Stamina drains while fleeing
- Stop when exhausted
- Can be caught when player gets close enough
- Drop reward item when caught
- Creature is removed from world after being caught

**Configuration:**
```
Behavior Type: Chasing

Chasing Settings:
- Flee Speed: 600.0 (faster than player walk speed)
- Detection Radius: 800.0 (when they notice player)
- Catch Radius: 100.0 (how close player must get)
- Stamina Drain Rate: 1.0 (stamina per second)
- Max Stamina: 100.0
- Reward Item Class: BP_SpecialGem
- Reward Quantity: 1
```

**Example Enemies:**
- Magical wisps
- Rare butterflies
- Spirit animals
- Treasure goblins
- Elemental sprites

**Blueprint Setup:**
1. Create catchable creature blueprint
2. Set Behavior Type = Chasing
3. Configure flee speed faster than player
4. Set reward item (gem, crafting material, etc.)
5. Adjust stamina for difficulty:
   - High stamina = harder to catch
   - Low stamina = easier to catch

**Gameplay Loop:**
- Player spots rare creature
- Creature flees when player approaches
- Player must chase until creature exhausts
- Catch when close enough
- Receive rare reward

---

### 3. Neutral Mobs

**Use Case**: Creatures that ignore player unless provoked

**Behavior:**
- Wander peacefully alone or in groups
- Completely ignore player by default
- **Only attack when attacked first**
- Call nearby allies for help when attacked
- Group members stay together (cohesion)
- Return to peaceful wandering after combat

**Configuration:**
```
Behavior Type: Neutral

Neutral Settings:
- Group Type: MediumPack (Solo, SmallPack 2-4, MediumPack 5-10, LargeHerd 10+)
- Min Group Size: 3
- Max Group Size: 7
- Group Cohesion Radius: 300.0 (stay within this distance)
- Aggro Radius: 0.0 (0 = only when attacked)
- Call For Help: true
- Help Call Radius: 500.0 (how far allies respond)
```

**Example Enemies:**
- Wild boars (peaceful until attacked)
- Giant ants (small packs)
- Wolves (medium packs)
- Bison herds (large herds)
- Forest guardians

**Blueprint Setup:**
1. Create neutral enemy blueprint
2. Set Behavior Type = Neutral
3. Choose group type (solo or pack size)
4. Enable call for help for pack behavior
5. Spawn multiple of same type nearby for groups

**Important:**
- **Never attack player first**
- Only become hostile when player attacks them
- Great for peaceful exploration
- Punishes careless players
- Rewards skilled combat

---

### 4. Area Guard Mobs (Territorial + Anti-Cheese)

**Use Case**: Guard specific locations, prevent ranged exploits

**Behavior:**
- Guard a specific territory (circle area)
- Hostile to players within aggro radius
- Will not leave territory (returns if pulled out)
- **Anti-Cheese System**:
  - Detects if player attacks from outside territory
  - Spawns 5 punishment mobs near player
  - Punishment mobs are 2x player strength
  - Prevents safe ranged attacks
- Drop unique weapons and armor on death

**Configuration:**
```
Behavior Type: Area Guard

Area Guard Settings:
- Territory Center: (auto-set to spawn location or manual)
- Territory Radius: 1500.0 (area they protect)
- Aggro Radius: 800.0 (when they engage)
- Enable Anti-Cheese: true
- Anti-Cheese Detection Radius: 1200.0
- Punishment Mob Count: 5
- Punishment Mob Strength Multiplier: 2.0
- Punishment Mob Class: BP_SkeletonWarrior
- Unique Weapon Drops: [BP_LegendarySword, BP_MysticStaff]
- Unique Armor Drops: [BP_DragonHelm, BP_ShadowCloak]
```

**Example Enemies:**
- Dungeon guardians
- Treasure room protectors
- Boss arena guards
- Fort defenders
- Sacred grove protectors

**Blueprint Setup:**
1. Create area guard enemy
2. Set Behavior Type = AreaGuard
3. Place in level at desired location (becomes territory center)
4. Set territory radius to cover area
5. Enable anti-cheese
6. Set punishment mob blueprint
7. Add unique loot drops
8. Multiple guards can share same territory

**Anti-Cheese System Details:**
```
Player tries to attack from safety:
1. Guard detects damage from outside territory
2. System spawns 5 punishment mobs near player
3. Punishment mobs have 2x stats
4. Player must fight or flee
5. Discourages exploit gameplay
6. Rewards skilled melee combat
```

**Loot System:**
- Always drop unique items (not random)
- Each guard drops all configured items
- Encourages defeating guards properly
- Rewards brave players

---

### 5. Aggressive Mobs (Zone Bosses)

**Use Case**: Zone bosses, dangerous enemies, relentless hunters

**Behavior:**
- Detect players in large radius
- **Chase indefinitely** (or until max range)
- **Respect safe zones**:
  - Stop at safe zone boundary
  - Stare menacingly at player
  - Wait for player to exit
  - Resume chase immediately
- Never give up pursuit
- Boss-specific attack patterns

**Configuration:**
```
Behavior Type: Aggressive

Aggressive Settings:
- Detection Radius: 2000.0 (very large)
- Chase Speed: 700.0 (faster than player)
- Chase Indefinitely: true
- Respect Safe Zones: true
- Safe Zone Stare Distance: 200.0
- Is Boss: true
- Boss Attack Patterns: [See Boss Combat section]
```

**Example Enemies:**
- Zone bosses
- Mini-bosses
- Roaming elite enemies
- Dungeon hunters
- Cursed knights
- Ancient guardians

**Blueprint Setup:**
1. Create aggressive enemy blueprint
2. Set Behavior Type = Aggressive
3. Enable chase indefinitely
4. Enable respect safe zones
5. Set high chase speed
6. Configure as boss if needed
7. Add boss attack patterns

**Safe Zone Interaction:**
```
Boss chasing player:
1. Player runs to safe zone
2. Boss stops at boundary (won't enter)
3. Boss stares at player
4. Boss rotates to face player
5. Boss waits at edge
6. Player exits safe zone
7. Boss immediately resumes chase
8. Creates tense "you can't hide forever" moments
```

**Gameplay Design:**
- Creates memorable chase sequences
- Players must fight or use safe zones wisely
- Safe zones are temporary refuge, not escape
- Builds tension and fear
- Rewards skilled players who fight

---

## Combat AI Roles

All combat is handled by `CombatAIComponent` which has 4 main roles plus boss variants.

### 1. Warrior (Melee Combos)

**Description**: Melee fighters with combo attack chains

**Combat Style:**
- Close-range melee attacks
- 2-4 attack combo chains
- Each attack has windup and recovery
- Combo window timing (must continue within time)
- Can dodge and block

**Configuration:**
```
Combat Role: Warrior

Melee Combo Chains:
- Combo 1:
  * Attack 1: Slash (20 damage, 0.3s windup, 0.5s recovery)
  * Attack 2: Thrust (25 damage, 0.4s windup, 0.6s recovery)
  * Attack 3: Overhead (35 damage, 0.6s windup, 0.8s recovery)
  * Combo Window: 1.5s (time to continue combo)

- Combo 2:
  * Attack 1: Quick Jab (15 damage, 0.2s windup)
  * Attack 2: Spin Attack (30 damage, 0.5s windup)
  * Combo Window: 1.0s (faster combo)
```

**Behavior:**
- Move to melee range (150 units)
- Start random combo
- Execute attacks with timing
- Occasionally dodge (15% chance)
- Block when low health

**Example Warriors:**
- Skeleton warriors
- Goblin fighters
- Orc berserkers
- Knight enemies
- Samurai NPCs

---

### 2. Mage (Magic Casting)

**Description**: Ranged magic users with multiple spells

**Combat Style:**
- Mid to long range (400-1000 units)
- Cast variety of spells
- Spell cooldown management
- Projectile and AOE spells
- Maintain distance from player

**Configuration:**
```
Combat Role: Mage

Magic Spells:
- Fireball:
  * Damage: 30
  * Range: 1000
  * Cooldown: 3.0s
  * Windup: 0.8s
  * Is Ranged: true
  * Element: Fire
  * Projectile Class: BP_FireballProjectile

- Ice Nova:
  * Damage: 25
  * Range: 500 (AOE radius)
  * Cooldown: 5.0s
  * Windup: 1.2s
  * Is Ranged: false (AOE)
  * Element: Ice

- Lightning Bolt:
  * Damage: 40
  * Range: 1200
  * Cooldown: 4.0s
  * Element: Lightning
```

**Behavior:**
- Keep 400-1000 unit range
- Retreat if player gets too close
- Select spell not on cooldown
- Predict player movement for projectiles
- Cast AOE when surrounded

**Example Mages:**
- Elemental wizards
- Necromancers
- Shamans
- Dark sorcerers
- Crystal mages

---

### 3. Archer (Ranged Attacks)

**Description**: Bow/crossbow users with ranged tactics

**Combat Style:**
- Long range combat (400-800 optimal)
- Basic arrows, charged shots, multi-shot
- Maintain optimal distance
- Retreat when player gets close
- Requires line of sight

**Configuration:**
```
Combat Role: Archer

Ranged Attacks:
- Basic Arrow:
  * Damage: 20
  * Range: 1000
  * Cooldown: 1.5s
  * Windup: 0.5s (draw bow)
  * Is Ranged: true
  * Projectile Class: BP_Arrow

- Charged Shot:
  * Damage: 45
  * Range: 1200
  * Cooldown: 4.0s
  * Windup: 1.5s (longer draw)

- Multi-Shot:
  * Damage: 15 per arrow
  * Range: 800
  * Cooldown: 6.0s
  * Fires 5 arrows in spread
```

**Behavior:**
- Maintain 400-800 unit range
- Retreat if player under 400 units
- Only attack with line of sight
- Predict player movement
- Kite player around arena

**Example Archers:**
- Skeleton archers
- Elf rangers
- Goblin snipers
- Crossbow guards
- Hunter NPCs

---

### 4. Boss (Complex Patterns)

**Description**: Multi-phase bosses with special attack patterns

**Combat Style:**
- Combines all combat styles
- Phase-based difficulty scaling
- 11 unique attack patterns
- Summons minions
- Enraged modes
- Telegraphed attacks

**Boss Phases:**
```
Boss Phases Configuration:

Phase 1 (100-75% health):
- Health Threshold: 1.0
- Available Patterns: [BasicCombo, AreaOfEffect]
- Attack Speed Multiplier: 1.0
- Damage Multiplier: 1.0
- Summon Minions: false

Phase 2 (75-50% health):
- Health Threshold: 0.75
- Available Patterns: [Charge, TeleportStrike, RangedBarrage]
- Attack Speed Multiplier: 1.2
- Damage Multiplier: 1.15
- Summon Minions: true
- Minion Count: 3
- Minion Class: BP_SkeletonMinion

Phase 3 (50-25% health):
- Health Threshold: 0.50
- Available Patterns: [GroundSlam, ElementalBurst, EnragedMode]
- Attack Speed Multiplier: 1.5
- Damage Multiplier: 1.3
- Summon Minions: true
- Minion Count: 5

Phase 4 (25-0% health - Enraged):
- Health Threshold: 0.25
- Available Patterns: [All Patterns]
- Attack Speed Multiplier: 2.0
- Damage Multiplier: 1.5
- Summon Minions: true
- Minion Count: 7
```

**11 Boss Attack Patterns:**

1. **Basic Combo**: Standard melee combo attacks
2. **Area of Effect**: Damage all nearby players (500 radius, 1.5x damage)
3. **Charge Attack**: Dash forward 1000 units, heavy damage
4. **Summon Minions**: Spawn phase-specific minions
5. **Enraged Mode**: 1.5x attack speed, 1.3x damage permanently
6. **Teleport Strike**: Teleport behind player, immediate attack (2x damage)
7. **Ground Slam**: Jump and slam, AOE damage
8. **Ranged Barrage**: Fire 5 projectiles in 0.3s intervals
9. **Defensive Stance**: 2x defense for 5 seconds
10. **Elemental Burst**: Massive elemental AOE based on boss element
11. **Phased Attack**: Complex multi-step attack sequence

**Boss Behavior:**
- Phase transitions at health thresholds
- Select random pattern from phase patterns
- Execute pattern with dramatic telegraphs
- Summon minions at phase starts
- Scale difficulty with each phase

**Example Bosses:**
- Dragon lords
- Demon kings
- Ancient liches
- Elemental titans
- Corrupted knights

---

## Safe Zone System

Safe zones are protected areas where aggressive mobs cannot enter.

**Features:**
- Box-shaped protected area
- Push enemies out if they try to enter
- Optional player healing
- Visible debug boundaries
- Events for player enter/exit

**Configuration:**
```
SafeZoneVolume Settings:

- Zone Name: "Village Square"
- Box Extent: (1000, 1000, 500) (X, Y, Z size)
- Show Debug Bounds: true
- Debug Color: Green
- Prevent Combat: true
- Prevent Enemy Entry: true
- Heal Players: true
- Heal Rate: 5.0 HP/second
```

**Setup in Level:**
1. Add SafeZoneVolume actor
2. Set box extent to cover area
3. Enable prevent enemy entry
4. Optional: Enable healing
5. Name the zone for debugging

**Gameplay Uses:**
- Towns and villages
- Campfires and rest areas
- Tutorial zones
- Puzzle areas (no combat)
- Player respawn locations

---

## Blueprint Setup Guide

### Creating a Basic Enemy

1. **Create Enemy Blueprint:**
   - Right-click Content Browser
   - Blueprint Class > CombatEntity
   - Name: `BP_BasicWarrior`

2. **Add AI Components:**
   - Open blueprint
   - Add Component > AIBehaviorComponent
   - Add Component > CombatAIComponent

3. **Configure Behavior:**
   - Select AIBehaviorComponent
   - Behavior Type: Choose from dropdown
   - Configure settings for chosen type

4. **Configure Combat:**
   - Select CombatAIComponent
   - Combat Role: Choose role
   - Add attacks/spells/combos

5. **Set Stats:**
   - Max Health: 100
   - Base Damage: 20
   - Movement Speed: 400
   - Element Type: Choose element

6. **Test:**
   - Place in level
   - Play and observe behavior

### Creating a Pack of Neutral Wolves

```
1. Create BP_Wolf (child of CombatEntity)
2. Add AIBehaviorComponent:
   - Behavior Type: Neutral
   - Group Type: SmallPack
   - Min Group Size: 3
   - Max Group Size: 5
   - Call For Help: true
   - Help Radius: 500

3. Add CombatAIComponent:
   - Combat Role: Warrior
   - Add melee combo:
     * Bite (25 damage)
     * Claw (20 damage)
     * Lunge (30 damage)

4. Set Stats:
   - Health: 80
   - Speed: 450 (faster than player)
   - Damage: 20

5. Spawn 3-5 BP_Wolf near each other in level
6. They'll form pack automatically
7. Attack one = all attack you
```

### Creating a Territory Boss

```
1. Create BP_ForestGuardian (boss blueprint)
2. Add AIBehaviorComponent:
   - Behavior Type: AreaGuard
   - Territory Radius: 2000
   - Enable Anti-Cheese: true
   - Punishment Mobs: BP_TreeSpirit
   - Unique Drops: [Legendary Axe, Forest Crown]

3. Add CombatAIComponent:
   - Combat Role: Boss
   - Is Boss: true
   - Add 3 phases:

   Phase 1 (100-66%):
     - Patterns: [BasicCombo, GroundSlam]
     - Speed: 1.0x, Damage: 1.0x

   Phase 2 (66-33%):
     - Patterns: [Charge, SummonMinions, ElementalBurst]
     - Speed: 1.3x, Damage: 1.2x
     - Summon: 3x BP_TreeSpirit

   Phase 3 (33-0%):
     - Patterns: [All]
     - Speed: 1.8x, Damage: 1.5x
     - Summon: 5x BP_TreeSpirit

4. Set Stats:
   - Health: 1000 (boss health)
   - Damage: 50
   - Speed: 500

5. Place in forest clearing
6. Set up arena boundaries
```

---

## Example Configurations

### Easy Starting Area

**Passive Deer:**
- Health: 50
- Speed: 300
- Flee Distance: 600
- No combat

**Chasing Fireflies (Collectible):**
- Health: 10
- Speed: 400
- Stamina: 50 (easy to catch)
- Reward: Glow Essence

**Neutral Rabbits:**
- Health: 30
- Small packs (2-3)
- Weak attacks if provoked

### Mid-Game Dungeon

**Area Guard: Dungeon Keeper**
- Health: 300
- Territory: 1500 radius
- Anti-Cheese: enabled
- Drops: Rare Sword

**Warrior Skeletons:**
- Health: 120
- 3-hit combos
- Patrol dungeon

**Mage Liches:**
- Health: 100
- Fire + Ice spells
- Support skeletons

### End-Game Boss Arena

**Aggressive Dragon Boss:**
- Health: 5000
- 4 phases
- Flies and lands
- Summons dragonlings
- Respects safe zone at arena entrance

**Phase 1:** Basic attacks
**Phase 2:** Starts flying, ranged fire
**Phase 3:** Summons minions, enraged
**Phase 4:** All attacks, constant minions

---

## Advanced Features

### Group Formation

Spawn multiple neutral mobs near each other:
```
1. Place 5 wolves in circle (300 unit radius)
2. All set to Neutral + SmallPack
3. They'll automatically form pack
4. Leader is first wolf
5. Others follow leader
6. Attack one = all attack
```

### Custom Punishment Mobs

Create challenging anti-cheese mobs:
```
1. Create BP_PunishmentWarrior
2. Set very high stats:
   - Health: 500
   - Damage: 100 (2x player damage)
   - Speed: 600 (can't run away)
3. Add aggressive behavior
4. Use as punishment mob class
5. Makes cheesing extremely risky
```

### Safe Zone Network

Create connected safe zones:
```
Town Hub:
- Large safe zone (2000 radius)
- Healing enabled
- Shops and NPCs

Path Campsites:
- Small safe zones (500 radius)
- Between dangerous areas
- Quick heal stops

Boss Arena Entrance:
- Medium safe zone (1000 radius)
- Prep area before boss
- Boss waits at boundary
- Creates tension
```

### Boss Minion Waves

Configure escalating difficulty:
```
Phase 1: No minions (learn pattern)
Phase 2: 2 weak minions
Phase 3: 4 medium minions
Phase 4: 6 strong minions + constant spawning

Forces player to manage adds while fighting boss
```

---

## Testing Checklist

- [ ] Passive mobs wander and eat
- [ ] Passive mobs flee from player
- [ ] Chasing mobs flee and exhaust
- [ ] Chasing mobs drop reward when caught
- [ ] Neutral mobs ignore player
- [ ] Neutral mobs attack when provoked
- [ ] Neutral mobs call for help
- [ ] Area guards stay in territory
- [ ] Area guards spawn punishment mobs when cheesed
- [ ] Area guards drop unique loot
- [ ] Aggressive mobs chase player
- [ ] Aggressive mobs stop at safe zones
- [ ] Aggressive mobs stare at player in safe zone
- [ ] Warriors perform combos
- [ ] Mages cast spells with cooldowns
- [ ] Archers maintain range
- [ ] Bosses transition phases at health thresholds
- [ ] Boss patterns execute correctly
- [ ] Safe zones push out enemies
- [ ] Safe zones heal players (if enabled)

---

## Tips and Best Practices

**Balance:**
- Start with lower health for testing
- Gradually increase difficulty
- Use territory guards for important areas
- Place safe zones near hard content

**Performance:**
- Don't spawn too many aggressive mobs
- Use neutral mobs for ambient life
- Limit boss minion counts
- Safe zones have negligible performance cost

**Player Experience:**
- Passive mobs make world feel alive
- Chasing mobs create fun side objectives
- Neutral mobs reward cautious play
- Area guards prevent exploits
- Aggressive bosses create epic encounters
- Safe zones provide necessary breaks

**Level Design:**
- Passive mobs in starting areas
- Chasing mobs in exploration zones
- Neutral packs in wilderness
- Area guards at dungeons/treasures
- Aggressive bosses in special arenas
- Safe zones at checkpoints

---

This comprehensive AI system gives you complete control over enemy behavior and combat, creating a dynamic, challenging open-world experience!
