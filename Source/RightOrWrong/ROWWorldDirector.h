#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROWVenue.h"
#include "ROWNarrative.h"
#include "ROWCombatRules.h"
#include "ROWWorldDirector.generated.h"
class AROWCharacter;
class AROWTrainingTarget;
class AROWOpponent;
class UROWSaveGame;
class AStaticMeshActor;
UCLASS()
class RIGHTORWRONG_API AROWWorldDirector : public AActor
{
    GENERATED_BODY()
public:
    AROWWorldDirector();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;
    void HandleInteraction(AROWCharacter* Player);
    void HandleChoice(int32 Choice);
    void HandleAttack(AROWCharacter* Player);
    void HandleAttackReleased();
    void HandleGuard(bool bPressed);
    void SaveProgress();
    void LoadProgress();
    void ToggleMenu();
    void OpenJournal();
    void AdvanceSingleChoice();
    bool IsModal() const;
    bool IsProfileScreen() const;
    void SetRecruitName(const FString& Name) { PendingName = Name; }
    FString GetRecruitName() const { return PendingName; }
    FString PanelTitle() const;
    FString PanelSpeaker() const;
    FString PanelBody() const;
    TArray<FString> PanelOptions() const;
    FString GetObjectiveText() const;
    FString GetStatusText() const;
    FString GetNearbyActionText(const AROWCharacter* Player) const;
    FString GetMessage() const;
    float GetTextScale() const { return TextScale; }
    FVector GetObjectiveLocation() const;
    UFUNCTION(BlueprintPure, Category="Narrative") FString GetInitialContactNetwork() const;
    UFUNCTION(BlueprintPure, Category="Narrative") FString GetInitialContactTier() const;
    UFUNCTION(BlueprintPure, Category="Narrative") FString GetInitialContactDescription() const;
private:
    ROWNarrative::Session Narrative;
    ROWCombat::Match Combat;
    UPROPERTY() TObjectPtr<AROWOpponent> Opponent;
    bool bSparringPrompt = false;
    UPROPERTY() TArray<TObjectPtr<AROWVenue>> Venues;
    UPROPERTY() TArray<TObjectPtr<AROWTrainingTarget>> TrainingTargets;
    bool bMenu = false;
    bool bJournal = false;
    bool bConfirmNew = false;
    bool bDrawing = false;
    bool bGuarding = false;
    double DrawSeconds = 0;
    double GuardSeconds = 0;
    FVector StepOrigin = FVector::ZeroVector;
    FString PendingName = TEXT("Recruit");
    FString Notice;
    float TextScale = 1.f;
    void GeneratePrototypeNeighborhood();
    AROWVenue* PlaceVenue(EROWVenueType Type,const FString& Label,const FVector& Location);
    AStaticMeshActor* PlaceCube(const FVector& Center,const FVector& Size);
    AROWVenue* FindNearbyVenue(const FVector& Location) const;
    ROWNarrative::Frame TrainingFrame() const;
    void AfterAction(int32 OldStep);
    void RefreshAppearance();
    void ResetHeldInput();
    void StartSparring();
    ROWCombat::Frame CombatFrame() const;
    bool ValidateSave(const UROWSaveGame* Save,ROWNarrative::Session& Story,ROWCombat::Match& Match) const;
};
