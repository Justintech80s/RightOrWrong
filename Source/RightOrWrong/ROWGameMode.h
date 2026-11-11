#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ROWGameMode.generated.h"

class AROWWorldDirector;

UCLASS()
class RIGHTORWRONG_API AROWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AROWGameMode();
    virtual void InitGameState() override;
    virtual void BeginPlay() override;
    virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
    AROWWorldDirector* GetDirector() const { return Director; }

private:
    UPROPERTY() TObjectPtr<AROWWorldDirector> Director;
};
