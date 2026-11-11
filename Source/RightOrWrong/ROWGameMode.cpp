#include "ROWGameMode.h"
#include "ROWCharacter.h"
#include "ROWWorldDirector.h"
#include "ROWHUD.h"
#include "Engine/World.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AROWGameMode::AROWGameMode()
{
    DefaultPawnClass = AROWCharacter::StaticClass();
    HUDClass = AROWHUD::StaticClass();
}

void AROWGameMode::InitGameState()
{
    Super::InitGameState();
    // An Empty Level has no PlayerStart. Make this project usable without manual placement.
    bool bHasStart = false;
    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        bHasStart = true;
        break;
    }
    if (!bHasStart && GetWorld())
    {
        GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), FVector(0, 0, 150), FRotator::ZeroRotator);
    }
}

APawn* AROWGameMode::SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform)
{
    FTransform ElevatedStart = SpawnTransform;
    ElevatedStart.SetLocation(FVector(0, 0, 150));
    return Super::SpawnDefaultPawnAtTransform_Implementation(NewPlayer, ElevatedStart);
}

void AROWGameMode::BeginPlay()
{
    Super::BeginPlay();
    if (GetWorld())
    {
        Director = GetWorld()->SpawnActor<AROWWorldDirector>(AROWWorldDirector::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
        // Loading is explicit from the menu after the player has spawned.
    }
}
