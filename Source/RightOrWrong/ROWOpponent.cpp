#include "ROWOpponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"

AROWOpponent::AROWOpponent()
{
    PrimaryActorTick.bCanEverTick=false;
    GetCapsuleComponent()->InitCapsuleSize(42.f,90.f);
    auto* Movement=GetCharacterMovement();
    Movement->bRunPhysicsWithNoController=true;
    Movement->bOrientRotationToMovement=false;
    Movement->MaxWalkSpeed=260.f;
    Movement->MaxAcceleration=1600.f;
    Movement->BrakingDecelerationWalking=1800.f;
    bUseControllerRotationYaw=false;
    Torso=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SparringBody"));
    Torso->SetupAttachment(GetCapsuleComponent());
    Torso->SetRelativeLocation(FVector(0,0,-10));
    Torso->SetRelativeScale3D(FVector(.6,.45,1.2));
    Torso->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Head=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SparringHead"));
    Head->SetupAttachment(GetCapsuleComponent());
    Head->SetRelativeLocation(FVector(0,0,72));Head->SetRelativeScale3D(FVector(.38));
    Head->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    LeadGlove=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeadGlove"));
    LeadGlove->SetupAttachment(GetCapsuleComponent());LeadGlove->SetRelativeLocation(FVector(32,-32,30));
    LeadGlove->SetRelativeScale3D(FVector(.25));LeadGlove->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if(Cube.Succeeded())Torso->SetStaticMesh(Cube.Object);
    if(Sphere.Succeeded()){Head->SetStaticMesh(Sphere.Object);LeadGlove->SetStaticMesh(Sphere.Object);}
}
void AROWOpponent::BeginPlay()
{
    Super::BeginPlay();
    if(auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_ROWColor.M_ROWColor")))
    {BodyMaterial=UMaterialInstanceDynamic::Create(Base,this);Torso->SetMaterial(0,BodyMaterial);}
}
void AROWOpponent::StopMovement()
{
    ConsumeMovementInputVector();GetCharacterMovement()->StopMovementImmediately();
}
void AROWOpponent::UpdateMatch(const ROWCombat::State& State,const FVector& PlayerLocation,bool bPaused)
{
    const bool Active=State.Result==ROWCombat::Outcome::Active;
    if(bPaused||!Active){StopMovement();LeadGlove->SetRelativeLocation(FVector(32,-32,30));return;}
    const FVector Toward=(PlayerLocation-GetActorLocation()).GetSafeNormal2D();
    if(!Toward.IsNearlyZero())SetActorRotation(Toward.Rotation());
    const double Distance=FVector::Dist2D(PlayerLocation,GetActorLocation());
    if(State.AI==ROWCombat::Phase::Closing&&Distance>150)AddMovementInput(Toward,1.f,true);
    else StopMovement();
    const bool Windup=State.AI==ROWCombat::Phase::Windup;
    const bool Guard=State.AI==ROWCombat::Phase::Guard;
    if(BodyMaterial)BodyMaterial->SetVectorParameterValue(TEXT("Tint"),Windup?FLinearColor(.85,.12,.04):Guard?FLinearColor(.1,.35,.9):FLinearColor(.25,.55,.4));
    LeadGlove->SetRelativeLocation(Windup?FVector(10,-40,65):Guard?FVector(45,-15,65):State.AI==ROWCombat::Phase::Recovery&&State.PhaseLeft>.8?FVector(100,-15,40):FVector(32,-32,30));
}
