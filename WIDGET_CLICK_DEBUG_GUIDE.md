# Widget Click Events Not Working - Debug Guide

## Common Issues & Solutions

### Issue 1: Button "Is Enabled" is False

**Check This First!**

1. Open **WBP_RadialMagicMenu** in Designer
2. Click on **Button_Fire** (or any button)
3. Look in **Details Panel** (right side)
4. Find **Behavior** section
5. Check if **Is Enabled** is ✅ **TRUE**

**If it's FALSE**:
- ✅ Check the box to enable it
- Compile and test

**Do this for ALL 8 buttons!**

---

### Issue 2: Button "Visibility" is Wrong

1. Select each button
2. In **Details → Behavior**
3. **Visibility** should be: **Visible** (NOT "Hidden" or "Not Hit-Testable")

If it says anything else, change to **Visible**.

---

### Issue 3: OnClicked Event Not Properly Bound

**Verify in Graph Tab**:

1. Switch to **Graph** tab
2. Look at your event nodes
3. Each button should have a chain like this:

```
OnClicked (Button_Fire)
├─ Get Player Character
├─ Cast To BP_NinjaWizardCharacter
├─ Set Selected Element (Fire)
└─ Remove From Parent
```

**Common mistake**: The cast fails because character class is wrong.

**Fix**:
- Make sure you're casting to the EXACT class name
- If your character is `BP_NinjaWizardCharacter`, type exactly that
- Case-sensitive!

---

### Issue 4: Input Mode Not Set to UI

The widget needs to capture mouse input!

**In Event Construct**:
```
Event Construct
├─ Get Player Controller
│   └─ Set Show Mouse Cursor (TRUE) ← Must be TRUE
└─ Set Input Mode UI Only ← THIS IS CRITICAL
    ├─ Player Controller: [From Get Player Controller]
    └─ Widget to Focus: Self
```

**Checklist**:
- [ ] Set Show Mouse Cursor is checked TRUE
- [ ] Set Input Mode UI Only is called
- [ ] Widget to Focus is set to **Self** (drag from the left sidebar)

---

### Issue 5: Canvas Panel Blocking Clicks

**Check Z-Order**:

1. In **Hierarchy** (Designer Tab, left side)
2. Buttons should be BELOW (later in list) than background images
3. Drag buttons to be at the bottom of the Overlay

**Hierarchy should be**:
```
Overlay
├─ Image (background) ← First
├─ Image (center icon) ← Second
├─ Button_Fire ← All buttons AFTER images
├─ Button_Water
├─ Button_Earth
[etc...]
```

---

### Issue 6: Mouse Cursor Not Visible

If you can't see the cursor, you can't click!

**Test**:
1. PIE (Play)
2. Hold Q
3. **Do you see mouse cursor?**

**If NO**:
- Event Construct needs "Set Show Mouse Cursor = TRUE"
- Make sure it's called BEFORE adding widget to viewport

**In Character Blueprint** (where you open the menu):
```
IA_OpenMagicMenu (Started)
├─ Create Widget (WBP_RadialMagicMenu)
├─ Add to Viewport
└─ Get Player Controller
    └─ Set Show Mouse Cursor (TRUE) ← Add this if missing!
```

---

### Issue 7: Widget Not Actually Added to Viewport

**Character Blueprint Check**:

```
IA_OpenMagicMenu (Started)
├─ Create Widget (WBP_RadialMagicMenu)
│   └─ Class: WBP_RadialMagicMenu ← Is this set?
└─ Add to Viewport ← Is this called?
```

**Test**: Add a **Print String** node after "Add to Viewport":
```
Add to Viewport
└─ Print String ("Menu Added!")
```

If you don't see "Menu Added!" in game, the widget isn't being created.

---

### Issue 8: Buttons Are Too Small or Overlapping

**Check Button Size**:
1. Select Button_Fire
2. Details → Slot (Canvas Panel Slot)
3. **Size X**: Should be 80
4. **Size Y**: Should be 80

If they're really small (like 10x10), you won't be able to click them easily.

---

### Issue 9: No Event Graph Code

**This is the most common issue!**

**How to properly create OnClicked events**:

1. **Switch to Designer Tab**
2. **Click Button_Fire in Hierarchy**
3. **In Details panel (right), scroll down to Events section**
4. **Find "On Clicked"**
5. **Click the green ➕ button next to "On Clicked"**
6. **This auto-creates the event node in Graph and switches you there**

**For each of the 8 buttons**, do this:

1. Designer Tab → Select Button
2. Details → Events → On Clicked → Click ➕
3. It creates the event automatically
4. Add your logic

---

## Step-by-Step: Re-Creating Click Events

Let me walk you through ONE button completely:

### Button_Fire - Complete Setup

#### Step 1: Designer Tab
```
1. Click "Button_Fire" in Hierarchy
2. Details Panel (right side):
   - Behavior → Is Enabled: ✅ TRUE
   - Behavior → Visibility: Visible
   - Slot → Size X: 80, Size Y: 80
```

#### Step 2: Create Event
```
3. Still in Details Panel, scroll to "Events" section
4. Find "On Clicked"
5. Click the green ➕ button
6. You're now in Graph Tab with new event node
```

#### Step 3: Add Logic (in Graph Tab)
```
OnClicked (Button_Fire)  ← This was auto-created
│
├─ Get Player Character
│   │
│   └─ Cast to BP_NinjaWizardCharacter
│       │
│       ├─ Set Selected Element
│       │   └─ Element: Fire (select from dropdown)
│       │
│       └─ Remove from Parent
│           └─ Target: Self
```

**How to create these nodes**:

1. **Get Player Character**:
   - Drag from OnClicked execute pin (white)
   - Type "Get Player Character"
   - Press Enter

2. **Cast to BP_NinjaWizardCharacter**:
   - Drag from blue "Return Value" of Get Player Character
   - Type "Cast to BP_NinjaWizardCharacter"
   - Press Enter

3. **Set Selected Element**:
   - Drag from blue "As BP Ninja Wizard Character" pin
   - Type "Set Selected Element"
   - Press Enter
   - In the dropdown that appears, select **Fire**

4. **Remove from Parent**:
   - Drag from white execute pin of "Set Selected Element"
   - Type "Remove from Parent"
   - Drag from "Target" pin to **Self** (from left sidebar under Variables)

#### Step 4: Compile & Test
```
1. Click "Compile" button (top left)
2. Click "Save"
3. Close widget editor
4. PIE
5. Hold Q
6. Click Fire button
7. Menu should close and Fire should be selected
```

---

## Quick Test: Is Widget Receiving Input?

Add this to **Event Construct**:

```
Event Construct
├─ [Your existing code]
└─ Print String ("Widget Constructed!")
```

Then add this to **Button_Fire OnClicked**:
```
OnClicked (Button_Fire)
├─ Print String ("Fire Button Clicked!")
└─ [Rest of your code]
```

**Test**:
1. PIE
2. Hold Q → Should see "Widget Constructed!"
3. Click Fire button → Should see "Fire Button Clicked!"

**If you DON'T see "Fire Button Clicked!"**:
- Button isn't receiving clicks
- Check Issues 1-6 above

**If you DO see "Fire Button Clicked!" but nothing happens**:
- The event is working but the logic is broken
- Check the Cast to BP_NinjaWizardCharacter
- Add more Print String nodes after each step to find where it breaks

---

## Nuclear Option: Start Fresh

If nothing works, try this:

### Create a Simple Test Widget

1. **New Widget**: WBP_TestClick
2. **Add just ONE button**
3. **Add this code**:

```
Event Construct
└─ Print String ("Test Widget Created")

OnClicked (Button_0)
└─ Print String ("Button Clicked!")
```

4. **In Character Blueprint**:
```
IA_OpenMagicMenu (Started)
├─ Create Widget (WBP_TestClick) ← Use test widget
├─ Add to Viewport
└─ Set Input Mode UI Only
```

5. **Test**: If this works, your main widget has an issue. If this doesn't work, it's an input setup problem.

---

## Checklist Before Asking For More Help

Go through this checklist:

- [ ] All buttons have "Is Enabled" = TRUE
- [ ] All buttons have "Visibility" = Visible
- [ ] Event Construct calls "Set Input Mode UI Only"
- [ ] Event Construct calls "Set Show Mouse Cursor" = TRUE
- [ ] Each button has green ➕ event created in Designer
- [ ] Each OnClicked event has logic in Graph
- [ ] Widget is added to viewport in Character BP
- [ ] Print String shows "Widget Constructed!" when opening menu
- [ ] Print String shows "Button Clicked!" when clicking button
- [ ] Mouse cursor is visible when menu is open
- [ ] Buttons are visible and not overlapped by other widgets

---

## Common Unreal Engine Gotchas

### Gotcha 1: Input Mode
If Input Mode is "Game Only", widgets don't receive clicks!
**Must be**: "UI Only" or "Game and UI"

### Gotcha 2: Widget Z-Order
Widgets added later are on top. If you add another widget after the menu, it might be blocking clicks.

### Gotcha 3: Focus
Sometimes widgets need focus to receive input:
```
Add to Viewport
└─ Set Keyboard Focus
    └─ Widget: [Your Menu Widget Reference]
```

### Gotcha 4: Button Visibility
"Not Hit-Testable" means visible but can't be clicked!
Always use "Visible" for buttons.

---

## What to Check in Editor Right Now

1. **Open WBP_RadialMagicMenu**
2. **Designer Tab**
3. **Click Button_Fire**
4. **Look at Details Panel** → Take a screenshot if still broken
5. **Graph Tab** → Take a screenshot of OnClicked events
6. **Check if "OnClicked (Button_Fire)" event node exists**

If you can provide screenshots of:
- Button Details panel (Behavior section)
- Graph showing OnClicked event nodes

I can tell you exactly what's wrong!

---

## Still Not Working?

Try this diagnostic:

**In WBP_RadialMagicMenu Event Graph**:

```
Event Construct
├─ Print String ("Menu Created")
├─ Print String ("Input Mode Set")
├─ Delay (0.1 seconds)
└─ Print String ("Menu Ready")

Event Tick
└─ Print String ("Menu Ticking") ← Spammy but proves widget is alive
```

**What you should see**:
- "Menu Created" appears once
- "Input Mode Set" appears once
- "Menu Ready" appears after short delay
- "Menu Ticking" spams every frame (remove after testing)

If you don't see ANY of these, the widget isn't actually being created/displayed.

---

Let me know what you find! I'm here to help debug this. 🔍
