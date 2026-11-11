#include "../Source/RightOrWrong/ROWCombatRules.h"
#include "../Source/RightOrWrong/ROWNarrative.h"
#include <cstdlib>
#include <iostream>
#include <limits>
using namespace ROWCombat;
struct CommaDecimal : std::numpunct<char> { char do_decimal_point() const override {return ',';} };
static int Checks=0;
static void Check(bool Value,const char* Label)
{++Checks;if(!Value){std::cerr<<"FAIL: "<<Label<<'\n';std::exit(1);}}
static Frame Close(){return {150,1,0,true,true};}
static void Advance(Match& M,double Seconds,const Frame& F,bool Guard=false)
{while(Seconds>1e-8){const double Dt=std::min(Seconds,.05);M.Tick(Dt,F,Guard);Seconds-=Dt;}}
static void RoundTrip(const Match& M)
{
    Match Copy;Check(Copy.Restore(M.Serialize()),"valid state restores");
    Check(Copy.Serialize()==M.Serialize(),"all match state retained");
}
int main()
{
    Match M;RoundTrip(M);Check(!M.Strike(Close(),false),"no attack outside match");
    M.Start();Check(M.IsActive(),"start match");
    M.Strike({500,1,0,true,true},false);Check(M.GetState().OpponentHealth==100,"distance prevents remote hit");
    Check(M.GetState().Stamina==82,"miss spends stamina");
    Check(!M.Strike(Close(),false),"cooldown blocks spam");
    Advance(M,.6,{500,1,0,true,true});M.Strike({150,-1,0,true,true},false);
    Check(M.GetState().OpponentHealth==100,"must face opponent");
    Advance(M,.6,{500,1,0,true,true});M.Strike({150,1,0,false,true},false);
    Check(M.GetState().OpponentHealth==100,"walls prevent hits");
    Advance(M,.6,{500,1,0,true,true});M.Strike({150,1,200,true,true},false);
    Check(M.GetState().OpponentHealth==100,"vertical separation prevents hits");
    Advance(M,.6,{500,1,0,true,true});Check(!M.Strike(Close(),true),"guard must release to strike");
    M.Start();M.Tick(.05,Close(),false);Check(M.GetState().AI==Phase::Windup,"opponent telegraphs");
    Check(M.GetState().PlayerHealth==100,"windup not immediate hit");RoundTrip(M);
    Advance(M,.8,Close());Check(M.GetState().PlayerHealth==82,"single normal hit resolves");
    Check(M.GetState().AI==Phase::Recovery,"attack has recovery");
    M.Strike(Close(),false);Check(M.GetState().OpponentHealth==80,"recovery counter bonus");
    const int HP=M.GetState().PlayerHealth;Advance(M,.5,Close());
    Check(M.GetState().PlayerHealth==HP,"no repeated damage during recovery");
    M.Start();Advance(M,.85,Close(),true);Check(M.GetState().PlayerHealth==98,"guard reduces damage");
    Check(M.GetState().Stamina<100,"guard costs stamina");
    M.Start();Advance(M,.85,{150,-1,0,true,true},true);Check(M.GetState().PlayerHealth==82,"back-facing guard fails");
    M.Start();M.Tick(.05,Close(),false);Advance(M,.8,{400,1,0,true,true});
    Check(M.GetState().PlayerHealth==100,"retreat avoids telegraphed strike");
    M.Start();Advance(M,.85,{150,1,0,false,true});Check(M.GetState().PlayerHealth==100,"opponent cannot start attack through wall");
    // Prolonged turtling drains stamina and eventually breaks the guard.
    M.Start();bool Broken=false;
    for(int I=0;I<2400&&M.IsActive();++I){M.Tick(.05,Close(),true);Broken|=M.GetState().GuardBreak>0;RoundTrip(M);}
    Check(Broken,"sustained guard can break");Check(M.GetState().Result==Outcome::Lost,"passive guard is not invincible");
    Check(M.GetState().Losses==1,"loss counted once");
    M.Tick(1,Close(),false);M.Forfeit();Check(M.GetState().Losses==1,"completed loss cannot be awarded twice");
    M.Start();Advance(M,120.1,{700,1,0,true,true});Check(M.GetState().Result==Outcome::Draw,"timeout tie");
    Check(M.GetState().Draws==1,"draw recorded");RoundTrip(M);
    M.DismissResult();Check(!M.GetState().ResultPending,"dismiss result persists");RoundTrip(M);
    M.Start();M.Strike(Close(),false);Advance(M,120.1,{700,1,0,true,true});
    Check(M.GetState().Result==Outcome::Won,"timeout points decision");RoundTrip(M);
    M.Start();M.Tick(.1,{150,1,0,true,false},false);Check(M.GetState().Result==Outcome::Lost,"out of bounds forfeits");RoundTrip(M);
    // A countering player can finish a whole match; save/restore each simulation frame.
    M.Start();int Frames=0;
    while(M.IsActive()&&Frames++<2400)
    {
        const auto PhaseNow=M.GetState().AI;
        const bool Guard=PhaseNow==Phase::Windup;
        M.Tick(.05,Close(),Guard);
        if(M.GetState().AI==Phase::Recovery)M.Strike(Close(),false);
        Match Copy;Check(Copy.Restore(M.Serialize()),"in-progress checkpoint restores");
        Check(Copy.Serialize()==M.Serialize(),"checkpoint preserves attack cadence");
        M=Copy;
    }
    Check(M.GetState().Result==Outcome::Won,"counter strategy wins match");
    Check(M.GetState().Wins==2,"rematch retains record without duplicate reward");
    M.Start();const auto Before=M.Serialize();
    for(double Bad:{std::numeric_limits<double>::infinity(),std::numeric_limits<double>::quiet_NaN(),-1.0})
    {M.Tick(Bad,Close(),false);Check(M.Serialize()==Before,"invalid tick rejected");}
    auto F=Close();F.Distance=std::numeric_limits<double>::quiet_NaN();
    M.Tick(.1,F,false);Check(!M.Strike(F,false),"nonfinite distance rejected");Check(M.Serialize()==Before,"invalid input transaction");
    for(const std::string& Bad:{std::string(),std::string("ROW_SPARRING 100"),Before+" garbage",std::string(3000,'x'),std::string("ROW_SPARRING 1 1 0 100 100 nan 120 0 0 0 0 0 0 0 0"),std::string("ROW_SPARRING 1 1 0 100 100 100 0 0 0 0 0 0 0 0 0")})
    {Check(!M.Restore(Bad),"invalid checkpoint rejected");Check(M.Serialize()==Before,"failed restore leaves live match unchanged");}
    // Restoring a windup may not replay a hit or reset its warning time.
    M.Start();M.Tick(.3,Close(),false);Match Copy;Check(Copy.Restore(M.Serialize()),"restore windup");
    Advance(M,.6,Close());Advance(Copy,.6,Close());Check(M.Serialize()==Copy.Serialize(),"deterministic continuation after load");
    const auto SavedLocale=std::locale();
    std::locale::global(std::locale(std::locale::classic(),new CommaDecimal));
    const auto InternationalMatch=M.Serialize();
    ROWNarrative::Session Story;Story.SetName("Locale Test");Story.Choose(1);Story.Choose(1);
    Story.Visit(ROWNarrative::Place::Academy);Story.Choose(1);Story.Choose(1);
    Story.Visit(ROWNarrative::Place::Mentor);Story.Choose(1);Story.Visit(ROWNarrative::Place::Training,0);Story.Choose(1);Story.Tick(.35,{});
    const auto InternationalStory=Story.Serialize();
    std::locale::global(SavedLocale);
    Match LocalMatch;ROWNarrative::Session LocalStory;
    Check(LocalMatch.Restore(InternationalMatch),"combat checkpoint crosses decimal locale");
    Check(LocalMatch.Serialize()==M.Serialize(),"combat locale does not change numeric data");
    Check(LocalStory.Restore(InternationalStory),"narrative checkpoint crosses decimal locale");
    Check(LocalStory.GetAssessment().Elapsed==.35,"assessment time independent of decimal locale");
    std::cout<<"PASS: "<<Checks<<" combat assertions (telegraphs, counter wins, blocking, stamina, bounds, timers and checkpoint continuation).\n";
}
