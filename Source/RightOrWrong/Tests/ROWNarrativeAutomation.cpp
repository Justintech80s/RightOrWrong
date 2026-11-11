#include "ROWNarrative.h"
#include "ROWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FROWNarrativeSaveRoundTrip,"RightOrWrong.Narrative.SaveGameRoundTrip",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FROWNarrativeSaveRoundTrip::RunTest(const FString& Parameters)
{
    (void)Parameters;
    for(int32 Sport=0;Sport<6;++Sport)
    {
        ROWNarrative::Session Session;Session.SetName("Test Recruit");Session.Choose(1);Session.Choose(1);
        Session.Visit(ROWNarrative::Place::Academy);Session.Choose(1);Session.Choose(Sport+1);
        Session.Visit(ROWNarrative::Place::Mentor);Session.Choose(1);Session.Visit(ROWNarrative::Place::Training,Sport);Session.Choose(1);
        Session.Tick(4.0,{});
        UROWSaveGame* Save=Cast<UROWSaveGame>(UGameplayStatics::CreateSaveGameObject(UROWSaveGame::StaticClass()));
        if(!TestNotNull(TEXT("Save object"),Save))return false;
        const auto Snapshot=Session.Serialize();Save->NarrativeSnapshot=UTF8_TO_TCHAR(Snapshot.c_str());Save->PlayerLocation=FVector(10,20,150);
        TArray<uint8> Bytes;if(!TestTrue(TEXT("Unreal serialization succeeds"),UGameplayStatics::SaveGameToMemory(Save,Bytes)))return false;
        auto* Loaded=Cast<UROWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));if(!TestNotNull(TEXT("Loaded save"),Loaded))return false;
        ROWNarrative::Session Restored;
        TestTrue(TEXT("Narrative snapshot validates"),Restored.Restore(TCHAR_TO_UTF8(*Loaded->NarrativeSnapshot)));
        TestEqual(TEXT("Selected sport preserved"),Restored.GetState().Sport,Sport);
        TestEqual(TEXT("Assessment clock preserved"),Restored.GetAssessment().Elapsed,4.0);
        TestTrue(TEXT("Player location preserved"),Loaded->PlayerLocation.Equals(Save->PlayerLocation));
    }
    return true;
}
#endif
