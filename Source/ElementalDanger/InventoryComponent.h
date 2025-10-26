// Inventory Component - Manages player's unlimited inventory and equipment

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionTypes.h"
#include "InventoryComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Inventory Data
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FItemData> InventoryItems;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Weapons")
	TArray<FWeaponData> WeaponInventory;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Armor")
	TArray<FArmorData> ArmorInventory;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory|Potions")
	TArray<FPotionData> PotionInventory;

	// ============================================
	// Equipment Loadout (What's Currently Equipped)
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	FEquipmentLoadout CurrentLoadout;

	// ============================================
	// Item Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(const FItemData& Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddWeapon(const FWeaponData& Weapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddArmor(const FArmorData& Armor);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddPotion(const FPotionData& Potion);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FName ItemID, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemID) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetItemQuantity(FName ItemID) const;

	// C++ only - returns pointer to item data
	FItemData* GetItemByID(FName ItemID);

	// ============================================
	// Equipment Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipWeapon(FName WeaponID, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipWeapon(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipArmor(FName ArmorID);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipArmor(EArmorSlot ArmorSlot);

	// C++ only - returns pointer to equipped weapon
	FWeaponData* GetEquippedWeapon(int32 SlotIndex);

	// C++ only - returns pointer to equipped armor
	FArmorData* GetEquippedArmor(EArmorSlot ArmorSlot);

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool IsWeaponEquipped(FName WeaponID) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool IsArmorEquipped(FName ArmorID) const;

	// ============================================
	// Weapon Switching
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Equipment|Weapons")
	void SwitchToWeaponSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Equipment|Weapons")
	int32 GetCurrentWeaponSlot() const { return CurrentWeaponSlot; }

	// C++ only - returns pointer to current weapon
	FWeaponData* GetCurrentWeapon();

	// ============================================
	// Inventory Query Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Inventory|Query")
	TArray<FWeaponData> GetWeaponsByStyle(EWeaponStyle WeaponStyle) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Query")
	TArray<FArmorData> GetArmorBySlot(EArmorSlot ArmorSlot) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Query")
	TArray<FItemData> GetItemsByRarity(EItemRarity Rarity) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Query")
	TArray<FPotionData> GetAvailablePotions() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Query")
	int32 GetTotalInventoryItemCount() const;

	// ============================================
	// Potion Usage
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Inventory|Potions")
	bool CanUsePotion(FName PotionID, float CurrentHealthPercent) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory|Potions")
	bool UsePotion(FName PotionID);

	// ============================================
	// Stat Calculation from Equipment
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Equipment|Stats")
	float GetTotalEquipmentDefense() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment|Stats")
	float GetTotalEquipmentAttackBonus() const;

	UFUNCTION(BlueprintCallable, Category = "Equipment|Stats")
	float GetEquipmentStatModifier(FName StatName) const;

	// ============================================
	// Sorting and Filtering
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Inventory|Sort")
	void SortInventoryByRarity();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Sort")
	void SortInventoryByName();

	UFUNCTION(BlueprintCallable, Category = "Inventory|Sort")
	void SortInventoryByType();

	// ============================================
	// C++ Helper Functions (Public for component access)
	// ============================================

	FWeaponData* FindWeaponByID(FName WeaponID);
	FArmorData* FindArmorByID(FName ArmorID);
	FPotionData* FindPotionByID(FName PotionID);

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnItemAdded(const FItemData& Item);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnItemRemoved(FName ItemID, int32 Quantity);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnWeaponEquipped(const FWeaponData& Weapon, int32 SlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnWeaponUnequipped(int32 SlotIndex);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnArmorEquipped(const FArmorData& Armor);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnArmorUnequipped(EArmorSlot ArmorSlot);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnWeaponSwitched(int32 NewSlotIndex, const FWeaponData& NewWeapon);

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory|Events")
	void OnPotionUsed(const FPotionData& Potion);

protected:
	// ============================================
	// Internal Helper Functions
	// ============================================

	void UpdateEquipmentStats();

	// ============================================
	// Current Weapon Slot
	// ============================================

	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	int32 CurrentWeaponSlot = 0;

	// ============================================
	// Configuration
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config")
	bool bAutoStackItems = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config")
	bool bAutoSortOnAdd = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Config")
	float MinHealthPercentForPotions = 0.1f; // 10% minimum
};
