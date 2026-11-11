#pragma once
#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ROWHUD.generated.h"
class SWidget;
UCLASS()
class RIGHTORWRONG_API AROWHUD : public AHUD
{
    GENERATED_BODY()
public:
    virtual void DrawHUD() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
private:
    TSharedPtr<SWidget> RootWidget;
    bool bPreviousModal = false;
};
