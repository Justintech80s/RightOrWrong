#include "ROWVenue.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

AROWVenue::AROWVenue()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = .1f;
    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    SetRootComponent(SceneRoot);

    Pedestal = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MarkerPedestal"));
    Pedestal->SetupAttachment(SceneRoot);
    Pedestal->SetRelativeLocation(FVector(0, 0, 60));
    Pedestal->SetRelativeScale3D(FVector(0.35, 0.35, 1.2));
    Pedestal->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (Cube.Succeeded()) Pedestal->SetStaticMesh(Cube.Object);

    Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("FloatingText"));
    Text->SetupAttachment(SceneRoot);
    Text->SetRelativeLocation(FVector(0, 0, 195));
    Text->SetHorizontalAlignment(EHorizTextAligment::EHTA_Center);
    Text->SetWorldSize(38.f);
    Text->SetTextRenderColor(FColor(245, 230, 50));
    Text->SetText(FText::FromString(TEXT("SPORTS VENUE")));
}

void AROWVenue::InitializeVenue(EROWVenueType InType, const FString& InLabel)
{
    VenueType = InType;
    VenueLabel = InLabel;
    Text->SetText(FText::FromString(InLabel));
    if (InType == EROWVenueType::PoliceStation)
    {
        Text->SetTextRenderColor(FColor(100, 210, 255));
    }
    else if (InType == EROWVenueType::Underground)
    {
        Text->SetTextRenderColor(FColor(255, 125, 110));
    }
    else if (InType == EROWVenueType::TrainingStation)
    {
        Text->SetTextRenderColor(FColor(155, 255, 195));
    }
    else if (InType == EROWVenueType::FollowUpCommand || InType == EROWVenueType::FollowUpOversight || InType == EROWVenueType::FollowUpFixer)
    {
        Text->SetTextRenderColor(FColor(235, 170, 255));
    }
    else if (InType == EROWVenueType::AcademySeminar || InType == EROWVenueType::AcademyGraduation)
    {
        Text->SetTextRenderColor(FColor(255, 220, 130));
    }
    else if (InType == EROWVenueType::Evidence)
    {
        Text->SetTextRenderColor(FColor(120, 255, 140));
    }
}

void AROWVenue::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(auto* PC=UGameplayStatics::GetPlayerController(this,0))
    {
        FVector Eye;FRotator View;PC->GetPlayerViewPoint(Eye,View);
        Text->SetWorldRotation((Eye-Text->GetComponentLocation()).Rotation());
    }
}
