// Skill Tree Component Implementation

#include "SkillTreeComponent.h"
#include "NinjaWizardCharacter.h"

USkillTreeComponent::USkillTreeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AvailableSkillPoints = 0;
	TotalSkillPointsEarned = 0;
}

void USkillTreeComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ANinjaWizardCharacter>(GetOwner());
	InitializeSkillTrees();
}

void USkillTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// ============================================
// Initialization
// ============================================

void USkillTreeComponent::InitializeSkillTrees()
{
	// Initialize elemental trees (all hidden until discovered)
	CreateDefaultElementalTree(EMagicElement::Fire);
	CreateDefaultElementalTree(EMagicElement::Water);
	CreateDefaultElementalTree(EMagicElement::Earth);
	CreateDefaultElementalTree(EMagicElement::Air);
	CreateDefaultElementalTree(EMagicElement::Lightning);
	CreateDefaultElementalTree(EMagicElement::Light);
	CreateDefaultElementalTree(EMagicElement::Dark);
	CreateDefaultElementalTree(EMagicElement::Poison);
	CreateDefaultElementalTree(EMagicElement::Ice);

	// Initialize weapon trees (all hidden until discovered)
	CreateDefaultWeaponTree(EWeaponStyle::TwoHandedSword);
	CreateDefaultWeaponTree(EWeaponStyle::Spear);
	CreateDefaultWeaponTree(EWeaponStyle::Fist);
	CreateDefaultWeaponTree(EWeaponStyle::Bow);
}

void USkillTreeComponent::CreateDefaultElementalTree(EMagicElement Element)
{
	FSkillTreeData NewTree;
	NewTree.TreeType = GetTreeTypeFromElement(Element);
	NewTree.TreeName = FText::FromString(FString::Printf(TEXT("%s Mastery"), *UEnum::GetValueAsString(Element)));
	NewTree.bIsDiscovered = false; // Hidden until player finds element
	NewTree.PointsSpent = 0;

	// Add example skill nodes (you'll want to configure these in Blueprints/data)
	// This is just a template structure

	ElementalTrees.Add(Element, NewTree);
}

void USkillTreeComponent::CreateDefaultWeaponTree(EWeaponStyle Weapon)
{
	FSkillTreeData NewTree;
	NewTree.TreeType = GetTreeTypeFromWeapon(Weapon);
	NewTree.TreeName = FText::FromString(FString::Printf(TEXT("%s Mastery"), *UEnum::GetValueAsString(Weapon)));
	NewTree.bIsDiscovered = false; // Hidden until player finds weapon
	NewTree.PointsSpent = 0;

	// Add Return weapon skill to all weapon trees
	FSkillNodeData ReturnSkill;
	ReturnSkill.NodeID = FName(*FString::Printf(TEXT("Return_%s"), *UEnum::GetValueAsString(Weapon)));
	ReturnSkill.NodeName = FText::FromString(TEXT("Weapon Return"));
	ReturnSkill.Description = FText::FromString(TEXT("Throw your weapon and recall it, damaging enemies in its path"));
	ReturnSkill.NodeType = ESkillNodeType::Active;
	ReturnSkill.EffectType = ESkillEffectType::WeaponReturn;
	ReturnSkill.RequiredLevel = 5;
	ReturnSkill.SkillPointCost = 2;
	ReturnSkill.EffectValue = 100.0f; // Return damage percentage
	ReturnSkill.bIsDiscovered = false;
	ReturnSkill.bIsUnlocked = false;

	NewTree.SkillNodes.Add(ReturnSkill);

	WeaponTrees.Add(Weapon, NewTree);
}

// ============================================
// Skill Tree Discovery
// ============================================

void USkillTreeComponent::DiscoverSkillTree(ESkillTreeType TreeType)
{
	// Find which tree to discover
	for (auto& Elem : ElementalTrees)
	{
		if (GetTreeTypeFromElement(Elem.Key) == TreeType)
		{
			if (!Elem.Value.bIsDiscovered)
			{
				Elem.Value.bIsDiscovered = true;
				OnSkillTreeDiscovered(TreeType);
			}
			return;
		}
	}

	for (auto& Elem : WeaponTrees)
	{
		if (GetTreeTypeFromWeapon(Elem.Key) == TreeType)
		{
			if (!Elem.Value.bIsDiscovered)
			{
				Elem.Value.bIsDiscovered = true;
				OnSkillTreeDiscovered(TreeType);
			}
			return;
		}
	}
}

bool USkillTreeComponent::IsSkillTreeDiscovered(ESkillTreeType TreeType) const
{
	for (const auto& Elem : ElementalTrees)
	{
		if (GetTreeTypeFromElement(Elem.Key) == TreeType)
		{
			return Elem.Value.bIsDiscovered;
		}
	}

	for (const auto& Elem : WeaponTrees)
	{
		if (GetTreeTypeFromWeapon(Elem.Key) == TreeType)
		{
			return Elem.Value.bIsDiscovered;
		}
	}

	return false;
}

void USkillTreeComponent::DiscoverSkillNode(ESkillTreeType TreeType, FName NodeID)
{
	FSkillNodeData* Node = GetSkillNode(TreeType, NodeID);
	if (Node && !Node->bIsDiscovered)
	{
		Node->bIsDiscovered = true;
		OnSkillNodeDiscovered(TreeType, NodeID);
	}
}

bool USkillTreeComponent::IsSkillNodeDiscovered(ESkillTreeType TreeType, FName NodeID) const
{
	// This is a const method, so we need const_cast for the helper
	USkillTreeComponent* MutableThis = const_cast<USkillTreeComponent*>(this);
	FSkillNodeData* Node = MutableThis->GetSkillNode(TreeType, NodeID);
	return Node ? Node->bIsDiscovered : false;
}

// ============================================
// Skill Unlocking
// ============================================

bool USkillTreeComponent::CanUnlockSkill(ESkillTreeType TreeType, FName NodeID) const
{
	FSkillTreeData* Tree = const_cast<USkillTreeComponent*>(this)->GetSkillTree(TreeType);
	if (!Tree || !Tree->bIsDiscovered) return false;

	FSkillNodeData* Node = const_cast<USkillTreeComponent*>(this)->GetSkillNode(TreeType, NodeID);
	if (!Node || !Node->bIsDiscovered || Node->bIsUnlocked) return false;

	// Check skill points
	if (AvailableSkillPoints < Node->SkillPointCost) return false;

	// Check level requirement
	if (OwnerCharacter)
	{
		// You'll need to add level tracking to NinjaWizardCharacter
		// For now, assume requirement is met
	}

	// Check prerequisites
	return CheckPrerequisites(*Node, *Tree);
}

bool USkillTreeComponent::UnlockSkill(ESkillTreeType TreeType, FName NodeID)
{
	if (!CanUnlockSkill(TreeType, NodeID)) return false;

	FSkillTreeData* Tree = GetSkillTree(TreeType);
	FSkillNodeData* Node = GetSkillNode(TreeType, NodeID);

	if (!Tree || !Node) return false;

	// Spend skill points
	AvailableSkillPoints -= Node->SkillPointCost;
	Tree->PointsSpent += Node->SkillPointCost;

	// Unlock the skill
	Node->bIsUnlocked = true;

	// Apply skill effects
	ApplySkillEffects(*Node);

	OnSkillUnlocked(TreeType, *Node);

	return true;
}

bool USkillTreeComponent::IsSkillUnlocked(ESkillTreeType TreeType, FName NodeID) const
{
	FSkillNodeData* Node = const_cast<USkillTreeComponent*>(this)->GetSkillNode(TreeType, NodeID);
	return Node ? Node->bIsUnlocked : false;
}

void USkillTreeComponent::AddSkillPoints(int32 Points)
{
	AvailableSkillPoints += Points;
	TotalSkillPointsEarned += Points;
	OnSkillPointsGained(Points);
}

// ============================================
// Skill Effects
// ============================================

TArray<FSkillNodeData> USkillTreeComponent::GetActiveSkills() const
{
	TArray<FSkillNodeData> ActiveSkills;

	for (const auto& TreePair : ElementalTrees)
	{
		for (const FSkillNodeData& Node : TreePair.Value.SkillNodes)
		{
			if (Node.bIsUnlocked && Node.NodeType == ESkillNodeType::Active)
			{
				ActiveSkills.Add(Node);
			}
		}
	}

	for (const auto& TreePair : WeaponTrees)
	{
		for (const FSkillNodeData& Node : TreePair.Value.SkillNodes)
		{
			if (Node.bIsUnlocked && Node.NodeType == ESkillNodeType::Active)
			{
				ActiveSkills.Add(Node);
			}
		}
	}

	return ActiveSkills;
}

TArray<FSkillNodeData> USkillTreeComponent::GetPassiveSkills() const
{
	TArray<FSkillNodeData> PassiveSkills;

	for (const auto& TreePair : ElementalTrees)
	{
		for (const FSkillNodeData& Node : TreePair.Value.SkillNodes)
		{
			if (Node.bIsUnlocked && Node.NodeType == ESkillNodeType::Passive)
			{
				PassiveSkills.Add(Node);
			}
		}
	}

	for (const auto& TreePair : WeaponTrees)
	{
		for (const FSkillNodeData& Node : TreePair.Value.SkillNodes)
		{
			if (Node.bIsUnlocked && Node.NodeType == ESkillNodeType::Passive)
			{
				PassiveSkills.Add(Node);
			}
		}
	}

	return PassiveSkills;
}

float USkillTreeComponent::GetSkillEffectValue(ESkillTreeType TreeType, FName NodeID) const
{
	FSkillNodeData* Node = const_cast<USkillTreeComponent*>(this)->GetSkillNode(TreeType, NodeID);
	return Node ? Node->EffectValue : 0.0f;
}

TArray<FSkillNodeData> USkillTreeComponent::GetSkillsWithEffect(ESkillEffectType EffectType) const
{
	TArray<FSkillNodeData> MatchingSkills;

	for (const auto& TreePair : ElementalTrees)
	{
		for (const FSkillNodeData& Node : TreePair.Value.SkillNodes)
		{
			if (Node.bIsUnlocked && Node.EffectType == EffectType)
			{
				MatchingSkills.Add(Node);
			}
		}
	}

	for (const auto& TreePair : WeaponTrees)
	{
		for (const FSkillNodeData& Node : TreePair.Value.SkillNodes)
		{
			if (Node.bIsUnlocked && Node.EffectType == EffectType)
			{
				MatchingSkills.Add(Node);
			}
		}
	}

	return MatchingSkills;
}

// ============================================
// Return Weapon Skill
// ============================================

bool USkillTreeComponent::HasReturnWeaponSkill(EWeaponStyle WeaponStyle) const
{
	if (!WeaponTrees.Contains(WeaponStyle)) return false;

	const FSkillTreeData& Tree = WeaponTrees[WeaponStyle];
	for (const FSkillNodeData& Node : Tree.SkillNodes)
	{
		if (Node.EffectType == ESkillEffectType::WeaponReturn && Node.bIsUnlocked)
		{
			return true;
		}
	}

	return false;
}

void USkillTreeComponent::ActivateReturnWeapon(EWeaponStyle WeaponStyle)
{
	if (HasReturnWeaponSkill(WeaponStyle))
	{
		OnReturnWeaponActivated(WeaponStyle);
		// Actual weapon return logic will be in WeaponComponent or character
	}
}

// ============================================
// Tree Query Functions
// ============================================

FSkillTreeData* USkillTreeComponent::GetSkillTree(ESkillTreeType TreeType)
{
	for (auto& Elem : ElementalTrees)
	{
		if (GetTreeTypeFromElement(Elem.Key) == TreeType)
		{
			return &Elem.Value;
		}
	}

	for (auto& Elem : WeaponTrees)
	{
		if (GetTreeTypeFromWeapon(Elem.Key) == TreeType)
		{
			return &Elem.Value;
		}
	}

	return nullptr;
}

FSkillNodeData* USkillTreeComponent::GetSkillNode(ESkillTreeType TreeType, FName NodeID)
{
	FSkillTreeData* Tree = GetSkillTree(TreeType);
	if (!Tree) return nullptr;

	for (FSkillNodeData& Node : Tree->SkillNodes)
	{
		if (Node.NodeID == NodeID)
		{
			return &Node;
		}
	}

	return nullptr;
}

TArray<FSkillNodeData> USkillTreeComponent::GetAvailableSkillsForTree(ESkillTreeType TreeType) const
{
	TArray<FSkillNodeData> AvailableSkills;

	FSkillTreeData* Tree = const_cast<USkillTreeComponent*>(this)->GetSkillTree(TreeType);
	if (!Tree || !Tree->bIsDiscovered) return AvailableSkills;

	for (const FSkillNodeData& Node : Tree->SkillNodes)
	{
		if (Node.bIsDiscovered && !Node.bIsUnlocked)
		{
			if (const_cast<USkillTreeComponent*>(this)->CheckPrerequisites(Node, *Tree))
			{
				AvailableSkills.Add(Node);
			}
		}
	}

	return AvailableSkills;
}

int32 USkillTreeComponent::GetTotalPointsSpent(ESkillTreeType TreeType) const
{
	FSkillTreeData* Tree = const_cast<USkillTreeComponent*>(this)->GetSkillTree(TreeType);
	return Tree ? Tree->PointsSpent : 0;
}

// ============================================
// Skill Tree Reset
// ============================================

void USkillTreeComponent::ResetSkillTree(ESkillTreeType TreeType)
{
	FSkillTreeData* Tree = GetSkillTree(TreeType);
	if (!Tree) return;

	// Refund skill points
	AvailableSkillPoints += Tree->PointsSpent;

	// Reset all nodes
	for (FSkillNodeData& Node : Tree->SkillNodes)
	{
		Node.bIsUnlocked = false;
	}

	Tree->PointsSpent = 0;
}

void USkillTreeComponent::ResetAllSkillTrees()
{
	for (auto& Elem : ElementalTrees)
	{
		ResetSkillTree(GetTreeTypeFromElement(Elem.Key));
	}

	for (auto& Elem : WeaponTrees)
	{
		ResetSkillTree(GetTreeTypeFromWeapon(Elem.Key));
	}
}

// ============================================
// Helper Functions
// ============================================

bool USkillTreeComponent::CheckPrerequisites(const FSkillNodeData& Node, const FSkillTreeData& Tree) const
{
	if (Node.PrerequisiteNodes.Num() == 0) return true;

	for (const FName& PrereqID : Node.PrerequisiteNodes)
	{
		bool bPrereqMet = false;
		for (const FSkillNodeData& OtherNode : Tree.SkillNodes)
		{
			if (OtherNode.NodeID == PrereqID && OtherNode.bIsUnlocked)
			{
				bPrereqMet = true;
				break;
			}
		}

		if (!bPrereqMet) return false;
	}

	return true;
}

void USkillTreeComponent::ApplySkillEffects(const FSkillNodeData& SkillNode)
{
	// Apply skill effects to character
	// This will be expanded based on EffectType
	// For now, just fire the event
}

ESkillTreeType USkillTreeComponent::GetTreeTypeFromElement(EMagicElement Element) const
{
	switch (Element)
	{
		case EMagicElement::Fire: return ESkillTreeType::FireTree;
		case EMagicElement::Water: return ESkillTreeType::WaterTree;
		case EMagicElement::Earth: return ESkillTreeType::EarthTree;
		case EMagicElement::Air: return ESkillTreeType::AirTree;
		case EMagicElement::Lightning: return ESkillTreeType::LightningTree;
		case EMagicElement::Light: return ESkillTreeType::LightTree;
		case EMagicElement::Dark: return ESkillTreeType::DarkTree;
		case EMagicElement::Poison: return ESkillTreeType::PoisonTree;
		case EMagicElement::Ice: return ESkillTreeType::IceTree;
		default: return ESkillTreeType::FireTree;
	}
}

ESkillTreeType USkillTreeComponent::GetTreeTypeFromWeapon(EWeaponStyle Weapon) const
{
	switch (Weapon)
	{
		case EWeaponStyle::TwoHandedSword: return ESkillTreeType::SwordTree;
		case EWeaponStyle::Spear: return ESkillTreeType::SpearTree;
		case EWeaponStyle::Fist: return ESkillTreeType::FistTree;
		case EWeaponStyle::Bow: return ESkillTreeType::BowTree;
		default: return ESkillTreeType::SwordTree;
	}
}
