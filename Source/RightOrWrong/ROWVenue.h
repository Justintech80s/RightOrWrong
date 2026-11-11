#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROWVenue.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UTextRenderComponent;

UENUM(BlueprintType)
enum class EROWVenueType : uint8
{
    None,
    Dojo,
    PoliceStation,
    Underground,
    Evidence,
    RecruitmentAcademy,
    DistrictInfo,
    TrainingStation,
    RivalRecruit,
    CommunityCoach,
    FollowUpCommand,
    FollowUpOversight,
    FollowUpFixer,
    AcademySeminar,
    AcademyGraduation,
    InitialContact,
    SparringRing
};

UCLASS()
class RIGHTORWRONG_API AROWVenue : public AActor
{
    GENERATED_BODY()

public:
    AROWVenue();
    virtual void Tick(float DeltaSeconds) override;
    void InitializeVenue(EROWVenueType InType, const FString& InLabel);

    UPROPERTY(BlueprintReadOnly, Category="Right or Wrong")
    EROWVenueType VenueType = EROWVenueType::Dojo;

    UPROPERTY(BlueprintReadOnly, Category="Right or Wrong")
    FString VenueLabel;

    FString Description;

    // Valid only for a TrainingStation, with indices 0..5 in ROWProgressionRules.h.
    UPROPERTY(BlueprintReadOnly, Category="Right or Wrong")
    int32 SportIndex = -1;

private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<USceneComponent> SceneRoot;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Pedestal;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UTextRenderComponent> Text;
};
