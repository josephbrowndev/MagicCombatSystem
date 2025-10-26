// Skill Tree Component - Manages element and weapon skill trees

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProgressionTypes.h"
#include "SkillTreeComponent.generated.h"

class ANinjaWizardCharacter;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ELEMENTALDANGER_API USkillTreeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkillTreeComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ============================================
	// Skill Points
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	int32 AvailableSkillPoints = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree")
	int32 TotalSkillPointsEarned = 0;

	// ============================================
	// Skill Trees
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree|Element")
	TMap<EMagicElement, FSkillTreeData> ElementalTrees;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill Tree|Weapon")
	TMap<EWeaponStyle, FSkillTreeData> WeaponTrees;

	// ============================================
	// Skill Tree Management
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void DiscoverSkillTree(ESkillTreeType TreeType);

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool IsSkillTreeDiscovered(ESkillTreeType TreeType) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void DiscoverSkillNode(ESkillTreeType TreeType, FName NodeID);

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool IsSkillNodeDiscovered(ESkillTreeType TreeType, FName NodeID) const;

	// ============================================
	// Skill Unlocking
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool CanUnlockSkill(ESkillTreeType TreeType, FName NodeID) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool UnlockSkill(ESkillTreeType TreeType, FName NodeID);

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	bool IsSkillUnlocked(ESkillTreeType TreeType, FName NodeID) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void AddSkillPoints(int32 Points);

	// ============================================
	// Skill Effects
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	TArray<FSkillNodeData> GetActiveSkills() const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	TArray<FSkillNodeData> GetPassiveSkills() const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	float GetSkillEffectValue(ESkillTreeType TreeType, FName NodeID) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	TArray<FSkillNodeData> GetSkillsWithEffect(ESkillEffectType EffectType) const;

	// ============================================
	// Return Weapon Skill
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Skill Tree|Special")
	bool HasReturnWeaponSkill(EWeaponStyle WeaponStyle) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree|Special")
	void ActivateReturnWeapon(EWeaponStyle WeaponStyle);

	// ============================================
	// Tree Query Functions
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	FSkillTreeData* GetSkillTree(ESkillTreeType TreeType);

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	FSkillNodeData* GetSkillNode(ESkillTreeType TreeType, FName NodeID);

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	TArray<FSkillNodeData> GetAvailableSkillsForTree(ESkillTreeType TreeType) const;

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	int32 GetTotalPointsSpent(ESkillTreeType TreeType) const;

	// ============================================
	// Skill Tree Reset
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void ResetSkillTree(ESkillTreeType TreeType);

	UFUNCTION(BlueprintCallable, Category = "Skill Tree")
	void ResetAllSkillTrees();

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill Tree|Events")
	void OnSkillTreeDiscovered(ESkillTreeType TreeType);

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill Tree|Events")
	void OnSkillNodeDiscovered(ESkillTreeType TreeType, FName NodeID);

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill Tree|Events")
	void OnSkillUnlocked(ESkillTreeType TreeType, const FSkillNodeData& SkillNode);

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill Tree|Events")
	void OnSkillPointsGained(int32 Points);

	UFUNCTION(BlueprintImplementableEvent, Category = "Skill Tree|Events")
	void OnReturnWeaponActivated(EWeaponStyle WeaponStyle);

protected:
	UPROPERTY()
	ANinjaWizardCharacter* OwnerCharacter;

	// Helper functions
	void InitializeSkillTrees();
	void CreateDefaultElementalTree(EMagicElement Element);
	void CreateDefaultWeaponTree(EWeaponStyle Weapon);
	bool CheckPrerequisites(const FSkillNodeData& Node, const FSkillTreeData& Tree) const;
	void ApplySkillEffects(const FSkillNodeData& SkillNode);
	ESkillTreeType GetTreeTypeFromElement(EMagicElement Element) const;
	ESkillTreeType GetTreeTypeFromWeapon(EWeaponStyle Weapon) const;
};
