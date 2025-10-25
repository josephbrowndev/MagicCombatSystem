# Implementing Dialogue Button Functions - Detailed Guide

This guide shows you exactly how to implement the Learn Spells, Learn Weapons, and View Quests buttons in your dialogue widget.

## Table of Contents
1. [Simple Approach - Direct Teaching](#simple-approach)
2. [Intermediate Approach - Scroll Box List](#intermediate-approach)
3. [Advanced Approach - Dynamic Menu](#advanced-approach)

---

## Simple Approach - Direct Teaching

This approach teaches all available spells/weapons at once when the button is clicked.

### Learn Spells Button

1. In `WBP_Dialogue`, select the **LearnSpellsButton**
2. In the Details panel, scroll to **Events** → Click the **+** next to **On Clicked**
3. In the Event Graph:

```
Event OnClicked (LearnSpellsButton)
├─ Get CurrentMentor
│  └─ Is Valid? (Branch)
│     └─ True:
│        ├─ Get TeachableSpells (from CurrentMentor)
│        └─ ForEachLoop (connect TeachableSpells array)
│           ├─ Loop Body:
│           │  ├─ Get CurrentMentor
│           │  ├─ Get PlayerCharacter
│           │  └─ Call TeachSpell (on CurrentMentor)
│           │     ├─ Player: PlayerCharacter
│           │     └─ Spell: Array Element (from ForEachLoop)
│           └─ Completed:
│              └─ Print String ("Learned all available spells!")
```

**Blueprint Node Details:**
- **Get CurrentMentor**: Variable → Get CurrentMentor
- **Is Valid**: Utilities → Flow Control → Is Valid
- **Get TeachableSpells**: Drag off CurrentMentor → Search "Teachable Spells" → Get
- **ForEachLoop**: Right-click → Utilities → Flow Control → For Each Loop
  - Connect TeachableSpells array to "Array" pin
- **TeachSpell**: Drag off CurrentMentor → Search "Teach Spell"
  - Connect PlayerCharacter to Player pin
  - Connect "Array Element" from ForEachLoop to Spell pin

### Learn Weapons Button

Same structure but use **TeachableAttacks** and **TeachAttack**:

```
Event OnClicked (LearnWeaponsButton)
├─ Get CurrentMentor
│  └─ Is Valid?
│     └─ True:
│        ├─ Get TeachableAttacks
│        └─ ForEachLoop
│           ├─ Loop Body:
│           │  └─ Call TeachAttack
│           │     ├─ Player: PlayerCharacter
│           │     └─ Attack: Array Element
│           └─ Completed:
│              └─ Print String ("Learned all available attacks!")
```

### View Quests Button

```
Event OnClicked (ViewQuestsButton)
├─ Get CurrentMentor
│  └─ Is Valid?
│     └─ True:
│        ├─ Get PlayerCharacter
│        └─ Call GetAvailableQuests (on CurrentMentor)
│           ├─ Player: PlayerCharacter
│           └─ Return Value:
│              ├─ Get Length (of array)
│              └─ Branch (Length > 0?)
│                 ├─ True:
│                 │  └─ ForEachLoop (on quests array)
│                 │     └─ Loop Body:
│                 │        ├─ Get QuestName (from Array Element)
│                 │        ├─ Convert to String
│                 │        └─ Print String
│                 └─ False:
│                    └─ Print String ("No quests available")
```

---

## Intermediate Approach - Scroll Box List

This creates a scrollable list of items the player can click on individually.

### Step 1: Create Spell Item Widget

First, create a widget for each spell in the list:

1. Create new Widget Blueprint: `WBP_SpellListItem`
2. Design:

```
Canvas Panel (Root)
└─ Button - MainButton (Fill screen)
    └─ Horizontal Box
        ├─ Text Block - SpellNameText (Size: Fill, Weight: 0.7)
        │   - Text: "Fireball"
        │   - Font Size: 18
        ├─ Spacer (Size: 10)
        ├─ Text Block - ManaCostText (Size: Auto)
        │   - Text: "Mana: 20"
        │   - Font Size: 14
        │   - Color: Light Blue
        └─ Text Block - MasteryText (Size: Auto)
            - Text: "Req: 0"
            - Font Size: 14
            - Color: Yellow
```

3. Make these variables (check "Is Variable"):
   - SpellNameText
   - ManaCostText
   - MasteryText
   - MainButton

4. Create Custom Event: `InitializeSpellItem`
   - Inputs:
     - SpellData (Type: FSpellData)
     - Mentor (Type: AMentorBase, Object Reference)
     - Player (Type: ANinjaWizardCharacter, Object Reference)

5. In Event Graph for `InitializeSpellItem`:

```
Event InitializeSpellItem
├─ Break FSpellData (SpellData input)
│  ├─ Spell Name → Convert to Text → Set SpellNameText
│  ├─ Mana Cost → Format Text ("Mana: {0}")
│  │  └─ Set ManaCostText
│  └─ Required Mastery → Format Text ("Req: {0}")
│     └─ Set MasteryText
├─ Set local variable: StoredSpellData = SpellData
├─ Set local variable: StoredMentor = Mentor
└─ Set local variable: StoredPlayer = Player
```

6. Create Event for MainButton **On Clicked**:

```
Event OnClicked (MainButton)
├─ Get StoredMentor
├─ Get StoredPlayer
├─ Get StoredSpellData
└─ Call TeachSpell (on StoredMentor)
   ├─ Player: StoredPlayer
   └─ Spell: StoredSpellData
   └─ After Teaching:
      ├─ Print String ("Learned {SpellName}!")
      └─ Set Button IsEnabled = false (grayed out)
      └─ Set SpellNameText Color = Gray
```

### Step 2: Update Dialogue Widget to Use List

Back in `WBP_Dialogue`, update the design:

1. Add a **Scroll Box** widget below the DialogueText:
   - Name it: `SpellListScrollBox`
   - Make it a variable
   - Set Visibility: Collapsed (default hidden)

2. Add a **Back Button** to return from lists:
   - Name it: `BackButton`
   - Text: "← Back"
   - Make it a variable
   - Set Visibility: Collapsed (default hidden)

### Step 3: Implement Learn Spells Button

```
Event OnClicked (LearnSpellsButton)
├─ Hide dialogue text area
│  └─ Set DialogueText Visibility = Collapsed
├─ Show spell list
│  └─ Set SpellListScrollBox Visibility = Visible
├─ Show back button
│  └─ Set BackButton Visibility = Visible
├─ Hide action buttons
│  ├─ Set LearnSpellsButton Visibility = Collapsed
│  ├─ Set LearnWeaponsButton Visibility = Collapsed
│  ├─ Set ViewQuestsButton Visibility = Collapsed
│  └─ Set ExitButton Visibility = Collapsed
├─ Clear existing list
│  └─ Call ClearChildren (on SpellListScrollBox)
├─ Get CurrentMentor → Get TeachableSpells
└─ ForEachLoop (TeachableSpells)
   └─ Loop Body (for each spell):
      ├─ Create Widget (Class: WBP_SpellListItem)
      ├─ Call InitializeSpellItem (on created widget)
      │  ├─ SpellData: Array Element
      │  ├─ Mentor: CurrentMentor
      │  └─ Player: PlayerCharacter
      └─ Add Child to Scroll Box
         ├─ Parent: SpellListScrollBox
         └─ Content: Created Widget
```

### Step 4: Implement Back Button

```
Event OnClicked (BackButton)
├─ Show dialogue text area
│  └─ Set DialogueText Visibility = Visible
├─ Hide spell list
│  └─ Set SpellListScrollBox Visibility = Collapsed
├─ Hide back button
│  └─ Set BackButton Visibility = Collapsed
├─ Show action buttons
│  ├─ Set LearnSpellsButton Visibility = Visible
│  ├─ Set LearnWeaponsButton Visibility = Visible
│  ├─ Set ViewQuestsButton Visibility = Visible
│  └─ Set ExitButton Visibility = Visible
└─ Clear spell list
   └─ Call ClearChildren (on SpellListScrollBox)
```

### Step 5: Repeat for Weapons

1. Create `WBP_WeaponListItem` (similar to spell item)
2. Add another Scroll Box: `WeaponListScrollBox`
3. Implement LearnWeaponsButton the same way

---

## Advanced Approach - Dynamic Menu with Filtering

This approach adds search/filter capabilities and better UX.

### Step 1: Enhanced Spell List Item

Add to `WBP_SpellListItem`:

1. Add **Border** with hover state
2. Add **Image** icon for spell element
3. Add **Progress Bar** showing if player has mastery requirement

```
Border (Root)
├─ Background Color Binding:
│  └─ Is Hovered? Light Blue : Transparent
└─ Horizontal Box
    ├─ Image - ElementIcon (Size: 48x48)
    ├─ Vertical Box (Fill)
    │  ├─ Text - SpellNameText
    │  ├─ Text - DescriptionText
    │  └─ Progress Bar - MasteryRequirement
    └─ Button - LearnButton
       └─ Text: "Learn"
```

### Step 2: Add Search/Filter to Dialogue

In `WBP_Dialogue`:

1. Add above the Scroll Box:

```
Horizontal Box
├─ Text: "Search:"
├─ Editable Text Box - SearchBox
│  └─ Hint Text: "Filter by name..."
└─ Combo Box - ElementFilter
   └─ Options: "All", "Fire", "Water", etc.
```

2. Create function: `FilterSpells`
   - Inputs:
     - AllSpells (Array of FSpellData)
     - SearchText (Text)
     - ElementFilter (EMagicElement)
   - Output:
     - FilteredSpells (Array of FSpellData)

3. Implementation:

```
Function FilterSpells
├─ ForEachLoop (AllSpells)
│  └─ Loop Body:
│     ├─ Get Spell Name from Array Element
│     ├─ Convert to String
│     ├─ Contains (Search for SearchText)
│     └─ Branch (Contains?)
│        └─ True:
│           ├─ Check Element matches filter
│           └─ If matches:
│              └─ Add to FilteredSpells array
└─ Return FilteredSpells
```

4. Bind to Search Box:

```
Event OnTextChanged (SearchBox)
├─ Get SearchBox Text
├─ Get ElementFilter Selection
├─ Call FilterSpells
└─ Refresh SpellListScrollBox with filtered results
```

### Step 3: Add Sort Options

```
Combo Box - SortBy
└─ Options: "Name", "Mana Cost", "Required Mastery"

Function SortSpells
├─ Input: Spells Array
├─ Input: Sort Type (enum)
└─ Use Sort Array node with custom predicate
   ├─ By Name: Compare SpellName alphabetically
   ├─ By Mana: Compare ManaCost values
   └─ By Mastery: Compare RequiredMastery values
```

---

## Quick Reference - Common Blueprint Nodes

### Getting Data from Structs

```
Break FSpellData
├─ Spell Name (FName)
├─ Element (EMagicElement)
├─ Mana Cost (float)
├─ Base Damage (float)
├─ Cast Time (float)
├─ Required Mastery (int32)
└─ Description (FText)
```

### Array Operations

- **For Each Loop**: Iterate through array
- **Get (Array)**: Get item at index
- **Add**: Add item to end of array
- **Clear**: Remove all items
- **Length**: Get array size
- **Find**: Search for item in array

### Widget Operations

- **Create Widget**: Spawn new widget instance
  - Class: Select your widget BP
  - Owning Player: Get Owning Player
- **Add Child to Scroll Box**: Add widget to scroll box
- **Clear Children**: Remove all child widgets
- **Remove from Parent**: Delete widget from UI

### Text Operations

- **Format Text**: Create formatted string
  - Example: `"Mana: {0}"` with Mana Cost connected
- **To Text**: Convert other types to Text
- **Append**: Combine text strings

---

## Testing Tips

1. **Add Debug Prints**:
   ```
   After each major step, add:
   Print String → "Step X completed"
   ```

2. **Check Array Sizes**:
   ```
   Get TeachableSpells
   → Get Length
   → Print String ("Found {0} spells")
   ```

3. **Verify Variables**:
   ```
   Is Valid? node before using any object reference
   ```

4. **Test with Empty Lists**:
   Make sure your code handles mentors with no spells/weapons/quests

---

## Example: Complete Learn Spells Implementation

Here's a complete, copy-paste-ready implementation using the intermediate approach:

### In WBP_SpellListItem Graph:

**Variables** (Add these):
- StoredSpellData (Type: FSpellData)
- StoredMentor (Type: AMentorBase, Object Reference)
- StoredPlayer (Type: ANinjaWizardCharacter, Object Reference)

**Custom Event: InitializeSpellItem**:
```
1. Add Custom Event node: InitializeSpellItem
2. Add input pins:
   - SpellData (FSpellData)
   - Mentor (AMentorBase reference)
   - Player (ANinjaWizardCharacter reference)
3. Connect:
   - Break SpellData → Get Spell Name → To Text (Conversion) → Set SpellNameText (Text)
   - Break SpellData → Get Mana Cost → Format Text ("Mana: {0}") → Set ManaCostText (Text)
   - Break SpellData → Get Required Mastery → Format Text ("Req: {0}") → Set MasteryText (Text)
   - Set StoredSpellData = SpellData
   - Set StoredMentor = Mentor
   - Set StoredPlayer = Player
```

**MainButton OnClicked Event**:
```
1. Get StoredMentor
2. Is Valid? (branch)
3. True path:
   - Get StoredPlayer
   - Get StoredSpellData
   - Call TeachSpell (on StoredMentor)
     • Player = StoredPlayer
     • Spell = StoredSpellData
   - Set MainButton IsEnabled = false
   - Print String "Spell learned!"
```

### In WBP_Dialogue Graph:

**LearnSpellsButton OnClicked Event**:
```
1. Set DialogueText Visibility = Collapsed
2. Set SpellListScrollBox Visibility = Visible
3. Set BackButton Visibility = Visible
4. Set LearnSpellsButton Visibility = Collapsed
5. Set LearnWeaponsButton Visibility = Collapsed
6. Set ViewQuestsButton Visibility = Collapsed
7. Set ExitButton Visibility = Collapsed
8. SpellListScrollBox → Clear Children
9. Get CurrentMentor → Get TeachableSpells
10. For Each Loop (connect TeachableSpells)
    Loop Body:
    - Create Widget (Class = WBP_SpellListItem)
    - Call InitializeSpellItem on created widget
      • SpellData = Array Element (from loop)
      • Mentor = CurrentMentor
      • Player = PlayerCharacter
    - Add Child to Scroll Box
      • Parent = SpellListScrollBox
      • Content = Created Widget
```

**BackButton OnClicked Event**:
```
1. Set DialogueText Visibility = Visible
2. Set SpellListScrollBox Visibility = Collapsed
3. Set BackButton Visibility = Collapsed
4. Set LearnSpellsButton Visibility = Visible
5. Set LearnWeaponsButton Visibility = Visible
6. Set ViewQuestsButton Visibility = Visible
7. Set ExitButton Visibility = Visible
8. SpellListScrollBox → Clear Children
```

---

## Common Issues & Solutions

**Issue**: "Can't find TeachableSpells"
- **Solution**: Make sure you're calling it on CurrentMentor, not CurrentPlayer

**Issue**: "Widgets not appearing in scroll box"
- **Solution**: Check that you're calling AddChildToScrollBox, not AddToViewport

**Issue**: "Buttons not clickable"
- **Solution**: Make sure parent widgets have "Is Focusable" = false, only buttons should be focusable

**Issue**: "List shows old items"
- **Solution**: Call ClearChildren before rebuilding the list

**Issue**: "Can't click items after learning"
- **Solution**: Make sure you're setting IsEnabled = false, not Visibility = Collapsed

---

## Next Steps

1. Start with the **Simple Approach** to get it working
2. Add the **Intermediate Approach** for better UX
3. Enhance with the **Advanced Approach** features as needed

The simple approach will get you running in 10-15 minutes!

Let me know which approach you'd like more details on! 🎮
