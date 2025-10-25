# Ninja Wizard Combat Game - Design Document

## Overview
A mana-based combat game featuring elemental magic and multiple weapon styles. Players are ninja wizards who can master various magical elements and combat techniques through mentorship from dragons and martial tutors.

## Core Systems

### 1. Magic System

#### Base Elements (9 Total)
1. Fire
2. Water
3. Earth
4. Air
5. Lightning
6. Light
7. Dark
8. Poison
9. Ice

#### Magic Combination System
Players can combine two elements to create hybrid magics:
- Fire + Earth = Lava
- Lightning + Air = Storm
- Dark + Poison = Curse
- Fire + Air = Inferno
- Water + Ice = Glacier
- Light + Fire = Solar
- Dark + Fire = Hellfire
- Water + Earth = Mud/Swamp
- Lightning + Water = Electro-Current
- And many more combinations...

#### Mana System
- **Mana-based casting** (NOT cooldown-based)
- Each spell costs mana to cast
- Mana regenerates over time
- Higher mastery = more efficient mana usage
- Different spell tiers cost different amounts of mana

### 2. Weapon Combat System

#### Attack Styles (4 + expandable)
1. **Two-Handed Sword** - Heavy, powerful strikes
2. **Spear** - Medium range, precise attacks
3. **Fist/Martial Arts** - Fast, combo-based
4. **Bow** - Ranged physical attacks

#### Combat Mechanics
- Each weapon style has unique move sets
- Weapons can be infused with magic
- Combo system for chaining attacks
- Stamina system for melee attacks (separate from mana)

### 3. Mentor/Tutor System

#### Dragon Mentors (Magic)
- Each element has a dragon mentor
- Dragons teach spells and magical techniques
- Unlock combination magics through multiple dragon relationships
- Dragon approval/reputation system

#### Combat Tutors (Weapons)
1. **Warrior Tutor** - Teaches sword and spear
2. **Ninja Tutor** - Teaches fist and bow

Players can learn from both tutors to master all weapon styles.

### 4. Progression System

#### Mastery Levels
- Individual mastery for each element (1-100)
- Individual mastery for each weapon style (1-100)
- Mastery affects:
  - Mana efficiency
  - Spell/attack power
  - Unlock new abilities
  - Access to combination magics

#### Character Development
- No class restrictions - learn everything
- Encourage personal playstyle through mastery bonuses
- Create unique builds by combining preferred elements and weapons

### 5. Game Structure

#### Initial: Level-Based Design
- Linear progression through zones/levels
- Each level introduces new elements or challenges
- Tutorial levels with mentors
- Boss fights to prove mastery

#### Future: Open World Expansion
- Convert levels into interconnected open world zones
- Free-form exploration
- Dynamic mentor locations
- World events and challenges

## Technical Architecture

### Core Classes
1. **MagicElement** - Base class for all elements
2. **MagicCombination** - Handles element fusion
3. **ManaComponent** - Manages player mana
4. **WeaponStyle** - Base class for weapon types
5. **PlayerCharacter** - Main player controller
6. **MentorSystem** - Handles learning and progression
7. **MasteryManager** - Tracks player progression

### Data Structures
- Element Data Table
- Spell Data Table
- Combination Recipe Table
- Weapon Data Table
- Mentor Dialogue/Quest Data

## Gameplay Loop

1. Meet mentor (dragon or combat tutor)
2. Learn basic spells/techniques
3. Practice in combat scenarios
4. Gain mastery through use
5. Unlock combinations/advanced techniques
6. Develop personal combat style
7. Master all elements and weapons
8. Create unique hybrid playstyle

## Future Expansion Ideas
- More weapon types (dual swords, staff, shield, etc.)
- Advanced element combinations (3+ elements)
- PvP combat
- Co-op multiplayer
- Rare/legendary dragon mentors
- Ultimate fusion magics
- Weapon crafting and customization
