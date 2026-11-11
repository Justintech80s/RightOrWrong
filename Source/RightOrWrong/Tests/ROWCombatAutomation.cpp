#include "ROWCombatRules.h"
#include "ROWNarrative.h"
#include "ROWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FROWCombatSaveRoundTrip,"RightOrWrong.Combat.SaveGameRoundTrip",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FROWCombatSaveRoundTrip::RunTest(const FString& Parameters)
{
    (void)Parameters;
    ROWNarrative::Session Story;Story.SetName("Save Test");Story.Choose(1);Story.Choose(1);
    Story.Visit(ROWNarrative::Place::Academy);Story.Choose(1);Story.Choose(1);
    Story.Visit(ROWNarrative::Place::Mentor);Story.Choose(1);
    Story.Visit(ROWNarrative::Place::Training,0);Story.Choose(1);
    ROWNarrative::Frame Training;Training.Distance=200;Training.Aim=1;
    for(int32 I=0;I<6;++I)Story.Act(I%2==0?ROWNarrative::Action::Strike:ROWNarrative::Action::Guard,Training);
    Story.Choose(1);
    if(!TestEqual(TEXT("Assessment completed before sparring"),Story.GetState().PlacementScore,100))return false;
    ROWCombat::Match Match;Match.Start();
    ROWCombat::Frame Frame;Frame.Distance=150;Frame.Facing=1;Frame.LineOfSight=true;
    Match.Tick(.3,Frame,false);
    UROWSaveGame* Save=Cast<UROWSaveGame>(UGameplayStatics::CreateSaveGameObject(UROWSaveGame::StaticClass()));
    if(!TestNotNull(TEXT("Save object"),Save))return false;
    const auto StoryText=Story.Serialize(),MatchText=Match.Serialize();
    Save->NarrativeSnapshot=UTF8_TO_TCHAR(StoryText.c_str());Save->CombatSnapshot=UTF8_TO_TCHAR(MatchText.c_str());
    Save->Generation=42;Save->PlayerLocation=FVector(-2700,3200,60);Save->OpponentLocation=FVector(-2700,3350,60);Save->TextScale=1.4f;
    TArray<uint8> Bytes;if(!TestTrue(TEXT("Unreal serializes match checkpoint"),UGameplayStatics::SaveGameToMemory(Save,Bytes)))return false;
    auto* Loaded=Cast<UROWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
    if(!TestNotNull(TEXT("Unreal deserializes checkpoint"),Loaded))return false;
    ROWCombat::Match Restored;ROWNarrative::Session RestoredStory;
    TestTrue(TEXT("Match snapshot validates"),Restored.Restore(TCHAR_TO_UTF8(*Loaded->CombatSnapshot)));
    TestTrue(TEXT("Narrative snapshot validates"),RestoredStory.Restore(TCHAR_TO_UTF8(*Loaded->NarrativeSnapshot)));
    TestEqual(TEXT("Schema preserved"),Loaded->Schema,2);
    TestEqual(TEXT("Generation preserved"),Loaded->Generation,static_cast<int64>(42));
    TestEqual(TEXT("Windup time preserved"),Restored.GetState().PhaseLeft,Match.GetState().PhaseLeft);
    TestTrue(TEXT("Opponent transform preserved"),Loaded->OpponentLocation.Equals(Save->OpponentLocation));
    TestEqual(TEXT("Text scaling preserved"),Loaded->TextScale,1.4f);
    Match.Tick(.6,Frame,false);Restored.Tick(.6,Frame,false);
    TestTrue(TEXT("AI continues identically after Unreal serialization"),Match.Serialize()==Restored.Serialize());
    TestEqual(TEXT("Placement unaffected by combat checkpoint"),RestoredStory.GetState().PlacementScore,100);
    return true;
}
#endif
