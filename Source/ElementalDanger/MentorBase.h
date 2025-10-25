// Base Mentor Class - For dragons and combat tutors

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicTypes.h"
#include "InteractableInterface.h"
#include "MentorBase.generated.h"

class ANinjaWizardCharacter;

USTRUCT(BlueprintType)
struct FMentorDialogue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredMasteryLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsQuestDialogue;

	FMentorDialogue()
		: RequiredMasteryLevel(0)
		, bIsQuestDialogue(false)
	{}
};

USTRUCT(BlueprintType)
struct FMentorQuest
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText QuestDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 RequiredMasteryLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSpellData> RewardSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FWeaponAttackData> RewardAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMagicElement RewardElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponStyle RewardWeaponStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCompleted;

	FMentorQuest()
		: RequiredMasteryLevel(0)
		, RewardElement(EMagicElement::None)
		, RewardWeaponStyle(EWeaponStyle::None)
		, bCompleted(false)
	{}
};

UCLASS()
class ELEMENTALDANGER_API AMentorBase : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AMentorBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ============================================
	// IInteractableInterface Implementation
	// ============================================

	virtual void Interact_Implementation(ANinjaWizardCharacter* Player) override;
	virtual FText GetInteractionText_Implementation() const override;
	virtual bool CanInteract_Implementation(ANinjaWizardCharacter* Player) const override;
	virtual void OnInteractionFocus_Implementation() override;
	virtual void OnInteractionUnfocus_Implementation() override;

	// ============================================
	// Mentor Properties
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor")
	EMentorType MentorType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor")
	FName MentorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor")
	FText MentorDescription;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mentor")
	int32 RelationshipLevel; // 0-100

	// ============================================
	// Teaching
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor|Teaching")
	TArray<FSpellData> TeachableSpells;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor|Teaching")
	TArray<FWeaponAttackData> TeachableAttacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor|Teaching")
	EMagicElement PrimaryElement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor|Teaching")
	TArray<EWeaponStyle> TeachableWeaponStyles;

	// ============================================
	// Dialogue & Quests
	// ============================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor|Dialogue")
	TArray<FMentorDialogue> DialogueOptions;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mentor|Quests")
	TArray<FMentorQuest> AvailableQuests;

	// ============================================
	// Interaction
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	void InteractWithPlayer(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	void TeachSpell(ANinjaWizardCharacter* Player, const FSpellData& Spell);

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	void TeachAttack(ANinjaWizardCharacter* Player, const FWeaponAttackData& Attack);

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	void UnlockElementForPlayer(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	void UnlockWeaponStyleForPlayer(ANinjaWizardCharacter* Player, EWeaponStyle WeaponStyle);

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	void ImproveRelationship(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Mentor")
	bool CanTeachPlayer(ANinjaWizardCharacter* Player) const;

	// ============================================
	// Quests
	// ============================================

	UFUNCTION(BlueprintCallable, Category = "Mentor|Quests")
	TArray<FMentorQuest> GetAvailableQuests(ANinjaWizardCharacter* Player) const;

	UFUNCTION(BlueprintCallable, Category = "Mentor|Quests")
	void CompleteQuest(FMentorQuest& Quest, ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintCallable, Category = "Mentor|Quests")
	void GiveQuestRewards(const FMentorQuest& Quest, ANinjaWizardCharacter* Player);

	// ============================================
	// Events
	// ============================================

	UFUNCTION(BlueprintImplementableEvent, Category = "Mentor|Events")
	void OnPlayerInteract(ANinjaWizardCharacter* Player);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mentor|Events")
	void OnSpellTaught(ANinjaWizardCharacter* Player, const FSpellData& Spell);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mentor|Events")
	void OnRelationshipImproved(int32 NewLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "Mentor|Events")
	void OnQuestCompleted(ANinjaWizardCharacter* Player, const FMentorQuest& Quest);

protected:
	UFUNCTION(BlueprintCallable, Category = "Mentor")
	virtual void InitializeMentor();
};
