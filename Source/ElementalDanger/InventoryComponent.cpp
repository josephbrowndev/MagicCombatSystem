// Inventory Component Implementation

#include "InventoryComponent.h"
#include "NinjaWizardCharacter.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bAutoStackItems = true;
	bAutoSortOnAdd = false;
	MinHealthPercentForPotions = 0.1f;
	CurrentWeaponSlot = 0;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ============================================
// Item Management
// ============================================

bool UInventoryComponent::AddItem(const FItemData& Item)
{
	// Check if we already have this item and should stack it
	if (bAutoStackItems && Item.bIsStackable)
	{
		for (FItemData& ExistingItem : InventoryItems)
		{
			if (ExistingItem.ItemID == Item.ItemID)
			{
				ExistingItem.StackSize += Item.StackSize;
				OnItemAdded(Item);
				return true;
			}
		}
	}

	// Add as new item
	InventoryItems.Add(Item);
	OnItemAdded(Item);

	if (bAutoSortOnAdd)
	{
		SortInventoryByRarity();
	}

	return true;
}

bool UInventoryComponent::AddWeapon(const FWeaponData& Weapon)
{
	// Check if we already have this exact weapon
	for (const FWeaponData& ExistingWeapon : WeaponInventory)
	{
		if (ExistingWeapon.ItemID == Weapon.ItemID)
		{
			// Weapon already exists, don't add duplicate
			return false;
		}
	}

	WeaponInventory.Add(Weapon);

	// Also add to general inventory
	FItemData ItemData;
	ItemData.ItemID = Weapon.ItemID;
	ItemData.ItemName = Weapon.ItemName;
	ItemData.Description = Weapon.Description;
	ItemData.ItemType = EItemType::Weapon;
	ItemData.Rarity = Weapon.Rarity;
	ItemData.bIsStackable = false;
	ItemData.StackSize = 1;
	ItemData.Value = Weapon.Value;

	AddItem(ItemData);

	return true;
}

bool UInventoryComponent::AddArmor(const FArmorData& Armor)
{
	// Check if we already have this exact armor piece
	for (const FArmorData& ExistingArmor : ArmorInventory)
	{
		if (ExistingArmor.ItemID == Armor.ItemID)
		{
			// Armor already exists, don't add duplicate
			return false;
		}
	}

	ArmorInventory.Add(Armor);

	// Also add to general inventory
	FItemData ItemData;
	ItemData.ItemID = Armor.ItemID;
	ItemData.ItemName = Armor.ItemName;
	ItemData.Description = Armor.Description;
	ItemData.ItemType = EItemType::Armor;
	ItemData.Rarity = Armor.Rarity;
	ItemData.bIsStackable = false;
	ItemData.StackSize = 1;
	ItemData.Value = Armor.Value;

	AddItem(ItemData);

	return true;
}

bool UInventoryComponent::AddPotion(const FPotionData& Potion)
{
	// Check if we already have this potion and should stack it
	if (bAutoStackItems)
	{
		for (FPotionData& ExistingPotion : PotionInventory)
		{
			if (ExistingPotion.ItemID == Potion.ItemID)
			{
				ExistingPotion.StackSize += Potion.StackSize;

				// Update in general inventory
				for (FItemData& Item : InventoryItems)
				{
					if (Item.ItemID == Potion.ItemID)
					{
						Item.StackSize += Potion.StackSize;
						OnItemAdded(Item);
						return true;
					}
				}
			}
		}
	}

	PotionInventory.Add(Potion);

	// Also add to general inventory
	FItemData ItemData;
	ItemData.ItemID = Potion.ItemID;
	ItemData.ItemName = Potion.ItemName;
	ItemData.Description = Potion.Description;
	ItemData.ItemType = EItemType::Potion;
	ItemData.Rarity = Potion.Rarity;
	ItemData.bIsStackable = true;
	ItemData.StackSize = Potion.StackSize;
	ItemData.Value = Potion.Value;

	AddItem(ItemData);

	return true;
}

bool UInventoryComponent::RemoveItem(FName ItemID, int32 Quantity)
{
	for (int32 i = InventoryItems.Num() - 1; i >= 0; i--)
	{
		if (InventoryItems[i].ItemID == ItemID)
		{
			if (InventoryItems[i].bIsStackable)
			{
				InventoryItems[i].StackSize -= Quantity;
				if (InventoryItems[i].StackSize <= 0)
				{
					InventoryItems.RemoveAt(i);
				}
			}
			else
			{
				InventoryItems.RemoveAt(i);
			}

			OnItemRemoved(ItemID, Quantity);
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::HasItem(FName ItemID) const
{
	for (const FItemData& Item : InventoryItems)
	{
		if (Item.ItemID == ItemID)
		{
			return true;
		}
	}
	return false;
}

int32 UInventoryComponent::GetItemQuantity(FName ItemID) const
{
	for (const FItemData& Item : InventoryItems)
	{
		if (Item.ItemID == ItemID)
		{
			return Item.StackSize;
		}
	}
	return 0;
}

FItemData* UInventoryComponent::GetItemByID(FName ItemID)
{
	for (FItemData& Item : InventoryItems)
	{
		if (Item.ItemID == ItemID)
		{
			return &Item;
		}
	}
	return nullptr;
}

// ============================================
// Equipment Management
// ============================================

bool UInventoryComponent::EquipWeapon(FName WeaponID, int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= 4) return false;

	FWeaponData* Weapon = FindWeaponByID(WeaponID);
	if (!Weapon) return false;

	// Check if already equipped in another slot
	if (IsWeaponEquipped(WeaponID)) return false;

	// Equip the weapon
	CurrentLoadout.EquippedWeapons[SlotIndex] = *Weapon;
	CurrentLoadout.bWeaponSlotFilled[SlotIndex] = true;

	UpdateEquipmentStats();
	OnWeaponEquipped(*Weapon, SlotIndex);

	return true;
}

bool UInventoryComponent::UnequipWeapon(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= 4) return false;
	if (!CurrentLoadout.bWeaponSlotFilled[SlotIndex]) return false;

	CurrentLoadout.bWeaponSlotFilled[SlotIndex] = false;
	UpdateEquipmentStats();
	OnWeaponUnequipped(SlotIndex);

	return true;
}

bool UInventoryComponent::EquipArmor(FName ArmorID)
{
	FArmorData* Armor = FindArmorByID(ArmorID);
	if (!Armor) return false;

	// Check if slot is already occupied
	EArmorSlot TargetSlot = Armor->ArmorSlot;
	if (CurrentLoadout.bArmorSlotFilled[static_cast<uint8>(TargetSlot)])
	{
		// Unequip current armor in that slot first
		UnequipArmor(TargetSlot);
	}

	// Equip the new armor
	CurrentLoadout.EquippedArmor[static_cast<uint8>(TargetSlot)] = *Armor;
	CurrentLoadout.bArmorSlotFilled[static_cast<uint8>(TargetSlot)] = true;

	UpdateEquipmentStats();
	OnArmorEquipped(*Armor);

	return true;
}

bool UInventoryComponent::UnequipArmor(EArmorSlot ArmorSlot)
{
	uint8 SlotIndex = static_cast<uint8>(ArmorSlot);
	if (!CurrentLoadout.bArmorSlotFilled[SlotIndex]) return false;

	CurrentLoadout.bArmorSlotFilled[SlotIndex] = false;
	UpdateEquipmentStats();
	OnArmorUnequipped(ArmorSlot);

	return true;
}

FWeaponData* UInventoryComponent::GetEquippedWeapon(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= 4) return nullptr;
	if (!CurrentLoadout.bWeaponSlotFilled[SlotIndex]) return nullptr;

	return &CurrentLoadout.EquippedWeapons[SlotIndex];
}

FArmorData* UInventoryComponent::GetEquippedArmor(EArmorSlot ArmorSlot)
{
	uint8 SlotIndex = static_cast<uint8>(ArmorSlot);
	if (!CurrentLoadout.bArmorSlotFilled[SlotIndex]) return nullptr;

	return &CurrentLoadout.EquippedArmor[SlotIndex];
}

bool UInventoryComponent::IsWeaponEquipped(FName WeaponID) const
{
	for (int32 i = 0; i < 4; i++)
	{
		if (CurrentLoadout.bWeaponSlotFilled[i])
		{
			if (CurrentLoadout.EquippedWeapons[i].ItemID == WeaponID)
			{
				return true;
			}
		}
	}
	return false;
}

bool UInventoryComponent::IsArmorEquipped(FName ArmorID) const
{
	for (int32 i = 0; i < 8; i++)
	{
		if (CurrentLoadout.bArmorSlotFilled[i])
		{
			if (CurrentLoadout.EquippedArmor[i].ItemID == ArmorID)
			{
				return true;
			}
		}
	}
	return false;
}

// ============================================
// Weapon Switching
// ============================================

void UInventoryComponent::SwitchToWeaponSlot(int32 SlotIndex)
{
	if (SlotIndex < 0 || SlotIndex >= 4) return;
	if (!CurrentLoadout.bWeaponSlotFilled[SlotIndex]) return;

	CurrentWeaponSlot = SlotIndex;
	OnWeaponSwitched(SlotIndex, CurrentLoadout.EquippedWeapons[SlotIndex]);
}

FWeaponData* UInventoryComponent::GetCurrentWeapon()
{
	return GetEquippedWeapon(CurrentWeaponSlot);
}

// ============================================
// Inventory Query Functions
// ============================================

TArray<FWeaponData> UInventoryComponent::GetWeaponsByStyle(EWeaponStyle WeaponStyle) const
{
	TArray<FWeaponData> MatchingWeapons;

	for (const FWeaponData& Weapon : WeaponInventory)
	{
		if (Weapon.WeaponStyle == WeaponStyle)
		{
			MatchingWeapons.Add(Weapon);
		}
	}

	return MatchingWeapons;
}

TArray<FArmorData> UInventoryComponent::GetArmorBySlot(EArmorSlot ArmorSlot) const
{
	TArray<FArmorData> MatchingArmor;

	for (const FArmorData& Armor : ArmorInventory)
	{
		if (Armor.ArmorSlot == ArmorSlot)
		{
			MatchingArmor.Add(Armor);
		}
	}

	return MatchingArmor;
}

TArray<FItemData> UInventoryComponent::GetItemsByRarity(EItemRarity Rarity) const
{
	TArray<FItemData> MatchingItems;

	for (const FItemData& Item : InventoryItems)
	{
		if (Item.Rarity == Rarity)
		{
			MatchingItems.Add(Item);
		}
	}

	return MatchingItems;
}

TArray<FPotionData> UInventoryComponent::GetAvailablePotions() const
{
	TArray<FPotionData> AvailablePotions;

	for (const FPotionData& Potion : PotionInventory)
	{
		if (Potion.StackSize > 0)
		{
			AvailablePotions.Add(Potion);
		}
	}

	return AvailablePotions;
}

int32 UInventoryComponent::GetTotalInventoryItemCount() const
{
	return InventoryItems.Num();
}

// ============================================
// Potion Usage
// ============================================

bool UInventoryComponent::CanUsePotion(FName PotionID, float CurrentHealthPercent) const
{
	// Check if health is above minimum threshold
	if (CurrentHealthPercent < MinHealthPercentForPotions)
	{
		return false;
	}

	// Check if we have the potion
	for (const FPotionData& Potion : PotionInventory)
	{
		if (Potion.ItemID == PotionID && Potion.StackSize > 0)
		{
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::UsePotion(FName PotionID)
{
	FPotionData* Potion = FindPotionByID(PotionID);
	if (!Potion || Potion->StackSize <= 0) return false;

	// Consume one potion
	Potion->StackSize--;

	// Also update in general inventory
	RemoveItem(PotionID, 1);

	OnPotionUsed(*Potion);

	return true;
}

// ============================================
// Stat Calculation from Equipment
// ============================================

float UInventoryComponent::GetTotalEquipmentDefense() const
{
	float TotalDefense = 0.0f;

	// Sum defense from all equipped armor
	for (int32 i = 0; i < 8; i++)
	{
		if (CurrentLoadout.bArmorSlotFilled[i])
		{
			TotalDefense += CurrentLoadout.EquippedArmor[i].DefenseValue;
		}
	}

	return TotalDefense;
}

float UInventoryComponent::GetTotalEquipmentAttackBonus() const
{
	float TotalAttack = 0.0f;

	// Get attack from current weapon
	if (CurrentLoadout.bWeaponSlotFilled[CurrentWeaponSlot])
	{
		TotalAttack += CurrentLoadout.EquippedWeapons[CurrentWeaponSlot].BaseDamage;
	}

	return TotalAttack;
}

float UInventoryComponent::GetEquipmentStatModifier(FName StatName) const
{
	float TotalModifier = 0.0f;

	// Sum stat modifiers from all equipped items
	for (int32 i = 0; i < 4; i++)
	{
		if (CurrentLoadout.bWeaponSlotFilled[i])
		{
			if (CurrentLoadout.EquippedWeapons[i].StatModifiers.Contains(StatName))
			{
				TotalModifier += CurrentLoadout.EquippedWeapons[i].StatModifiers[StatName];
			}
		}
	}

	for (int32 i = 0; i < 8; i++)
	{
		if (CurrentLoadout.bArmorSlotFilled[i])
		{
			if (CurrentLoadout.EquippedArmor[i].StatModifiers.Contains(StatName))
			{
				TotalModifier += CurrentLoadout.EquippedArmor[i].StatModifiers[StatName];
			}
		}
	}

	return TotalModifier;
}

// ============================================
// Sorting and Filtering
// ============================================

void UInventoryComponent::SortInventoryByRarity()
{
	InventoryItems.Sort([](const FItemData& A, const FItemData& B)
	{
		return static_cast<uint8>(A.Rarity) > static_cast<uint8>(B.Rarity);
	});
}

void UInventoryComponent::SortInventoryByName()
{
	InventoryItems.Sort([](const FItemData& A, const FItemData& B)
	{
		return A.ItemName.CompareTo(B.ItemName) < 0;
	});
}

void UInventoryComponent::SortInventoryByType()
{
	InventoryItems.Sort([](const FItemData& A, const FItemData& B)
	{
		return static_cast<uint8>(A.ItemType) < static_cast<uint8>(B.ItemType);
	});
}

// ============================================
// Helper Functions
// ============================================

FWeaponData* UInventoryComponent::FindWeaponByID(FName WeaponID)
{
	for (FWeaponData& Weapon : WeaponInventory)
	{
		if (Weapon.ItemID == WeaponID)
		{
			return &Weapon;
		}
	}
	return nullptr;
}

FArmorData* UInventoryComponent::FindArmorByID(FName ArmorID)
{
	for (FArmorData& Armor : ArmorInventory)
	{
		if (Armor.ItemID == ArmorID)
		{
			return &Armor;
		}
	}
	return nullptr;
}

FPotionData* UInventoryComponent::FindPotionByID(FName PotionID)
{
	for (FPotionData& Potion : PotionInventory)
	{
		if (Potion.ItemID == PotionID)
		{
			return &Potion;
		}
	}
	return nullptr;
}

void UInventoryComponent::UpdateEquipmentStats()
{
	// Recalculate total equipment stats
	// This will be called whenever equipment changes
	// The actual stat application will be handled by the character
}
