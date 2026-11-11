#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROWTrainingTarget.generated.h"
class USceneComponent;
class UStaticMeshComponent;
UCLASS()
class RIGHTORWRONG_API AROWTrainingTarget : public AActor
{
    GENERATED_BODY()
public:
    AROWTrainingTarget();
    void Configure(int32 Sport);
    void React();
    virtual void Tick(float DeltaSeconds) override;
    FVector GetAimPoint() const { return Home; }
private:
    UPROPERTY() TObjectPtr<USceneComponent> Root;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Body;
    UPROPERTY() TObjectPtr<UStaticMeshComponent> Center;
    int32 Program = 0;
    float Reaction = 0.f;
    FVector Home = FVector::ZeroVector;
};
