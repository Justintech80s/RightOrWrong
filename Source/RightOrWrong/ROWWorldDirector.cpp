#include "ROWWorldDirector.h"
#include "ROWCharacter.h"
#include "ROWOpponent.h"
#include "ROWBlockout.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ROWTrainingTarget.h"
#include "ROWSaveGame.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Engine/HitResult.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/DirectionalLight.h"
#include "Engine/SkyLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/LightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
namespace
{
    constexpr TCHAR Slot[] = TEXT("RightOrWrong_WrittenNarrative_1");
    constexpr TCHAR BackupSlot[] = TEXT("RightOrWrong_WrittenNarrative_1_Backup");
    const FVector RingCenter(-2700,3300,60);
    const FVector PlayerStart(-2700,3000,60);
    const FVector OpponentStart(-2700,3600,60);
    bool InRing(const FVector& P){return FMath::Abs(P.X-RingCenter.X)<=700&&FMath::Abs(P.Y-RingCenter.Y)<=650;}
    bool SanePosition(const FVector& P){return !P.ContainsNaN()&&FMath::Abs(P.X)<4500&&FMath::Abs(P.Y)<4500&&P.Z>-500&&P.Z<2500;}
    FString Text(const std::string& Value) { return FString(UTF8_TO_TCHAR(Value.c_str())); }
    FVector Site(const char* Name,double Z=-35)
    {
        for(const auto& P:ROWBlockout::Destinations())if(std::string(P.Name)==Name)return FVector(P.X,P.Y,Z);
        checkNoEntry();return FVector(0,0,Z);
    }
    const FVector Stations[] = {Site("Karate",-45),Site("Judo",-45),Site("Taekwondo",-45),Site("Archery",-45),Site("Fencing",-45),Site("Sport shooting",-45)};
}
AROWWorldDirector::AROWWorldDirector() { PrimaryActorTick.bCanEverTick = true; }
void AROWWorldDirector::BeginPlay() { Super::BeginPlay(); GeneratePrototypeNeighborhood(); }
AStaticMeshActor* AROWWorldDirector::PlaceCube(const FVector& Center,const FVector& Size)
{
    UStaticMesh* Cube = LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (!GetWorld() || !Cube) return nullptr;
    auto* Actor = GetWorld()->SpawnActor<AStaticMeshActor>(Center,FRotator::ZeroRotator);
    if (!Actor) return nullptr;
    auto* Mesh = Actor->GetStaticMeshComponent(); Mesh->SetMobility(EComponentMobility::Movable);
    Mesh->SetStaticMesh(Cube); Mesh->SetWorldScale3D(Size/100.f); Mesh->SetCollisionProfileName(TEXT("BlockAll"));
    Mesh->SetMobility(EComponentMobility::Static); return Actor;
}
AROWVenue* AROWWorldDirector::PlaceVenue(EROWVenueType Type,const FString& Label,const FVector& Location)
{
    auto* Venue = GetWorld()->SpawnActor<AROWVenue>(Location,FRotator::ZeroRotator);
    if (Venue) { Venue->InitializeVenue(Type,Label); Venues.Add(Venue); } return Venue;
}
void AROWWorldDirector::GeneratePrototypeNeighborhood()
{
    const FLinearColor Palette[]={FLinearColor(.18,.23,.2),FLinearColor(.43,.45,.46),FLinearColor(.3,.35,.42),
        FLinearColor(.07,.08,.1),FLinearColor(.9,.83,.57),FLinearColor(.42,.23,.17),FLinearColor(.27,.17,.08),
        FLinearColor(.12,.3,.12),FLinearColor(.09,.24,.36)};
    auto* Base=LoadObject<UMaterialInterface>(nullptr,TEXT("/Game/Materials/M_ROWColor.M_ROWColor"));
    TArray<UMaterialInstanceDynamic*> Materials;
    for(const auto& Color:Palette)
    {
        auto* Material=Base?UMaterialInstanceDynamic::Create(Base,this):nullptr;
        if(Material)Material->SetVectorParameterValue(TEXT("Tint"),Color);
        Materials.Add(Material);
    }
    for(const auto& B:ROWBlockout::Geometry())
        if(auto* A=PlaceCube(FVector(B.X,B.Y,B.Z),FVector(B.W,B.D,B.H)))
            if(Materials[B.Palette])A->GetStaticMeshComponent()->SetMaterial(0,Materials[B.Palette]);
    PlaceVenue(EROWVenueType::RecruitmentAcademy,TEXT("VANGUARD ACADEMY / GENERAL WARD"),Site("General Ward"));
    PlaceVenue(EROWVenueType::CommunityCoach,TEXT("COACH IMANI BROOKS"),Site("Coach Brooks"));
    PlaceVenue(EROWVenueType::RivalRecruit,TEXT("JORDAN CROSS"),Site("Jordan Cross"));
    PlaceVenue(EROWVenueType::InitialContact,TEXT("ACADEMY GATE / VISITING CONTACT"),Site("Visiting contact"));
    PlaceVenue(EROWVenueType::AcademyGraduation,TEXT("ACADEMY EVALUATION BOARD"),Site("Evaluation"));
    PlaceVenue(EROWVenueType::PoliceStation,TEXT("FIELD ASSIGNMENT DESK"),Site("Assignment"));
    PlaceVenue(EROWVenueType::Evidence,TEXT("SOUTH WARD / DISPUTED RECORDS"),Site("Records"));
    PlaceVenue(EROWVenueType::FollowUpCommand,TEXT("VANGUARD PROCUREMENT OFFICE"),Site("Procurement"));
    PlaceVenue(EROWVenueType::FollowUpOversight,TEXT("CIVILIAN OVERSIGHT OFFICE"),Site("Oversight"));
    PlaceVenue(EROWVenueType::FollowUpFixer,TEXT("DOCKLANDS FIXER"),Site("Fixer"));
    PlaceVenue(EROWVenueType::SparringRing,TEXT("SUPERVISED SPARRING / COACH BROOKS"),Site("Sparring"));
    Opponent=GetWorld()->SpawnActor<AROWOpponent>(OpponentStart,FRotator(0,-90,0));
    if(auto* V=PlaceVenue(EROWVenueType::DistrictInfo,TEXT("SOUTH WARD MARKET"),Site("Market")))
        V->Description=TEXT("The market noticeboard advertises fresh produce, council consultation, and a consultation about why the last consultation achieved nothing. The proprietor points you toward the records office south-east. These streets are an approved academy field-exercise area; you remain a recruit until the evaluation board clears you.");
    if(auto* V=PlaceVenue(EROWVenueType::DistrictInfo,TEXT("GREYHAVEN / NIGHT SHIFT CAFE"),Site("Cafe")))
        V->Description=TEXT("A handwritten menu offers coffee, pie, and plausible deniability. The owner says procurement vans visit the records office after closing. The fixer waits outside by the loading lane. You can explore before accepting an assignment; the records become mission evidence only after briefing.");
    for (int32 i=0;i<6;++i)
    {
        auto* Venue = PlaceVenue(EROWVenueType::TrainingStation,Text(ROWRules::SportName(i))+TEXT(" TRAINING"),Stations[i]);
        if (Venue) Venue->SportIndex = i;
        PlaceCube(Stations[i]+FVector(0,0,-1),FVector(450,400,8));
        const FVector Target = Stations[i]+FVector(0,(i==3||i==5)?1100:350,(i==3||i==5)?175:85);
        auto* Actor = GetWorld()->SpawnActor<AROWTrainingTarget>(Target,FRotator::ZeroRotator);
        TrainingTargets.Add(Actor); if (Actor) Actor->Configure(i);
    }
    GetWorld()->SpawnActor<ASkyAtmosphere>(FVector::ZeroVector,FRotator::ZeroRotator);
    auto* Sun = GetWorld()->SpawnActor<ADirectionalLight>(FVector(0,0,2200),FRotator(-55,-30,0));
    if(Sun)if(auto* Light=Cast<UDirectionalLightComponent>(Sun->GetLightComponent()))
    {Light->SetMobility(EComponentMobility::Movable);Light->bAtmosphereSunLight=true;Light->SetIntensity(10000.f);Light->MarkRenderStateDirty();}
    auto* Sky = GetWorld()->SpawnActor<ASkyLight>(FVector(0,0,1800),FRotator::ZeroRotator);
    if(Sky&&Sky->GetLightComponent())
    {Sky->GetLightComponent()->SetMobility(EComponentMobility::Movable);Sky->GetLightComponent()->SetIntensity(1.f);Sky->GetLightComponent()->SetRealTimeCapture(true);}
}
AROWVenue* AROWWorldDirector::FindNearbyVenue(const FVector& Position) const
{
    AROWVenue* Best=nullptr; float Range=300.f;
    for (AROWVenue* V:Venues) if (IsValid(V)) { const float D=FVector::Dist2D(Position,V->GetActorLocation()); if(D<Range){Range=D;Best=V;} }
    return Best;
}
bool AROWWorldDirector::IsModal() const { return bMenu || bJournal || bSparringPrompt || Combat.GetState().ResultPending || Narrative.IsModal(); }
bool AROWWorldDirector::IsProfileScreen() const { return !bMenu && !bJournal && Narrative.GetDialogue()==ROWNarrative::Dialogue::Profile; }
void AROWWorldDirector::HandleInteraction(AROWCharacter* Player)
{
    if (!Player || IsModal() || Combat.IsActive()) return;
    const auto* V = FindNearbyVenue(Player->GetActorLocation());
    if (!V) { Notice=TEXT("Approach a named destination and press E."); return; }
    if(V->VenueType==EROWVenueType::SparringRing)
    {
        if(Narrative.IsBusy()){Notice=TEXT("Complete the active assessment first.");return;}
        if(Narrative.GetState().PlacementScore<60){Narrative.ShowInfo("Complete your chosen program assessment first. Then Coach Brooks will supervise an optional sparring exhibition. No change of sport is required.");return;}
        bSparringPrompt=true;ResetHeldInput();Notice.Empty();return;
    }
    if(V->VenueType==EROWVenueType::DistrictInfo&&!V->Description.IsEmpty())
    {Narrative.ShowInfo(TCHAR_TO_UTF8(*V->Description));Notice.Empty();return;}
    using P=ROWNarrative::Place; P Place=P::District;
    switch(V->VenueType)
    {
        case EROWVenueType::RecruitmentAcademy: Place=P::Academy; break;
        case EROWVenueType::CommunityCoach: Place=P::Mentor; break;
        case EROWVenueType::TrainingStation: Place=P::Training; break;
        case EROWVenueType::RivalRecruit: Place=P::Recruit; break;
        case EROWVenueType::InitialContact: Place=P::Contact; break;
        case EROWVenueType::PoliceStation: Place=P::Assignment; break;
        case EROWVenueType::Evidence: Place=P::Evidence; break;
        case EROWVenueType::FollowUpCommand: Place=P::Command; break;
        case EROWVenueType::FollowUpOversight: Place=P::Oversight; break;
        case EROWVenueType::FollowUpFixer: Place=P::Fixer; break;
        case EROWVenueType::AcademyGraduation: Place=P::Graduation; break;
        default: break;
    }
    Notice.Empty(); Narrative.Visit(Place,V->SportIndex);
}
void AROWWorldDirector::RefreshAppearance()
{
    const auto& S=Narrative.GetState();
    if (auto* Player=Cast<AROWCharacter>(UGameplayStatics::GetPlayerPawn(this,0))) Player->ApplyProfile(S.Presentation,S.Outfit);
}
void AROWWorldDirector::HandleChoice(int32 Choice)
{
    if (bJournal) { if(Choice==1)bJournal=false; return; }
    if (bMenu)
    {
        if(bConfirmNew)
        {
            if(Choice==1){ Narrative=ROWNarrative::Session(); Combat=ROWCombat::Match();bSparringPrompt=false;ResetHeldInput();if(Opponent){Opponent->StopMovement();Opponent->SetActorLocation(OpponentStart,false,nullptr,ETeleportType::TeleportPhysics);} PendingName=TEXT("Recruit");bMenu=false;bConfirmNew=false;bDrawing=false;bGuarding=false;DrawSeconds=GuardSeconds=0;Notice.Empty();
                if(auto* P=UGameplayStatics::GetPlayerPawn(this,0))P->SetActorLocation(FVector(0,0,150),false,nullptr,ETeleportType::TeleportPhysics); RefreshAppearance(); }
            else if(Choice==2)bConfirmNew=false;
            return;
        }
        switch(Choice)
        {
            case 1:bMenu=false;break;
            case 2:SaveProgress();break;
            case 3:LoadProgress();break;
            case 4:bMenu=false;bJournal=true;break;
            case 5:TextScale=TextScale<1.19f?1.2f:TextScale<1.39f?1.4f:1.f;break;
            case 6:bConfirmNew=true;break;
            default:break;
        }
        return;
    }
    if(bSparringPrompt)
    {
        if(Choice==1)StartSparring();
        else if(Choice==2)bSparringPrompt=false;
        return;
    }
    if(Combat.GetState().ResultPending)
    {
        if(Choice==1)StartSparring();
        else if(Choice==2){Combat.DismissResult();Notice=TEXT("Sparring result recorded. Continue your mission or explore Greyhaven.");}
        return;
    }
    if(IsProfileScreen())
    {
        PendingName=PendingName.TrimStartAndEnd().Left(24);
        if(!Narrative.SetName(TCHAR_TO_UTF8(*PendingName))){ Notice=TEXT("Enter a name containing at least one letter or symbol.");return; }
    }
    const bool WasActive=Narrative.GetAssessment().Active;
    Notice.Empty();
    if(Narrative.Choose(Choice))RefreshAppearance();
    if(!WasActive && Narrative.GetAssessment().Active)
    {
        if(auto* P=UGameplayStatics::GetPlayerPawn(this,0))StepOrigin=P->GetActorLocation();
        DrawSeconds=GuardSeconds=0; bDrawing=bGuarding=false;
    }
}
void AROWWorldDirector::AdvanceSingleChoice() { if(PanelOptions().Num()==1)HandleChoice(1); }
void AROWWorldDirector::ToggleMenu() { ResetHeldInput(); if(bJournal){bJournal=false;return;} bMenu=!bMenu;bConfirmNew=false; }
void AROWWorldDirector::OpenJournal() { ResetHeldInput(); if(bJournal){bJournal=false;return;}bJournal=true;bMenu=false; }
ROWNarrative::Frame AROWWorldDirector::TrainingFrame() const
{
    ROWNarrative::Frame F; const int32 Sport=Narrative.GetAssessment().Sport;
    const auto* P=UGameplayStatics::GetPlayerPawn(this,0);
    auto* PC=UGameplayStatics::GetPlayerController(this,0);
    if(!P||!PC||!TrainingTargets.IsValidIndex(Sport)||!TrainingTargets[Sport])return F;
    const FVector Target=TrainingTargets[Sport]->GetAimPoint();
    F.Distance=FVector::Dist2D(P->GetActorLocation(),Target);
    F.LateralTravel=FMath::Abs((P->GetActorLocation()-StepOrigin).X);
    F.HoldSeconds=Sport==3?DrawSeconds:GuardSeconds;
    FVector Eye; FRotator Rotation; PC->GetPlayerViewPoint(Eye,Rotation); const FVector Direction=Rotation.Vector();
    if(Sport==3||Sport==5)
    {
        if(FMath::Abs(Direction.Y)>.001)
        {
            const double T=(Target.Y-Eye.Y)/Direction.Y;
            if(T>0)
            {
                const FVector Impact=Eye+Direction*T; const double Error=FVector2D(Impact.X-Target.X,Impact.Z-Target.Z).Size();
                F.Aim=FMath::Clamp(1.0-Error/100.0,0.0,1.0);
                FHitResult Hit; FCollisionQueryParams Params;Params.AddIgnoredActor(P);
                if(GetWorld()->LineTraceSingleByChannel(Hit,Eye,Impact+Direction*20,ECC_Visibility,Params) && Hit.GetActor()!=TrainingTargets[Sport].Get())F.Aim=0;
            }
        }
    }
    else F.Aim=FMath::Clamp((FVector::DotProduct(Direction.GetSafeNormal2D(),(Target-P->GetActorLocation()).GetSafeNormal2D())-.3)/.7,0.0,1.0);
    return F;
}
void AROWWorldDirector::AfterAction(int32 OldStep)
{
    if(OldStep==Narrative.GetAssessment().Step)return;
    if(auto* P=UGameplayStatics::GetPlayerPawn(this,0))StepOrigin=P->GetActorLocation();
    const int32 Sport=Narrative.GetAssessment().Sport;
    if(TrainingTargets.IsValidIndex(Sport)&&TrainingTargets[Sport])TrainingTargets[Sport]->React();
    Notice.Empty();
}
void AROWWorldDirector::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    auto* Player=UGameplayStatics::GetPlayerPawn(this,0);
    if(Opponent&&Player)Opponent->UpdateMatch(Combat.GetState(),Player->GetActorLocation(),IsModal());
    if(IsModal())return;
    if(Player&&Player->GetActorLocation().Z < -400)
    {
        Combat.Forfeit();ResetHeldInput();Player->SetActorLocation(FVector(0,0,150),false,nullptr,ETeleportType::TeleportPhysics);
        Notice=TEXT("Returned to the courtyard after leaving the playable ground.");return;
    }
    if(Combat.IsActive())
    {
        Combat.Tick(DeltaSeconds,CombatFrame(),bGuarding);
        if(!Combat.IsActive())ResetHeldInput();
        return;
    }
    if(bDrawing)DrawSeconds+=DeltaSeconds;
    if(bGuarding)GuardSeconds+=DeltaSeconds;
    const int32 OldStep=Narrative.GetAssessment().Step;
    Narrative.Tick(DeltaSeconds,TrainingFrame()); AfterAction(OldStep);
}
void AROWWorldDirector::HandleAttack(AROWCharacter* Player)
{
    if(IsModal())return;
    if(Combat.IsActive())
    {
        if(Combat.Strike(CombatFrame(),bGuarding)&&Player)Player->ShowStrike();Notice.Empty();
        if(!Combat.IsActive())ResetHeldInput();return;
    }
    if(!Narrative.GetAssessment().Active)return;
    const int32 Sport=Narrative.GetAssessment().Sport;
    if(Sport==3){bDrawing=true;DrawSeconds=0;return;}
    if(Player)Player->ShowStrike();
    const int32 Old=Narrative.GetAssessment().Step;
    Narrative.Act(Sport==5?ROWNarrative::Action::Shot:ROWNarrative::Action::Strike,TrainingFrame());AfterAction(Old);
}
void AROWWorldDirector::HandleAttackReleased()
{
    if(bDrawing&&!IsModal()) { const int32 Old=Narrative.GetAssessment().Step;Narrative.Act(ROWNarrative::Action::Release,TrainingFrame());AfterAction(Old); }
    bDrawing=false;DrawSeconds=0;
}
void AROWWorldDirector::HandleGuard(bool bPressed)
{
    if(bPressed&&IsModal())return;
    bGuarding=bPressed;GuardSeconds=0;
    if(!bPressed||IsModal()||!Narrative.GetAssessment().Active||Narrative.GetAssessment().Sport==5)return;
    const int32 Old=Narrative.GetAssessment().Step;Narrative.Act(ROWNarrative::Action::Guard,TrainingFrame());AfterAction(Old);
}
void AROWWorldDirector::ResetHeldInput()
{
    bDrawing=bGuarding=false;DrawSeconds=GuardSeconds=0;
    if(auto* P=Cast<AROWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))P->ResetHeldInput();
    if(Opponent)Opponent->StopMovement();
}
void AROWWorldDirector::StartSparring()
{
    auto* Player=UGameplayStatics::GetPlayerPawn(this,0);
    if(!Player||!Opponent){Notice=TEXT("The sparring actors are unavailable. Return to the courtyard and retry.");return;}
    if(Narrative.IsBusy()||Narrative.GetState().PlacementScore<60)return;
    ResetHeldInput();
    Player->SetActorLocationAndRotation(PlayerStart,FRotator(0,90,0),false,nullptr,ETeleportType::TeleportPhysics);
    Opponent->SetActorLocationAndRotation(OpponentStart,FRotator(0,-90,0),false,nullptr,ETeleportType::TeleportPhysics);
    if(auto* PC=UGameplayStatics::GetPlayerController(this,0))PC->SetControlRotation(FRotator(-15,90,0));
    Combat.Start();bSparringPrompt=false;Notice.Empty();
}
ROWCombat::Frame AROWWorldDirector::CombatFrame() const
{
    ROWCombat::Frame F;const auto* Player=UGameplayStatics::GetPlayerPawn(this,0);
    const auto* PC=UGameplayStatics::GetPlayerController(this,0);if(!Player||!PC||!Opponent)return F;
    const FVector From=Player->GetActorLocation(),To=Opponent->GetActorLocation();
    F.Distance=FVector::Dist2D(From,To);F.HeightDifference=To.Z-From.Z;
    F.Facing=FMath::Clamp(FVector::DotProduct(PC->GetControlRotation().Vector().GetSafeNormal2D(),(To-From).GetSafeNormal2D()),-1.0,1.0);
    F.PlayerInsideRing=InRing(From);
    FCollisionQueryParams Params;Params.AddIgnoredActor(Player);Params.AddIgnoredActor(Opponent);
    FHitResult Hit;F.LineOfSight=!GetWorld()->LineTraceSingleByChannel(Hit,From,To,ECC_Visibility,Params);
    return F;
}
FString AROWWorldDirector::PanelTitle() const
{
    if(bJournal)return TEXT("Your journal");
    if(bMenu)return bConfirmNew?TEXT("Begin a new recruit?"):TEXT("Paused");
    if(bSparringPrompt)return TEXT("Supervised sparring exhibition");
    if(Combat.GetState().ResultPending)return Text(Combat.Cue());
    return Text(Narrative.GetView().Title);
}
FString AROWWorldDirector::PanelSpeaker() const { return bMenu||bJournal?TEXT("RIGHT OR WRONG"):bSparringPrompt||Combat.GetState().ResultPending?TEXT("Coach Imani Brooks"):Text(Narrative.GetView().Speaker); }
FString AROWWorldDirector::PanelBody() const
{
    if(bJournal){ FString All;for(const auto& Entry:Narrative.GetJournal()){All+=Text(Entry)+TEXT("\n\n");}return All; }
    if(bMenu)return bConfirmNew?TEXT("This replaces the recruit in memory. Your existing saved game remains until you save again."):TEXT("Your assessment clock is paused. Conversations wait for you to advance them.");
    if(bSparringPrompt)return TEXT("One opponent. Two minutes. This is a supervised exhibition, not a change of athletic program. Move with WASD and face the opponent with the camera. LMB strikes within 2.2 metres; hold RMB to guard. Release guard to attack. A red opponent and written warning signal an incoming strike. Counter when they recover. Blocking costs stamina; recover between attacks. Leaving the painted ring forfeits. Your chosen program and contact placement stay intact.");
    if(Combat.GetState().ResultPending)
    {
        const auto& S=Combat.GetState();
        return FString::Printf(TEXT("%s\n\nYour health: %d / Opponent: %d\nExhibition record: %d wins, %d losses, %d draws.\n\nBrooks: 'The lesson is timing. The paperwork calls it resilience. Either way, you get another try.'"),*Text(Combat.Feedback),S.PlayerHealth,S.OpponentHealth,S.Wins,S.Losses,S.Draws);
    }
    return Text(Narrative.GetView().Text);
}
TArray<FString> AROWWorldDirector::PanelOptions() const
{
    if(bJournal)return {TEXT("Return")};
    if(bMenu)return bConfirmNew?TArray<FString>{TEXT("Start a new recruit"),TEXT("Keep playing")}:TArray<FString>{TEXT("Resume"),TEXT("Save progress"),TEXT("Load saved progress"),TEXT("Read journal"),FString::Printf(TEXT("Dialogue text size: %d%%"),FMath::RoundToInt(TextScale*100)),TEXT("New recruit")};
    if(bSparringPrompt)return {TEXT("Begin exhibition"),TEXT("Return to the academy")};
    if(Combat.GetState().ResultPending)return {TEXT("Rematch"),TEXT("Continue exploring")};
    TArray<FString> R;for(const auto& X:Narrative.GetView().Options)R.Add(Text(X));return R;
}
FString AROWWorldDirector::GetObjectiveText() const { return Combat.IsActive()?TEXT("SPARRING: ")+Text(Combat.Cue()):Text(Narrative.Objective()); }
FString AROWWorldDirector::GetMessage() const { return Notice.IsEmpty()?Text(Combat.IsActive()||Combat.GetState().ResultPending?Combat.Feedback:Narrative.Feedback):Notice; }
FString AROWWorldDirector::GetStatusText() const
{
    const auto& S=Narrative.GetState();const auto& A=Narrative.GetAssessment();
    FString Result=Text(S.Name)+TEXT(" | ")+Text(Narrative.SportName());
    if(S.PlacementScore>=0)Result+=FString::Printf(TEXT(" | Grade %s (%d/100)"),UTF8_TO_TCHAR(ROWNarrative::Grade(S.PlacementScore)),S.PlacementScore);
    if(Combat.IsActive())
    {
        const auto& C=Combat.GetState();
        return FString::Printf(TEXT("SPARRING | You: %d | Opponent: %d | Stamina: %d%% | %.0f seconds"),C.PlayerHealth,C.OpponentHealth,FMath::RoundToInt(C.Stamina),C.SecondsLeft);
    }
    if(A.Active){const auto F=TrainingFrame();Result+=FString::Printf(TEXT(" | Action %d/6 | %.1fs | Range %.0f cm | Aim %d%% | Hold %.1fs"),A.Step+1,12-A.Elapsed,F.Distance,FMath::RoundToInt(F.Aim*100),F.HoldSeconds);}
    return Result;
}
FString AROWWorldDirector::GetNearbyActionText(const AROWCharacter* Player) const
{
    if(!Player||IsModal()||Narrative.GetAssessment().Active||Combat.IsActive())return FString();
    const auto* V=FindNearbyVenue(Player->GetActorLocation());return V?TEXT("E: ")+V->VenueLabel:FString();
}
FVector AROWWorldDirector::GetObjectiveLocation() const
{
    if(Combat.IsActive()&&Opponent)return Opponent->GetActorLocation()+FVector(0,0,120);
    const auto& S=Narrative.GetState();EROWVenueType Desired=EROWVenueType::RecruitmentAcademy;
    if(!S.Briefed||S.Sport<0)Desired=EROWVenueType::RecruitmentAcademy;
    else if(!S.Mentored)Desired=EROWVenueType::CommunityCoach;
    else if(S.PlacementScore<0)Desired=EROWVenueType::TrainingStation;
    else if(S.RecruitChoice==0)Desired=EROWVenueType::RivalRecruit;
    else if(S.ContactChoice==0)Desired=EROWVenueType::InitialContact;
    else if(!S.Assigned)Desired=S.ContactChoice==2?EROWVenueType::RecruitmentAcademy:EROWVenueType::InitialContact;
    else if(!S.EvidenceFound)Desired=EROWVenueType::Evidence;
    else if(S.FollowUpChoice==0)Desired=S.CaseChoice==1?EROWVenueType::FollowUpCommand:S.CaseChoice==2?EROWVenueType::FollowUpOversight:EROWVenueType::FollowUpFixer;
    else Desired=EROWVenueType::AcademyGraduation;
    if(Narrative.GetAssessment().Active){const int32 I=Narrative.GetAssessment().Sport;if(TrainingTargets.IsValidIndex(I)&&TrainingTargets[I])return TrainingTargets[I]->GetAimPoint();}
    for(AROWVenue* V:Venues)if(IsValid(V)&&V->VenueType==Desired&&(Desired!=EROWVenueType::TrainingStation||V->SportIndex==S.Sport))return V->GetActorLocation()+FVector(0,0,230);
    return FVector::ZeroVector;
}
FString AROWWorldDirector::GetInitialContactNetwork() const
{
    const auto N=ROWStoryRouting::NetworkForSport(Narrative.GetState().Sport);
    return N==ROWStoryRouting::ContactNetwork::LawEnforcement?TEXT("LawEnforcement"):N==ROWStoryRouting::ContactNetwork::CriminalUnderworld?TEXT("CriminalUnderworld"):TEXT("Unselected");
}
FString AROWWorldDirector::GetInitialContactTier() const
{
    using T=ROWStoryRouting::ContactTier;switch(ROWNarrative::Tier(Narrative.GetState().PlacementScore))
    {case T::Elite:return TEXT("Elite");case T::Established:return TEXT("Established");case T::Neighborhood:return TEXT("Neighborhood");case T::Freelance:return TEXT("Freelance");default:return TEXT("Unavailable");}
}
FString AROWWorldDirector::GetInitialContactDescription() const { return Text(Narrative.ContactDescription()); }
bool AROWWorldDirector::ValidateSave(const UROWSaveGame* Save,ROWNarrative::Session& Story,ROWCombat::Match& Match) const
{
    if(!Save||(Save->Schema!=1&&Save->Schema!=2)||Save->Generation<0||Save->Generation>=1000000000000LL)return false;
    if(!SanePosition(Save->PlayerLocation)||!SanePosition(Save->TrainingStepOrigin)||Save->PlayerRotation.ContainsNaN()||Save->ViewRotation.ContainsNaN()||!FMath::IsFinite(Save->TextScale)||Save->TextScale<1||Save->TextScale>1.4f)return false;
    if(!Story.Restore(TCHAR_TO_UTF8(*Save->NarrativeSnapshot)))return false;
    if(Save->Schema==2)
    {
        if(!SanePosition(Save->OpponentLocation)||!Match.Restore(TCHAR_TO_UTF8(*Save->CombatSnapshot)))return false;
        if((Match.IsActive()||Match.GetState().ResultPending||Save->bSparringPrompt)&&(Story.IsBusy()||Story.GetState().PlacementScore<60))return false;
        if(Save->bSparringPrompt&&(Match.IsActive()||Match.GetState().ResultPending))return false;
        if(Match.IsActive()&&(!InRing(Save->PlayerLocation)||!InRing(Save->OpponentLocation)))return false;
    }
    return true;
}
void AROWWorldDirector::SaveProgress()
{
    // Rotate two complete checkpoints: a failed write cannot overwrite the newest valid save.
    const TCHAR* Slots[]={Slot,BackupSlot};int32 Newest=-1;int64 Generation=0;
    for(int32 I=0;I<2;++I)
    {
        const auto* Existing=Cast<UROWSaveGame>(UGameplayStatics::LoadGameFromSlot(Slots[I],0));
        ROWNarrative::Session Story;ROWCombat::Match Match;
        if(ValidateSave(Existing,Story,Match)&&(Newest<0||Existing->Generation>Generation)){Newest=I;Generation=Existing->Generation;}
    }
    auto* Save=Cast<UROWSaveGame>(UGameplayStatics::CreateSaveGameObject(UROWSaveGame::StaticClass()));
    if(!Save){Notice=TEXT("Save could not be created. Current progress is unchanged.");return;}
    Save->Generation=Generation+1;Save->NarrativeSnapshot=Text(Narrative.Serialize());Save->CombatSnapshot=Text(Combat.Serialize());
    Save->bSparringPrompt=bSparringPrompt;Save->TrainingStepOrigin=StepOrigin;Save->TextScale=TextScale;
    if(Opponent)Save->OpponentLocation=Opponent->GetActorLocation();
    if(auto* P=UGameplayStatics::GetPlayerPawn(this,0)){Save->PlayerLocation=P->GetActorLocation();Save->PlayerRotation=P->GetActorRotation();}
    if(auto* PC=UGameplayStatics::GetPlayerController(this,0))Save->ViewRotation=PC->GetControlRotation();
    ROWNarrative::Session CheckStory;ROWCombat::Match CheckMatch;
    if(!ValidateSave(Save,CheckStory,CheckMatch)){Notice=TEXT("Current checkpoint is not safe to save. Move onto the playable ground and retry.");return;}
    const TCHAR* Destination=Slots[Newest==0?1:0];
    if(!UGameplayStatics::SaveGameToSlot(Save,Destination,0)){Notice=TEXT("Save failed. The previous valid checkpoint is retained.");return;}
    const auto* ReadBack=Cast<UROWSaveGame>(UGameplayStatics::LoadGameFromSlot(Destination,0));
    CheckStory=ROWNarrative::Session();CheckMatch=ROWCombat::Match();
    const bool Verified=ValidateSave(ReadBack,CheckStory,CheckMatch)&&ReadBack->Generation==Save->Generation&&ReadBack->NarrativeSnapshot==Save->NarrativeSnapshot&&ReadBack->CombatSnapshot==Save->CombatSnapshot&&ReadBack->PlayerLocation.Equals(Save->PlayerLocation,.01)&&ReadBack->OpponentLocation.Equals(Save->OpponentLocation,.01)&&ReadBack->Schema==Save->Schema&&ReadBack->PlayerRotation.Equals(Save->PlayerRotation,.01)&&ReadBack->ViewRotation.Equals(Save->ViewRotation,.01)&&ReadBack->TrainingStepOrigin.Equals(Save->TrainingStepOrigin,.01)&&ReadBack->TextScale==Save->TextScale&&ReadBack->bSparringPrompt==Save->bSparringPrompt;
    Notice=Verified?TEXT("Checkpoint saved and read back. Dialogue, assessment, sparring and location are retained."):TEXT("Checkpoint verification failed. The previous valid save is retained.");
}
void AROWWorldDirector::LoadProgress()
{
    const TCHAR* Slots[]={Slot,BackupSlot};const UROWSaveGame* Best=nullptr;
    ROWNarrative::Session Story;ROWCombat::Match Match;bool InvalidFound=false;
    for(const TCHAR* Candidate:Slots)
    {
        if(!UGameplayStatics::DoesSaveGameExist(Candidate,0))continue;
        const auto* Save=Cast<UROWSaveGame>(UGameplayStatics::LoadGameFromSlot(Candidate,0));
        ROWNarrative::Session CandidateStory;ROWCombat::Match CandidateMatch;
        if(!ValidateSave(Save,CandidateStory,CandidateMatch)){InvalidFound=true;continue;}
        if(!Best||Save->Generation>Best->Generation){Best=Save;Story=CandidateStory;Match=CandidateMatch;}
    }
    if(!Best){Notice=InvalidFound?TEXT("No valid checkpoint could be read. Your current session was preserved."):TEXT("No saved checkpoint exists. Create your recruit and press F5 to save.");return;}
    if(Match.IsActive()&&!Opponent){Notice=TEXT("Cannot restore this match because its opponent is missing. Current progress was preserved.");return;}
    Narrative=Story;Combat=Match;ResetHeldInput();bMenu=bJournal=bConfirmNew=false;
    bSparringPrompt=Best->Schema==2&&Best->bSparringPrompt;
    StepOrigin=Best->TrainingStepOrigin;TextScale=Best->TextScale;PendingName=Text(Narrative.GetState().Name);
    FVector Location=Best->PlayerLocation;
    const bool Adjusted=!ROWBlockout::Walkable(Location.X,Location.Y)||Location.Z < -25;
    if(Adjusted)Location=Combat.IsActive()?PlayerStart:FVector(0,0,150);
    if(auto* P=UGameplayStatics::GetPlayerPawn(this,0))P->SetActorLocationAndRotation(Location,Best->PlayerRotation,false,nullptr,ETeleportType::TeleportPhysics);
    if(Opponent)Opponent->SetActorLocation(Best->Schema==2?Best->OpponentLocation:OpponentStart,false,nullptr,ETeleportType::TeleportPhysics);
    if(auto* PC=UGameplayStatics::GetPlayerController(this,0))PC->SetControlRotation(Best->ViewRotation);
    RefreshAppearance();
    Notice=InvalidFound?TEXT("Recovered the newest valid checkpoint; an unreadable checkpoint was ignored."):TEXT("Progress loaded. Re-press held controls to resume.");
    if(Adjusted)Notice+=TEXT(" Position was moved to safe ground.");
}
