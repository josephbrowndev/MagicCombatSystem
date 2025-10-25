# Implementation Guide

This guide will help you implement the Ninja Wizard Combat Game systems in Unreal Engine 5.

## Quick Start Checklist

### 1. Setting Up the Player Character

1. Create a Blueprint based on `ANinjaWizardCharacter`
2. Set up the character mesh and animations
3. Configure mana and stamina values:
   - Default: MaxMana = 100, ManaRegenRate = 5
   - Default: MaxStamina = 100, StaminaRegenRate = 10
4. Add input bindings for movement, jumping, casting, and attacking

### 2. Creating Spells

Create spell data using the `FSpellData` structure:

```cpp
// Example Fire Spell in Blueprint or C++
FSpellData Fireball;
Fireball.SpellName = "Fireball";
Fireball.Element = EMagicElement::Fire;
Fireball.ManaCost = 20.0f;
Fireball.BaseDamage = 50.0f;
Fireball.CastTime = 1.0f;
Fireball.RequiredMastery = 0;
Fireball.Description = "A basic fire projectile";
```

### 3. Setting Up Dragon Mentors

For each element, create a Blueprint based on `ADragonMentor`:

1. Set the `PrimaryElement` to the dragon's element
2. Set the `MentorType` appropriately
3. Add `TeachableSpells` for that element
4. Configure appearance and scale
5. Implement Blueprint events:
   - `OnPlayerInteract` - Show dialogue UI
   - `OnSpellTaught` - Visual feedback
   - `OnElementUnlockedForPlayer` - Celebration effect

### 4. Setting Up Combat Tutors

Create two Blueprints based on `ACombatTutor`:

**Warrior Tutor:**
- Specialization: Warrior
- Teaches: TwoHandedSword, Spear

**Ninja Tutor:**
- Specialization: Ninja
- Teaches: Fist, Bow

### 5. Creating Weapon Attacks

Define attacks using `FWeaponAttackData`:

```cpp
// Example Sword Attack
FWeaponAttackData BasicSlash;
BasicSlash.AttackName = "BasicSlash";
BasicSlash.WeaponStyle = EWeaponStyle::TwoHandedSword;
BasicSlash.StaminaCost = 15.0f;
BasicSlash.BaseDamage = 40.0f;
BasicSlash.AttackSpeed = 1.2f;
BasicSlash.ComboIndex = 0;  // First hit in combo
BasicSlash.RequiredMastery = 0;
```

## Element Combination Reference

| Element 1 | Element 2 | Result | Required Mastery |
|-----------|-----------|---------|------------------|
| Fire | Earth | Lava | 25 |
| Lightning | Air | Storm | 30 |
| Dark | Poison | Curse | 35 |
| Fire | Air | Inferno | 25 |
| Water | Ice | Glacier | 20 |
| Light | Fire | Solar | 40 |
| Dark | Fire | Hellfire | 45 |
| Water | Earth | Swamp | 20 |
| Lightning | Water | Electro Flood | 30 |
| Fire | Water | Steam | 25 |
| Earth | Ice | Crystal | 35 |
| Ice | Air | Blizzard | 30 |
| Light | Air | Radiance | 35 |
| Dark | Air | Shadow | 30 |
| Poison | Water | Toxin | 25 |
| Lightning | Fire | Plasma | 40 |
| Dark | Earth | Corruption | 35 |

## UI Implementation

### Player HUD Should Display:
1. **Resource Bars:**
   - Mana (current/max) with regen indicator
   - Stamina (current/max) with regen indicator
   - Health (implement as needed)

2. **Mastery Progress:**
   - Active element mastery levels
   - Active weapon mastery levels
   - Experience progress bars

3. **Active Spells/Abilities:**
   - Equipped spells with mana costs
   - Current weapon style
   - Available combos

4. **Mentor Relationships:**
   - Relationship levels with each mentor
   - Available quests

### Example Widget Bindings:

```cpp
// Get mana percentage for progress bar
float ManaPercent = PlayerCharacter->GetManaPercentage();

// Get mastery level for display
int32 FireMastery = PlayerCharacter->MasteryManager->GetMagicMastery(EMagicElement::Fire);

// Check available combinations
TArray<ECombinedMagic> Available = PlayerCharacter->MagicComponent->GetAvailableCombinations();
```

## Level Design Guidelines

### Tutorial Level
- Introduce basic movement and combat
- Meet first dragon mentor (Fire recommended)
- Learn first element and basic spells
- Meet Warrior or Ninja tutor
- Learn first weapon style

### Elemental Temple Levels
Each element gets a dedicated level:
1. Environmental theme matching the element
2. Puzzles requiring that element
3. Dragon mentor encounter
4. Boss fight to prove mastery
5. Reward: Advanced spells and combination unlock

### Suggested Level Order:
1. Tutorial
2. Fire Dragon Temple
3. Water Dragon Shrine
4. Earth Dragon Cavern
5. Air Dragon Peak
6. Lightning Dragon Storm
7. Light Dragon Sanctuary
8. Dark Dragon Abyss
9. Poison Dragon Swamp
10. Ice Dragon Glacier
11. Final Challenge
12. Open World Unlock

## Combat Balancing Recommendations

### Mana Costs:
- Basic Spells: 10-20 mana
- Intermediate: 25-40 mana
- Advanced: 45-60 mana
- Combined Magic: 50-80 mana
- Ultimate: 80-100 mana

### Stamina Costs:
- Light Attack: 10-15 stamina
- Heavy Attack: 20-30 stamina
- Special Move: 35-50 stamina

### Mastery Scaling:
- 0-25: Beginner
- 26-50: Competent
- 51-75: Expert
- 76-99: Master
- 100: Grand Master

### Benefits by Mastery Level:
- **25**: Unlock first combinations, -10% cost
- **50**: Advanced techniques, -25% cost, +15% damage
- **75**: Master techniques, -40% cost, +30% damage
- **100**: Grand Master, -50% cost, +50% damage, unique abilities

## Blueprint Events to Implement

### In ANinjaWizardCharacter Blueprint:
- `OnManaChanged` - Update UI
- `OnStaminaChanged` - Update UI
- `OnSpellCast` - Play visual effects
- `OnDeath` - Handle respawn

### In UMagicComponent:
- `OnSpellCast` - Spawn projectile/effect
- `OnElementUnlocked` - Notification
- `OnCombinationUnlocked` - Celebration

### In UWeaponComponent:
- `OnAttackPerformed` - Play animation
- `OnWeaponEquipped` - Change mesh
- `OnComboIncremented` - Visual feedback
- `OnWeaponInfused` - Add particle effects

### In AMentorBase:
- `OnPlayerInteract` - Dialogue system
- `OnSpellTaught` - Teaching animation
- `OnQuestCompleted` - Reward ceremony

## Testing Checklist

### Magic System:
- [ ] Mana consumption works correctly
- [ ] Mana regenerates at proper rate
- [ ] Spells respect mastery requirements
- [ ] Element combinations unlock at proper mastery
- [ ] Mastery experience gained from casting
- [ ] Mastery reduces mana costs

### Combat System:
- [ ] Stamina consumption works correctly
- [ ] Stamina regenerates properly
- [ ] Combo system chains correctly
- [ ] Weapon switching works
- [ ] Magic infusion adds damage
- [ ] Mastery improves weapon damage

### Mentor System:
- [ ] Can interact with mentors
- [ ] Relationship improves over time
- [ ] Can learn spells from dragons
- [ ] Can learn attacks from tutors
- [ ] Quests appear at correct mastery levels
- [ ] Quest rewards granted properly

### Progression:
- [ ] Mastery levels up from experience
- [ ] Benefits apply at mastery thresholds
- [ ] Can unlock all elements
- [ ] Can unlock all weapons
- [ ] Track progress across saves

## Performance Optimization Tips

1. **Spell Effects:**
   - Use Niagara for efficient particle systems
   - Pool frequently-used effects
   - LOD for distant effects

2. **Mastery Calculations:**
   - Cache mastery values when possible
   - Update only on level-up, not every tick

3. **Mentor AI:**
   - Disable tick when player not nearby
   - Use event-driven interaction

4. **UI Updates:**
   - Update bars only when values change
   - Throttle mastery progress updates

## Next Steps

1. Create Blueprint implementations of all C++ classes
2. Design and implement spell visual effects
3. Create weapon attack animations
4. Build mentor character models (or use placeholders)
5. Design first level (Tutorial)
6. Implement basic UI
7. Create data tables for spells and attacks
8. Playtest and balance

## Resources

- **DESIGN.md** - Detailed system architecture
- **README.md** - Project overview
- **Source/** - C++ implementation files
- [Unreal Engine Documentation](https://docs.unrealengine.com/)

## Support

For questions about the implementation, refer to the code comments in the C++ files. Each class is thoroughly documented with its purpose and usage.