#include "ROWTrainingTarget.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
AROWTrainingTarget::AROWTrainingTarget()
{
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root")); SetRootComponent(Root);
    Body = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Target")); Body->SetupAttachment(Root);
    Center = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Center")); Center->SetupAttachment(Root);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (Cube.Succeeded()) Body->SetStaticMesh(Cube.Object);
    if (Sphere.Succeeded()) Center->SetStaticMesh(Sphere.Object);
    Root->SetMobility(EComponentMobility::Movable);
    Body->SetMobility(EComponentMobility::Movable);
    Center->SetMobility(EComponentMobility::Movable);
    Body->SetCollisionProfileName(TEXT("BlockAll"));
    Center->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
void AROWTrainingTarget::Configure(int32 Sport)
{
    Program = Sport; Home = GetActorLocation();
    const bool bRange = Sport == 3 || Sport == 5;
    Body->SetRelativeScale3D(bRange ? FVector(2.f,.12f,2.f) : FVector(.6f,.5f,1.5f));
    Center->SetRelativeLocation(bRange ? FVector(0,-12,0) : FVector(0,0,105));
    Center->SetRelativeScale3D(FVector(bRange ? .22f : .4f));
    if (UMaterialInterface* Base = LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_ROWColor.M_ROWColor")))
    {
        UMaterialInstanceDynamic* Material = UMaterialInstanceDynamic::Create(Base,this);
        Material->SetVectorParameterValue(TEXT("Tint"),FLinearColor(.95f,.65f,.08f)); Center->SetMaterial(0,Material);
    }
}
void AROWTrainingTarget::React() { Reaction = .35f; }
void AROWTrainingTarget::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if (Reaction <= 0.f) return;
    Reaction = FMath::Max(0.f,Reaction-DeltaSeconds);
    const float Amount = FMath::Sin(Reaction/.35f*PI);
    SetActorRotation(FRotator(0,0,Amount*(Program == 1 ? 65.f : 8.f)));
    SetActorLocation(Home + FVector(0,Amount*12.f,0));
}
