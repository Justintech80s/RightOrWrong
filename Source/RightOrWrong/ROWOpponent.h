#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ROWCombatRules.h"
#include "ROWOpponent.generated.h"
class UStaticMeshComponent;
class UMaterialInstanceDynamic;

// Supervised opponent. Decisions come from ROWCombat::Match; CharacterMovement
// supplies capsule collision, floor checks and gravity instead of teleport chasing.
UCLASS()
class RIGHTORWRONG_API AROWOpponent : public ACharacter
{
    GENERATED_BODY()
public:
    AROWOpponent();
    virtual void BeginPlay() override;
    void UpdateMatch(const ROWCombat::State& State,const FVector& PlayerLocation,bool bPaused);
    void StopMovement();
private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Torso;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> Head;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> LeadGlove;
    UPROPERTY() TObjectPtr<UMaterialInstanceDynamic> BodyMaterial;
};
