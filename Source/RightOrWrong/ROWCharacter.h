#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ROWCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStaticMeshComponent;
class AROWWorldDirector;

UCLASS()
class RIGHTORWRONG_API AROWCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AROWCharacter();
    virtual void Tick(float DeltaSeconds) override;
    void ShowStrike();
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void PossessedBy(AController* NewController) override;
    void ResetHeldInput();
    void ApplyProfile(int32 Presentation, int32 Outfit);

private:
    UPROPERTY(VisibleAnywhere) TObjectPtr<USpringArmComponent> CameraBoom;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UCameraComponent> FollowCamera;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> BodyShape;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> HeadShape;
    UPROPERTY(VisibleAnywhere) TObjectPtr<UStaticMeshComponent> LeadGlove;
    float StrikePose = 0.f;
    bool bGuardPose = false;

    float LastAttackAt = -100.f;

    void MoveForward(float AxisValue);
    void MoveRight(float AxisValue);
    void Turn(float AxisValue);
    void LookUp(float AxisValue);
    void BeginJump();
    void EndJump();
    void Attack();
    void ReleaseAttack();
    void PauseMenu();
    void Journal();
    void Advance();
    void Interact();
    void BlockStart();
    void BlockEnd();
    void SprintStart();
    void SprintEnd();
    void ChoiceOne();
    void ChoiceTwo();
    void ChoiceThree();
    void ChoiceFour();
    void ChoiceFive();
    void ChoiceSix();
    void Save();
    void Load();
    AROWWorldDirector* GetDirector() const;
};
