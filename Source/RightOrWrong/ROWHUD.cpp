#include "ROWHUD.h"
#include "ROWGameMode.h"
#include "ROWWorldDirector.h"
#include "ROWCharacter.h"
#include "ROWBlockout.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SDPIScaler.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Styling/CoreStyle.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

class SROWNarrativePanel : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SROWNarrativePanel) {} SLATE_ARGUMENT(TWeakObjectPtr<AROWWorldDirector>,Director) SLATE_END_ARGS()
    void Construct(const FArguments& Args)
    {
        Director=Args._Director;
        TSharedRef<SVerticalBox> Content=SNew(SVerticalBox);
        Content->AddSlot().AutoHeight().Padding(0,0,0,8)[SNew(STextBlock).Text_Lambda([this]{return GetText(0);}).Font(FCoreStyle::GetDefaultFontStyle("Bold",26)).ColorAndOpacity(FLinearColor(.95,.75,.3))];
        Content->AddSlot().AutoHeight().Padding(0,0,0,12)[SNew(STextBlock).Text_Lambda([this]{return GetText(1);}).Font(FCoreStyle::GetDefaultFontStyle("Bold",18)).ColorAndOpacity(FLinearColor(.65,.8,1))];
        Content->AddSlot().AutoHeight().Padding(0,0,0,16)[SNew(STextBlock).Text_Lambda([this]{return GetText(2);}).AutoWrapText(true).Font_Lambda([this]{return FCoreStyle::GetDefaultFontStyle("Regular",FontSize());})];
        Content->AddSlot().AutoHeight().Padding(0,0,0,12)
        [SNew(SEditableTextBox).Visibility_Lambda([this]{return Director.IsValid()&&Director->IsProfileScreen()?EVisibility::Visible:EVisibility::Collapsed;})
            .Text_Lambda([this]{return Director.IsValid()?FText::FromString(Director->GetRecruitName()):FText();})
            .HintText(FText::FromString(TEXT("Recruit name"))).Font(FCoreStyle::GetDefaultFontStyle("Regular",22))
            .OnTextChanged_Lambda([this](const FText& Value){if(Director.IsValid())Director->SetRecruitName(Value.ToString());})];
        for(int32 i=0;i<6;++i)
        {
            Content->AddSlot().AutoHeight().Padding(0,0,0,8)
            [SNew(SButton).IsFocusable(false).ContentPadding(FMargin(12,9))
                .Visibility_Lambda([this,i]{return Director.IsValid()&&Director->PanelOptions().IsValidIndex(i)?EVisibility::Visible:EVisibility::Collapsed;})
                .OnClicked_Lambda([this,i]{if(Director.IsValid())Director->HandleChoice(i+1);return FReply::Handled().SetKeyboardFocus(SharedThis(this));})
                [SNew(STextBlock).Text_Lambda([this,i]{if(!Director.IsValid())return FText();const auto O=Director->PanelOptions();return O.IsValidIndex(i)?FText::FromString(FString::Printf(TEXT("%d. %s"),i+1,*O[i])):FText();})
                    .AutoWrapText(true).Font_Lambda([this]{return FCoreStyle::GetDefaultFontStyle("Regular",FontSize());})]];
        }
        Content->AddSlot().AutoHeight().Padding(0,6,0,0)[SNew(STextBlock).Text(FText::FromString(TEXT("Click a response or press its number. Enter advances a single response. Esc opens the menu. Scroll to read more."))).AutoWrapText(true).Font(FCoreStyle::GetDefaultFontStyle("Regular",15)).ColorAndOpacity(FLinearColor(.6,.65,.72))];
        ChildSlot
        [SNew(SDPIScaler).DPIScale_Lambda([]{FVector2D Size(1280,720);if(GEngine&&GEngine->GameViewport)GEngine->GameViewport->GetViewportSize(Size);return static_cast<float>(FMath::Clamp(FMath::Min(Size.X/1000.0,Size.Y/760.0),.35,1.6));})
            [SNew(SOverlay).Visibility(EVisibility::SelfHitTestInvisible)
                +SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Top).Padding(20)
                [SNew(SBorder).Visibility(EVisibility::HitTestInvisible).Padding(12).BorderBackgroundColor(FLinearColor(.025,.035,.055,.94))
                    [SNew(SVerticalBox)
                        +SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text_Lambda([this]{return Director.IsValid()?FText::FromString(Director->GetStatusText()):FText();}).Font(FCoreStyle::GetDefaultFontStyle("Bold",18)).AutoWrapText(true)]
                        +SVerticalBox::Slot().AutoHeight().Padding(0,5)[SNew(STextBlock).Text_Lambda([this]{return Director.IsValid()?FText::FromString(Director->GetObjectiveText()):FText();}).Font(FCoreStyle::GetDefaultFontStyle("Regular",18)).AutoWrapText(true)]]]
                +SOverlay::Slot().HAlign(HAlign_Center).VAlign(VAlign_Center)
                [SNew(SBox).WidthOverride(760).MaxDesiredHeight(540).Visibility_Lambda([this]{return Director.IsValid()&&Director->IsModal()?EVisibility::Visible:EVisibility::Collapsed;})
                    [SNew(SBorder).Padding(24).BorderBackgroundColor(FLinearColor(.025,.035,.055,.99))
                        [SNew(SScrollBox).ScrollBarAlwaysVisible(true)+SScrollBox::Slot()[Content]]]]
                +SOverlay::Slot().HAlign(HAlign_Fill).VAlign(VAlign_Bottom).Padding(20)
                [SNew(SBorder).Visibility(EVisibility::HitTestInvisible).Padding(10).BorderBackgroundColor(FLinearColor(.025,.035,.055,.94))
                    [SNew(SVerticalBox)
                        +SVerticalBox::Slot().AutoHeight()[SNew(STextBlock).Text_Lambda([this]{return Director.IsValid()?FText::FromString(Director->GetMessage()):FText();}).AutoWrapText(true).Font(FCoreStyle::GetDefaultFontStyle("Regular",16)).ColorAndOpacity(FLinearColor(.95,.8,.35))]
                        +SVerticalBox::Slot().AutoHeight().Padding(0,5)[SNew(STextBlock).Text(FText::FromString(TEXT("WASD move | Shift run | Space jump | Mouse look | E interact | LMB action/draw | RMB block/aim | Tab journal | Esc menu | F5 save | F9 load"))).AutoWrapText(true).Font(FCoreStyle::GetDefaultFontStyle("Regular",15))]]]
            ]
        ];
    }
    virtual bool SupportsKeyboardFocus() const override {return true;}
    virtual FReply OnKeyDown(const FGeometry& Geometry,const FKeyEvent& Event) override
    {
        if(!Director.IsValid())return FReply::Unhandled();
        const FKey Key=Event.GetKey();
        if(Key==EKeys::Escape){Director->ToggleMenu();return FReply::Handled();}
        if(Key==EKeys::Tab){Director->OpenJournal();return FReply::Handled();}
        if(Key==EKeys::F5){Director->SaveProgress();return FReply::Handled();}
        if(Key==EKeys::F9){Director->LoadProgress();return FReply::Handled();}
        if(Key==EKeys::Enter){Director->AdvanceSingleChoice();return FReply::Handled();}
        const FKey Choices[]={EKeys::One,EKeys::Two,EKeys::Three,EKeys::Four,EKeys::Five,EKeys::Six};
        for(int32 i=0;i<6;++i)if(Key==Choices[i]){Director->HandleChoice(i+1);return FReply::Handled();}
        return SCompoundWidget::OnKeyDown(Geometry,Event);
    }
private:
    TWeakObjectPtr<AROWWorldDirector> Director;
    int32 FontSize() const {return FMath::RoundToInt(20.f*(Director.IsValid()?Director->GetTextScale():1.f));}
    FText GetText(int32 Kind) const {if(!Director.IsValid())return FText();return FText::FromString(Kind==0?Director->PanelTitle():Kind==1?Director->PanelSpeaker():Director->PanelBody());}
};

void AROWHUD::DrawHUD()
{
    Super::DrawHUD();
    auto* Mode=GetWorld()?GetWorld()->GetAuthGameMode<AROWGameMode>():nullptr;
    auto* Director=Mode?Mode->GetDirector():nullptr;
    auto* PC=GetOwningPlayerController();
    if(!Director||!PC||!Canvas||!GEngine||!GEngine->GameViewport)return;
    if(!RootWidget.IsValid())
    {
        RootWidget=SNew(SROWNarrativePanel).Visibility(EVisibility::SelfHitTestInvisible).Director(TWeakObjectPtr<AROWWorldDirector>(Director));
        GEngine->GameViewport->AddViewportWidgetContent(RootWidget.ToSharedRef(),20);
    }
    const bool Modal=Director->IsModal();
    if(Modal!=bPreviousModal)
    {
        PC->bShowMouseCursor=Modal;
        if(Modal)
        {
            FInputModeGameAndUI ModeUI;ModeUI.SetWidgetToFocus(RootWidget);ModeUI.SetHideCursorDuringCapture(false);PC->SetInputMode(ModeUI);
            if(auto* Player=Cast<AROWCharacter>(PC->GetPawn()))Player->GetCharacterMovement()->StopMovementImmediately();
        }
        else PC->SetInputMode(FInputModeGameOnly());
        bPreviousModal=Modal;
    }
    if(Modal)return;
    const float X=Canvas->SizeX*.5f,Y=Canvas->SizeY*.5f;
    DrawLine(X-7,Y,X+7,Y,FLinearColor::White,1.5f);DrawLine(X,Y-7,X,Y+7,FLinearColor::White,1.5f);
    FVector2D Point; if(PC->ProjectWorldLocationToScreen(Director->GetObjectiveLocation(),Point,true))
    {
        Point.X=FMath::Clamp(Point.X,20.0,static_cast<double>(Canvas->SizeX)-190.0);Point.Y=FMath::Clamp(Point.Y,130.0,static_cast<double>(Canvas->SizeY)-130.0);
        DrawText(TEXT("NEXT OBJECTIVE"),FLinearColor(1,.8,.2),Point.X,Point.Y,GEngine->GetSmallFont(),1.1f);
    }
    // Compact navigation map: north at the top, player cyan, next destination gold.
    const float MapX=Canvas->SizeX-200.f,MapY=130.f,MapSize=175.f;
    DrawRect(FLinearColor(.02,.03,.05,.85),MapX,MapY,MapSize,MapSize);
    auto MapPoint=[&](const FVector& Location){return FVector2D(MapX+MapSize*(.5f+Location.X/9000.f),MapY+MapSize*(.5f-Location.Y/9000.f));};
    for(const auto& B:ROWBlockout::Geometry())
    {
        // Building silhouettes and roads make the miniature map useful for finding doorways.
        if(B.Palette!=2&&B.Palette!=3&&B.Palette!=5&&B.Palette!=8)continue;
        const auto Corner=MapPoint(FVector(B.X-B.W/2,B.Y+B.D/2,0));
        DrawRect(B.Palette==3?FLinearColor(.23,.26,.3):FLinearColor(.4,.45,.5,.75),Corner.X,Corner.Y,B.W*MapSize/9000,B.D*MapSize/9000);
    }
    for(const auto& Place:ROWBlockout::Destinations())
    {
        const auto Dot=MapPoint(FVector(Place.X,Place.Y,0));DrawRect(FLinearColor(.4,.7,.6),Dot.X-1,Dot.Y-1,2,2);
    }
    const FVector2D Goal=MapPoint(Director->GetObjectiveLocation());
    DrawRect(FLinearColor(1,.8,.2),Goal.X-3,Goal.Y-3,6,6);
    if(PC->GetPawn()){const FVector2D Here=MapPoint(PC->GetPawn()->GetActorLocation());DrawRect(FLinearColor(.2,.9,1),Here.X-3,Here.Y-3,6,6);}
    DrawText(TEXT("N  |  Vanguard / Greyhaven"),FLinearColor::White,MapX+5,MapY+5,GEngine->GetSmallFont(),.8f);
    auto* Player=Cast<AROWCharacter>(PC->GetPawn());
    DrawText(Director->GetNearbyActionText(Player),FLinearColor(.6,1,.8),Canvas->SizeX*.35f,Canvas->SizeY*.68f,GEngine->GetSmallFont(),1.4f);
}
void AROWHUD::EndPlay(const EEndPlayReason::Type Reason)
{
    if(RootWidget.IsValid()&&GEngine&&GEngine->GameViewport)GEngine->GameViewport->RemoveViewportWidgetContent(RootWidget.ToSharedRef());
    RootWidget.Reset();Super::EndPlay(Reason);
}
