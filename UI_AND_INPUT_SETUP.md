# UI and Input Setup Guide

This guide walks you through setting up the Enhanced Input system, camera controls, and dialogue UI for the Ninja Wizard game.

## Table of Contents
1. [Enhanced Input Setup](#enhanced-input-setup)
2. [Creating UI Widgets](#creating-ui-widgets)
3. [Testing Everything](#testing-everything)

---

## Enhanced Input Setup

### Step 1: Create Input Mapping Context

1. In Content Browser, create a new folder: `Content/Input`
2. Right-click → Input → Input Mapping Context
3. Name it `IMC_Default`
4. Open it and add the following mappings:

#### Movement (IA_Move)
- Create Input Action: `Content/Input/IA_Move`
  - Value Type: Axis2D (Vector 2D)
- In IMC_Default, add IA_Move:
  - **W Key**: Modifiers → Swizzle Input Axis Values (Order: YXZ)
  - **S Key**: Modifiers → Swizzle Input Axis Values (Order: YXZ), then Negate
  - **A Key**: Modifiers → Negate
  - **D Key**: (No modifiers)
  - **Gamepad Left Thumbstick 2D**: (No modifiers)

#### Look (IA_Look)
- Create Input Action: `Content/Input/IA_Look`
  - Value Type: Axis2D (Vector 2D)
- In IMC_Default, add IA_Look:
  - **Mouse XY 2D-Axis**: (No modifiers)
  - **Gamepad Right Thumbstick 2D**: (No modifiers)

#### Jump (IA_Jump)
- Create Input Action: `Content/Input/IA_Jump`
  - Value Type: Digital (bool)
- In IMC_Default, add IA_Jump:
  - **Space Bar**
  - **Gamepad Face Button Bottom** (A/Cross)

#### Interact (IA_Interact)
- Create Input Action: `Content/Input/IA_Interact`
  - Value Type: Digital (bool)
- In IMC_Default, add IA_Interact:
  - **E Key**
  - **Gamepad Face Button Right** (B/Circle)

#### Cast Spell (IA_CastSpell)
- Create Input Action: `Content/Input/IA_CastSpell`
  - Value Type: Digital (bool)
- In IMC_Default, add IA_CastSpell:
  - **Left Mouse Button**
  - **Gamepad Right Trigger**

#### Attack (IA_Attack)
- Create Input Action: `Content/Input/IA_Attack`
  - Value Type: Digital (bool)
- In IMC_Default, add IA_Attack:
  - **Right Mouse Button**
  - **Gamepad Left Trigger**

### Step 2: Assign Input Actions to Character Blueprint

1. Create a Blueprint based on `NinjaWizardCharacter`: `BP_NinjaWizardCharacter`
2. Open it and go to **Class Defaults**
3. In the **Input** section, assign:
   - Default Mapping Context: `IMC_Default`
   - Move Action: `IA_Move`
   - Look Action: `IA_Look`
   - Jump Action: `IA_Jump`
   - Interact Action: `IA_Interact`
   - Cast Spell Action: `IA_CastSpell`
   - Attack Action: `IA_Attack`

4. Adjust camera settings:
   - Mouse Sensitivity: 1.0 (adjust to preference)
   - Invert Camera Y: false (or true if you prefer inverted)

5. Adjust interaction settings:
   - Interaction Range: 300.0 (units)
   - Interaction Check Frequency: 0.1 (seconds)

---

## Creating UI Widgets

### Step 1: Create Player HUD Widget

1. Create folder: `Content/UI`
2. Right-click → User Interface → Widget Blueprint
3. Name it: `WBP_PlayerHUD`

#### Design the HUD:

**Canvas Panel** (Root)
├─ **Overlay** - Mana Bar Container (Anchors: Bottom-Left)
│   ├─ **Image** - Background (Tint: Dark Blue/Black)
│   ├─ **Progress Bar** - ManaBar
│   │   - Fill Color: Blue
│   │   - Bind Percent to Character's `GetManaPercentage()`
│   └─ **Text Block** - Mana Text
│       - Text: "Mana: 100/100"
│       - Bind to format mana values
│
├─ **Overlay** - Stamina Bar Container (Anchors: Bottom-Left, below Mana)
│   ├─ **Image** - Background (Tint: Dark Green/Black)
│   ├─ **Progress Bar** - StaminaBar
│   │   - Fill Color: Green
│   │   - Bind Percent to Character's `GetStaminaPercentage()`
│   └─ **Text Block** - Stamina Text
│       - Text: "Stamina: 100/100"
│
└─ **Vertical Box** - Mastery Display (Anchors: Top-Right)
    └─ Add text blocks for active masteries

#### Blueprint Setup (Graph):

1. Create variable: `OwningCharacter` (Type: Ninja Wizard Character, Instance Editable)
2. **Event Construct**:
   ```
   Get Owning Player Pawn → Cast to NinjaWizardCharacter → Set OwningCharacter
   ```

3. **Bind Mana Bar Percent**:
   ```
   OwningCharacter → GetManaPercentage → Return Value
   ```

4. **Bind Stamina Bar Percent**:
   ```
   OwningCharacter → GetStaminaPercentage → Return Value
   ```

### Step 2: Create Interaction Prompt Widget

1. Create Widget Blueprint: `WBP_InteractionPrompt`

#### Design:

**Canvas Panel** (Root)
└─ **Border** (Anchors: Bottom-Center)
    └─ **Text Block** - PromptText
        - Font Size: 24
        - Text: "Press E to Interact"
        - Justification: Center

#### Blueprint Setup:

1. Make `PromptText` a variable (click "Is Variable" checkbox)
2. Create function `SetPromptText`:
   - Input: Text (type: Text)
   - Set PromptText's Text to the input

### Step 3: Create Dialogue Widget

1. Create Widget Blueprint based on `DialogueWidget`: `WBP_Dialogue`

#### Design:

**Canvas Panel** (Root)
└─ **Border** - Main Container (Anchors: Fill, with margins)
    - Background Color: Semi-transparent black
    └─ **Vertical Box**
        ├─ **Horizontal Box** - Header
        │   ├─ **Text Block** - Mentor Name
        │   │   - Font Size: 32, Color: Gold
        │   └─ **Button** - Close Button
        │       └─ **Text Block**: "X"
        │
        ├─ **Spacer** (Size: 20)
        │
        ├─ **Border** - Dialogue Area
        │   └─ **Text Block** - DialogueText
        │       - Font Size: 18
        │       - Auto Wrap: true
        │       - Text: "Greeting dialogue here..."
        │
        ├─ **Spacer** (Size: 20)
        │
        └─ **Horizontal Box** - Action Buttons
            ├─ **Button** - LearnSpellsButton
            │   └─ **Text**: "Learn Spells"
            ├─ **Button** - LearnWeaponsButton
            │   └─ **Text**: "Learn Weapons"
            ├─ **Button** - ViewQuestsButton
            │   └─ **Text**: "View Quests"
            └─ **Button** - ExitButton
                └─ **Text**: "Exit"

#### Blueprint Setup (Graph):

1. Override `OnDialogueInitialized`:
   ```
   Event OnDialogueInitialized
   → Set MentorName text to Mentor's MentorName
   → DisplayDialogue with greeting text
   ```

2. Override `OnDialogueDisplayed`:
   ```
   Event OnDialogueDisplayed
   → Set DialogueText to the provided text
   ```

3. **Close Button Clicked**:
   ```
   OnClicked (Close Button) → CloseDialogue
   ```

4. **Learn Spells Button**:
   ```
   OnClicked (LearnSpellsButton)
   → Get CurrentMentor's TeachableSpells
   → Display in list or menu
   → Call TeachSpell on selection
   ```

5. **Learn Weapons Button**:
   ```
   OnClicked (LearnWeaponsButton)
   → Get CurrentMentor's TeachableAttacks
   → Display in list or menu
   → Call TeachAttack on selection
   ```

6. **View Quests Button**:
   ```
   OnClicked (ViewQuestsButton)
   → GetAvailableQuests from CurrentMentor
   → Display quest list
   ```

7. **Exit Button**:
   ```
   OnClicked (ExitButton) → CloseDialogue
   ```

### Step 4: Create HUD Blueprint

1. Create Blueprint based on `NinjaWizardHUD`: `BP_NinjaWizardHUD`
2. Open **Class Defaults**
3. Assign Widget Classes:
   - Player HUD Class: `WBP_PlayerHUD`
   - Dialogue Widget Class: `WBP_Dialogue`
   - Interaction Prompt Class: `WBP_InteractionPrompt`

### Step 5: Create Game Mode Blueprint

1. Create Blueprint based on `NinjaWizardGameMode`: `BP_NinjaWizardGameMode`
2. Open **Class Defaults**
3. Set:
   - Default Pawn Class: `BP_NinjaWizardCharacter`
   - HUD Class: `BP_NinjaWizardHUD`

---

## Creating a Test Mentor

### Step 1: Create Mentor Blueprint

1. Create Blueprint based on `DragonMentor`: `BP_FireDragon`
2. Add a visible component:
   - Add **Static Mesh** or **Skeletal Mesh** component
   - For testing, you can use a simple cube or sphere
   - Scale it appropriately

3. Set **Class Defaults**:
   - Mentor Type: Dragon Fire
   - Mentor Name: "Fire Dragon"
   - Primary Element: Fire
   - Relationship Level: 0

4. Add some test spells to `TeachableSpells`:
   - Create a few FSpellData entries in the array
   - Example: Fireball (ManaCost: 20, BaseDamage: 50, etc.)

### Step 2: Implement Visual Feedback (Optional)

Override the Blueprint events:

**OnInteractionFocus**:
```
Event OnInteractionFocus
→ Get Static Mesh Component
→ Set Render Custom Depth: true
→ Set Custom Depth Stencil Value: 1
```

**OnInteractionUnfocus**:
```
Event OnInteractionUnfocus
→ Get Static Mesh Component
→ Set Render Custom Depth: false
```

For this to work, enable Custom Depth in **Project Settings → Rendering → PostProcessing**.

---

## Testing Everything

### Step 1: Set Up Level

1. Create or open a level
2. Set **World Settings → Game Mode**: `BP_NinjaWizardGameMode`
3. Place `BP_FireDragon` in the level
4. Make sure there's a Player Start

### Step 2: Test Checklist

#### Movement & Camera:
- [ ] WASD keys move the character
- [ ] Mouse moves the camera
- [ ] Space bar makes character jump
- [ ] Character rotates to face movement direction

#### Interaction System:
- [ ] Walk towards the Fire Dragon
- [ ] Interaction prompt appears: "Press E to talk to Fire Dragon"
- [ ] Dragon highlights (if you implemented visual feedback)
- [ ] Walk away - prompt disappears

#### Dialogue System:
- [ ] Press E near the dragon
- [ ] Dialogue window opens
- [ ] Mouse cursor appears
- [ ] Can click buttons in dialogue
- [ ] Close button or Exit button closes dialogue
- [ ] Game returns to normal controls

#### UI Display:
- [ ] Mana bar shows at bottom-left
- [ ] Stamina bar shows at bottom-left
- [ ] Bars update when casting spells or attacking
- [ ] Bars regenerate over time

### Step 3: Debug Common Issues

**No Input Response:**
- Check that IMC_Default is assigned in BP_NinjaWizardCharacter
- Verify all Input Actions are assigned
- Check Project Settings → Input → Default Classes use Enhanced Input

**Dialogue Doesn't Open:**
- Check BP_NinjaWizardHUD has WBP_Dialogue assigned
- Verify BP_NinjaWizardGameMode has BP_NinjaWizardHUD as HUD Class
- Check World Settings is using BP_NinjaWizardGameMode

**Can't See UI:**
- Verify WBP_PlayerHUD is assigned in BP_NinjaWizardHUD
- Check widget visibility settings
- Make sure widgets are added to viewport

**Interaction Not Working:**
- Check Interaction Range in BP_NinjaWizardCharacter (default: 300)
- Verify mentor has collision enabled
- Check mentor implements IInteractableInterface (it does by default)

---

## Advanced Customization

### Styling the UI

Edit the widgets to match your game's art style:
- Replace progress bars with custom textures
- Add background images and borders
- Use custom fonts
- Add particle effects or animations

### Enhancing Dialogue

Add more features to the dialogue system:
- Dialogue trees with branching options
- Voice acting integration
- Animated portrait images
- Quest tracking integration
- Spell/weapon selection UI

### Input Customization

Players can customize controls by:
1. Creating a settings menu
2. Using Enhanced Input's remapping features
3. Saving preferences to Game Instance or Save Game

---

## Next Steps

1. **Create More Mentors**: Follow the same process for other dragon mentors and combat tutors
2. **Add Spell Selection UI**: Create a spell bar or radial menu for selecting spells
3. **Implement Quest System**: Expand dialogue to show and track quests
4. **Polish Visual Feedback**: Add particles, sounds, and animations
5. **Create Main Menu**: Add a title screen and pause menu

For more details on the game systems, see:
- `README.md` - Project overview
- `DESIGN.md` - System architecture
- `IMPLEMENTATION_GUIDE.md` - Content creation guide
