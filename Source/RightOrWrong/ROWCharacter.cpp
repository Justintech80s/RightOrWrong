#include "ROWCharacter.h"
#include "ROWGameMode.h"
#include "ROWWorldDirector.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Math/RotationMatrix.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "UObject/ConstructorHelpers.h"

AROWCharacter::AROWCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    GetCapsuleComponent()->InitCapsuleSize(42.f, 90.f);

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 650.f, 0.f);
    GetCharacterMovement()->MaxWalkSpeed = 410.f;
    GetCharacterMovement()->JumpZVelocity = 550.f;
    GetCharacterMovement()->AirControl = 0.25f;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(GetRootComponent());
    CameraBoom->TargetArmLength = 420.f;
    CameraBoom->SetRelativeRotation(FRotator(-19.f, 0.f, 0.f));
    CameraBoom->bUsePawnControlRotation = true;
    CameraBoom->bDoCollisionTest = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    BodyShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrototypeBody"));
    BodyShape->SetupAttachment(GetCapsuleComponent());
    BodyShape->SetRelativeLocation(FVector(0, 0, -10));
    BodyShape->SetRelativeScale3D(FVector(0.55, 0.42, 1.2));
    BodyShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    HeadShape = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PrototypeHead"));
    HeadShape->SetupAttachment(GetCapsuleComponent());
    HeadShape->SetRelativeLocation(FVector(0, 0, 72));
    HeadShape->SetRelativeScale3D(FVector(0.38));
    HeadShape->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    LeadGlove=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeadGlove"));
    LeadGlove->SetupAttachment(GetCapsuleComponent());LeadGlove->SetRelativeLocation(FVector(30,-30,25));
    LeadGlove->SetRelativeScale3D(FVector(.23));LeadGlove->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    if (Cube.Succeeded()) BodyShape->SetStaticMesh(Cube.Object);
    if (Sphere.Succeeded()) {HeadShape->SetStaticMesh(Sphere.Object);LeadGlove->SetStaticMesh(Sphere.Object);}
}

void AROWCharacter::ShowStrike()
{
    StrikePose=.25f;
    if(Controller)SetActorRotation(FRotator(0,Controller->GetControlRotation().Yaw,0));
}

void AROWCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    if(const auto* D=GetDirector())if(D->IsModal())return;
    StrikePose=FMath::Max(0.f,StrikePose-DeltaSeconds);
    LeadGlove->SetRelativeLocation(StrikePose>0?FVector(85,-15,40):bGuardPose?FVector(40,-15,65):FVector(30,-30,25));
}

void AROWCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);
    if (NewController) NewController->SetControlRotation(FRotator(-22.f, 90.f, 0.f));
    if(auto* PC=Cast<APlayerController>(NewController))if(PC->PlayerCameraManager)
    {PC->PlayerCameraManager->ViewPitchMin=-70.f;PC->PlayerCameraManager->ViewPitchMax=55.f;}
}

void AROWCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AROWCharacter::MoveForward);
    PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AROWCharacter::MoveRight);
    PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AROWCharacter::Turn);
    PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AROWCharacter::LookUp);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AROWCharacter::BeginJump);
    PlayerInputComponent->BindAction(TEXT("Jump"), IE_Released, this, &AROWCharacter::EndJump);
    PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AROWCharacter::Attack);
    PlayerInputComponent->BindAction(TEXT("Attack"), IE_Released, this, &AROWCharacter::ReleaseAttack);
    PlayerInputComponent->BindAction(TEXT("PauseMenu"), IE_Pressed, this, &AROWCharacter::PauseMenu);
    PlayerInputComponent->BindAction(TEXT("Journal"), IE_Pressed, this, &AROWCharacter::Journal);
    PlayerInputComponent->BindAction(TEXT("Advance"), IE_Pressed, this, &AROWCharacter::Advance);
    PlayerInputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AROWCharacter::Interact);
    PlayerInputComponent->BindAction(TEXT("Block"), IE_Pressed, this, &AROWCharacter::BlockStart);
    PlayerInputComponent->BindAction(TEXT("Block"), IE_Released, this, &AROWCharacter::BlockEnd);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &AROWCharacter::SprintStart);
    PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &AROWCharacter::SprintEnd);
    PlayerInputComponent->BindAction(TEXT("ChoiceOne"), IE_Pressed, this, &AROWCharacter::ChoiceOne);
    PlayerInputComponent->BindAction(TEXT("ChoiceTwo"), IE_Pressed, this, &AROWCharacter::ChoiceTwo);
    PlayerInputComponent->BindAction(TEXT("ChoiceThree"), IE_Pressed, this, &AROWCharacter::ChoiceThree);
    PlayerInputComponent->BindAction(TEXT("ChoiceFour"), IE_Pressed, this, &AROWCharacter::ChoiceFour);
    PlayerInputComponent->BindAction(TEXT("ChoiceFive"), IE_Pressed, this, &AROWCharacter::ChoiceFive);
    PlayerInputComponent->BindAction(TEXT("ChoiceSix"), IE_Pressed, this, &AROWCharacter::ChoiceSix);
    PlayerInputComponent->BindAction(TEXT("Save"), IE_Pressed, this, &AROWCharacter::Save);
    PlayerInputComponent->BindAction(TEXT("Load"), IE_Pressed, this, &AROWCharacter::Load);
}

void AROWCharacter::MoveForward(float AxisValue)
{
    if (!Controller || FMath::IsNearlyZero(AxisValue)) return;
    if(const auto* D=GetDirector())if(D->IsModal())return;
    const FRotator ViewYaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(ViewYaw).GetUnitAxis(EAxis::X), AxisValue);
}

void AROWCharacter::MoveRight(float AxisValue)
{
    if (!Controller || FMath::IsNearlyZero(AxisValue)) return;
    if(const auto* D=GetDirector())if(D->IsModal())return;
    const FRotator ViewYaw(0.f, Controller->GetControlRotation().Yaw, 0.f);
    AddMovementInput(FRotationMatrix(ViewYaw).GetUnitAxis(EAxis::Y), AxisValue);
}

void AROWCharacter::Turn(float AxisValue) { if(const auto* D=GetDirector())if(D->IsModal())return; AddControllerYawInput(AxisValue); }
void AROWCharacter::LookUp(float AxisValue) { if(const auto* D=GetDirector())if(D->IsModal())return; AddControllerPitchInput(AxisValue); }
void AROWCharacter::BeginJump() { if (const auto* D=GetDirector()) if(D->IsModal())return; Jump(); }
void AROWCharacter::EndJump() { StopJumping(); }
void AROWCharacter::BlockStart() { if(auto* D=GetDirector()){if(D->IsModal())return;D->HandleGuard(true);}bGuardPose=true; }
void AROWCharacter::BlockEnd() { bGuardPose=false;if(auto* D=GetDirector())D->HandleGuard(false); }
void AROWCharacter::SprintStart() { if(const auto* D=GetDirector())if(D->IsModal())return; GetCharacterMovement()->MaxWalkSpeed = 700.f; }
void AROWCharacter::SprintEnd() { GetCharacterMovement()->MaxWalkSpeed = 410.f; }

AROWWorldDirector* AROWCharacter::GetDirector() const
{
    const AROWGameMode* Mode = GetWorld() ? GetWorld()->GetAuthGameMode<AROWGameMode>() : nullptr;
    return Mode ? Mode->GetDirector() : nullptr;
}

void AROWCharacter::Attack()
{
    if (!GetWorld()) return;
    const float Now = GetWorld()->GetTimeSeconds();
    if (Now - LastAttackAt < 0.42f) return;
    LastAttackAt = Now;
    if (AROWWorldDirector* Director = GetDirector()) Director->HandleAttack(this);
}

void AROWCharacter::Interact()
{
    if (AROWWorldDirector* Director = GetDirector()) Director->HandleInteraction(this);
}

void AROWCharacter::ChoiceOne() { if (AROWWorldDirector* Director = GetDirector()) Director->HandleChoice(1); }
void AROWCharacter::ChoiceTwo() { if (AROWWorldDirector* Director = GetDirector()) Director->HandleChoice(2); }
void AROWCharacter::ChoiceThree() { if (AROWWorldDirector* Director = GetDirector()) Director->HandleChoice(3); }
void AROWCharacter::ChoiceFour() { if (AROWWorldDirector* Director = GetDirector()) Director->HandleChoice(4); }
void AROWCharacter::ChoiceFive() { if (AROWWorldDirector* Director = GetDirector()) Director->HandleChoice(5); }
void AROWCharacter::ChoiceSix() { if (AROWWorldDirector* Director = GetDirector()) Director->HandleChoice(6); }

void AROWCharacter::Save()
{
    if (AROWWorldDirector* Director = GetDirector()) Director->SaveProgress();
}

void AROWCharacter::Load()
{
    if (AROWWorldDirector* Director = GetDirector()) Director->LoadProgress();
}

void AROWCharacter::ResetHeldInput()
{
    StrikePose=0;bGuardPose=false;StopJumping();ConsumeMovementInputVector();GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->MaxWalkSpeed=410.f;LastAttackAt=-100.f;
}

void AROWCharacter::ReleaseAttack() { if(auto* D=GetDirector())D->HandleAttackReleased(); }
void AROWCharacter::PauseMenu() { if(auto* D=GetDirector())D->ToggleMenu(); }
void AROWCharacter::Journal() { if(auto* D=GetDirector())D->OpenJournal(); }
void AROWCharacter::Advance() { if(auto* D=GetDirector())D->AdvanceSingleChoice(); }
void AROWCharacter::ApplyProfile(int32 Presentation,int32 Outfit)
{
    BodyShape->SetRelativeScale3D(Presentation==2?FVector(.5,.42,1.15):FVector(.55,.42,1.2));
    if(UMaterialInterface* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_ROWColor.M_ROWColor")))
    {
        auto* Material=UMaterialInstanceDynamic::Create(Base,this);
        const FLinearColor Colors[]={FLinearColor(.85,.55,.08),FLinearColor(.08,.3,.8),FLinearColor(.25,.28,.33)};
        Material->SetVectorParameterValue(TEXT("Tint"),Colors[FMath::Clamp(Outfit,0,2)]);
        BodyShape->SetMaterial(0,Material);
    }
}
