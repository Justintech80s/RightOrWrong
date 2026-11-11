#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ROWSaveGame.generated.h"
UCLASS()
class RIGHTORWRONG_API UROWSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY(SaveGame) int32 Schema = 2;
    UPROPERTY(SaveGame) int64 Generation = 0;
    UPROPERTY(SaveGame) FString CombatSnapshot;
    UPROPERTY(SaveGame) FVector OpponentLocation = FVector(-2700,3600,60);
    UPROPERTY(SaveGame) bool bSparringPrompt = false;
    UPROPERTY(SaveGame) FString NarrativeSnapshot;
    UPROPERTY(SaveGame) FVector PlayerLocation = FVector(0,0,150);
    UPROPERTY(SaveGame) FRotator PlayerRotation = FRotator::ZeroRotator;
    UPROPERTY(SaveGame) FRotator ViewRotation = FRotator(-15,90,0);
    UPROPERTY(SaveGame) FVector TrainingStepOrigin = FVector::ZeroVector;
    UPROPERTY(SaveGame) float TextScale = 1.f;
};
