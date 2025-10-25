# Ninja Wizard Combat Game

An Unreal Engine 5 combat game featuring fluid elemental magic and martial combat systems. Players are ninja wizards who can master various magical elements and weapon styles through mentorship.

## Core Features

### Magic System
- **9 Base Elements**: Fire, Water, Earth, Air, Lightning, Light, Dark, Poison, Ice
- **Mana-Based Casting**: No cooldowns - manage your mana wisely
- **Magic Combinations**: Combine elements to create hybrid magics
  - Fire + Earth = Lava
  - Lightning + Air = Storm
  - Dark + Poison = Curse
  - And 15+ more combinations!
- **Mastery System**: Improve efficiency and power as you master each element

### Combat System
- **4 Weapon Styles**: Two-Handed Sword, Spear, Fist/Martial Arts, Bow
- **Stamina-Based Attacks**: Resource management for physical combat
- **Combo System**: Chain attacks for increased damage
- **Magic Infusion**: Imbue weapons with elemental power
- **No Class Restrictions**: Learn all weapon styles

### Mentor System
- **Dragon Mentors**: Each element has a dedicated dragon teacher
  - Build relationships to unlock advanced magic
  - Complete quests for special abilities
  - Unlock combination magics
- **Combat Tutors**: Two specialized martial instructors
  - Warrior Tutor: Teaches Sword and Spear
  - Ninja Tutor: Teaches Fist and Bow

### Progression
- **Individual Mastery Levels** (0-100) for each element and weapon
- **Relationship System** with mentors (0-100)
- **Mastery Benefits**:
  - Reduced mana/stamina costs
  - Increased damage output
  - Access to advanced techniques
  - Unlock combination magics at mastery thresholds

### Game Structure
- **Level-Based Progression**: Start with linear story levels
- **Open World Transition**: Unlock open world after completing story
- **Dragon Temples**: Visit elemental temples to meet dragon mentors
- **Training Grounds**: Learn combat techniques from tutors

## Technical Architecture

### Core Classes

#### Player Character
- `ANinjaWizardCharacter` - Main player class with mana and stamina systems
- Components:
  - `UMagicComponent` - Spell casting and element management
  - `UWeaponComponent` - Weapon combat and combo system
  - `UMasteryManagerComponent` - Track progression

#### Mentors
- `AMentorBase` - Base class for all mentors
- `ADragonMentor` - Elemental magic teachers
- `ACombatTutor` - Weapon style teachers

#### Game Mode
- `ANinjaWizardGameMode` - Manages level progression and open world transition

### Data Structures
- `EMagicElement` - Enum for all base elements
- `ECombinedMagic` - Enum for fusion magics
- `EWeaponStyle` - Enum for weapon types
- `FSpellData` - Spell configuration
- `FWeaponAttackData` - Attack configuration
- `FMasteryData` - Progression tracking
- `FMagicCombination` - Element fusion recipes

## Getting Started

### Prerequisites
- Unreal Engine 5.6
- Visual Studio 2022 (for C++ compilation)

### Building the Project
1. Clone the repository
2. Right-click `ElementalDanger.uproject` and select "Generate Visual Studio project files"
3. Open `ElementalDanger.sln` in Visual Studio
4. Build the project (Development Editor configuration)
5. Launch from Visual Studio or open the .uproject file

### Creating Content
The C++ systems are designed to be extended in Blueprint:
1. Create Blueprint child classes of core C++ classes
2. Design spell effects using Niagara or Cascade
3. Set up weapon animations and attack sequences
4. Configure dragon and tutor appearances
5. Build levels and temples

## Design Philosophy

### Mana Over Cooldowns
Unlike traditional MMOs, this game focuses on resource management rather than ability cooldowns. Players must balance their mana usage strategically.

### Freedom of Choice
Players can master all elements and all weapon styles. The game encourages experimentation to find your personal playstyle.

### Meaningful Progression
Every action grants mastery experience. Higher mastery provides tangible benefits like reduced costs and increased power.

### Mentor Relationships
Building relationships with mentors unlocks new abilities and combination magics. The journey to mastery involves both combat practice and mentor interaction.

## Future Expansion

### Planned Features
- Additional weapon types (Dual Swords, Staff, Shield)
- Triple-element combinations
- PvP arena combat
- Co-op multiplayer
- Legendary dragon encounters
- Weapon crafting system
- Rare/unique spell variants

### Extensibility
The system is designed for easy expansion:
- Add new elements by extending the enum
- Create new weapon styles
- Add more combination recipes
- Introduce new mentor types

## Development Roadmap

### Phase 1: Core Systems ✓
- Magic element framework
- Mana-based casting
- Weapon combat system
- Mastery progression
- Mentor system foundation

### Phase 2: Content Creation (Next)
- Spell visual effects
- Weapon animations
- Dragon and tutor models
- Level design
- UI/UX implementation

### Phase 3: Gameplay Polish
- Combat balancing
- AI enemies
- Boss encounters
- Quest system
- Dialogue system

### Phase 4: Open World
- World map creation
- Dynamic events
- Fast travel system
- Open world mentor placement

## Contributing
See DESIGN.md for detailed system architecture and design decisions.

## License
Copyright Epic Games, Inc. All Rights Reserved.