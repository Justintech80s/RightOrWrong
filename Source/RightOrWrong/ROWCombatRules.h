#pragma once
// The Unreal director and standalone tests use this same deterministic match model.
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>

namespace ROWCombat
{
enum class Outcome { Idle, Active, Won, Lost, Draw };
enum class Phase { Closing, Windup, Recovery, Guard };
struct Frame
{
    double Distance = 10000;
    double Facing = -1; // Camera's horizontal dot product toward the opponent.
    double HeightDifference = 0;
    bool LineOfSight = false;
    bool PlayerInsideRing = true;
};
struct State
{
    Outcome Result = Outcome::Idle;
    Phase AI = Phase::Closing;
    int PlayerHealth = 100, OpponentHealth = 100;
    double Stamina = 100, SecondsLeft = 120, PhaseLeft = 0;
    double AttackCooldown = 0, GuardBreak = 0;
    int Sequence = 0, Wins = 0, Losses = 0, Draws = 0;
    bool ResultPending = false;
};
class Match
{
public:
    const State& GetState() const { return Data; }
    bool IsActive() const { return Data.Result == Outcome::Active; }
    std::string Feedback;
    void Start()
    {
        const int W=Data.Wins,L=Data.Losses,D=Data.Draws;
        Data=State{};Data.Wins=W;Data.Losses=L;Data.Draws=D;
        Data.Result=Outcome::Active;
        Feedback="Exhibition started. Face your opponent, manage distance, and watch the attack warning.";
    }
    void DismissResult() { if(!IsActive())Data.ResultPending=false; }
    void Forfeit() { if(IsActive())Finish(Outcome::Lost,"Exhibition ended: you left the marked ring. You may retry."); }
    bool Strike(const Frame& F,bool GuardHeld)
    {
        if(!IsActive()||!ValidFrame(F))return false;
        if(!F.PlayerInsideRing){Forfeit();return false;}
        if(GuardHeld){Feedback="Release your guard to strike.";return false;}
        if(Data.AttackCooldown>0||Data.GuardBreak>0)return false;
        if(Data.Stamina<18){Feedback="Low stamina. Step back and recover.";return false;}
        Data.Stamina-=18;Data.AttackCooldown=.5;
        if(!InReach(F,220)||F.Facing<.55)
        { Feedback="Strike missed. Move within 2.2 m and face the opponent.";return true; }
        const int Damage=Data.AI==Phase::Guard?3:Data.AI==Phase::Recovery?20:14;
        Data.OpponentHealth=std::max(0,Data.OpponentHealth-Damage);
        Feedback=Data.AI==Phase::Guard?"Opponent blocked. Wait for an opening.":Data.AI==Phase::Recovery?"Counter landed during recovery.":"Clean strike landed.";
        if(Data.OpponentHealth==0)Finish(Outcome::Won,"Exhibition won. Coach Brooks records your result.");
        return true;
    }
    void Tick(double Delta,const Frame& F,bool GuardHeld)
    {
        if(!IsActive()||!std::isfinite(Delta)||Delta<=0||!ValidFrame(F))return;
        if(!F.PlayerInsideRing){Forfeit();return;}
        // Limit catch-up after a stall. Never resolve a long unseen barrage in one frame.
        double Remaining=std::min(Delta,1.0);
        while(Remaining>1e-9&&IsActive())
        {
            const double Dt=std::min(Remaining,.05);Remaining-=Dt;
            Data.SecondsLeft=std::max(0.0,Data.SecondsLeft-Dt);
            Data.AttackCooldown=std::max(0.0,Data.AttackCooldown-Dt);
            Data.GuardBreak=std::max(0.0,Data.GuardBreak-Dt);
            Data.Stamina=std::min(100.0,Data.Stamina+Dt*(GuardHeld?4.0:22.0));
            if(Data.SecondsLeft<1e-8)
            {
                Data.SecondsLeft=0;
                Finish(Data.PlayerHealth>Data.OpponentHealth?Outcome::Won:Data.PlayerHealth<Data.OpponentHealth?Outcome::Lost:Outcome::Draw,"Time. The higher remaining health wins; equal health is a draw.");
                break;
            }
            if(Data.AI==Phase::Closing)
            {
                if(InReach(F,195)){Data.AI=Phase::Windup;Data.PhaseLeft=Heavy()?1.05:.7;}
                continue;
            }
            Data.PhaseLeft=std::max(0.0,Data.PhaseLeft-Dt);
            if(Data.PhaseLeft>1e-8)continue;
            if(Data.AI==Phase::Windup)
            {
                if(InReach(F,225))
                {
                    const int Cost=Heavy()?30:20;
                    if(GuardHeld&&F.Facing>=.2&&Data.Stamina>=Cost&&Data.GuardBreak==0)
                    {Data.Stamina-=Cost;Data.PlayerHealth=std::max(0,Data.PlayerHealth-2);Feedback="Blocked. Release guard between attacks to recover stamina.";}
                    else
                    {
                        Data.PlayerHealth=std::max(0,Data.PlayerHealth-(Heavy()?26:18));
                        if(GuardHeld&&F.Facing>=.2){Data.Stamina=0;Data.GuardBreak=1;Feedback="Guard broken. Create distance and recover.";}
                        else Feedback="Opponent connected. Block while facing them, or move out of range.";
                    }
                    if(Data.PlayerHealth==0){Finish(Outcome::Lost,"Coach stops the exhibition. Recover and try again.");break;}
                }
                else Feedback="Opponent missed. Counter during recovery.";
                Data.AI=Phase::Recovery;Data.PhaseLeft=1.0;
            }
            else if(Data.AI==Phase::Recovery)
            {
                Data.Sequence=(Data.Sequence+1)%6;
                Data.AI=Data.Sequence%2==0?Phase::Guard:Phase::Closing;
                Data.PhaseLeft=Data.AI==Phase::Guard?.85:0;
            }
            else {Data.AI=Phase::Closing;Data.PhaseLeft=0;}
        }
    }
    bool Heavy() const {return Data.Sequence%3==2;}
    const char* Cue() const
    {
        if(!IsActive())return Data.Result==Outcome::Won?"VICTORY":Data.Result==Outcome::Lost?"DEFEAT":Data.Result==Outcome::Draw?"DRAW":"READY";
        if(Data.GuardBreak>0)return "GUARD BROKEN — create distance";
        switch(Data.AI)
        {
            case Phase::Closing:return "Opponent approaching";
            case Phase::Windup:return Heavy()?"HEAVY STRIKE INCOMING — block or evade":"STRIKE INCOMING — block or evade";
            case Phase::Recovery:return "COUNTER WINDOW — opponent recovering";
            case Phase::Guard:return "OPPONENT GUARDING — reposition";
        }
        return "";
    }
    std::string Serialize() const
    {
        std::ostringstream Out;Out.imbue(std::locale::classic());Out<<std::setprecision(17)<<"ROW_SPARRING 1 "
            <<static_cast<int>(Data.Result)<<' '<<static_cast<int>(Data.AI)<<' '
            <<Data.PlayerHealth<<' '<<Data.OpponentHealth<<' '<<Data.Stamina<<' '
            <<Data.SecondsLeft<<' '<<Data.PhaseLeft<<' '<<Data.AttackCooldown<<' '
            <<Data.GuardBreak<<' '<<Data.Sequence<<' '<<Data.Wins<<' '<<Data.Losses<<' '
            <<Data.Draws<<' '<<Data.ResultPending;
        return Out.str();
    }
    bool Restore(const std::string& Snapshot)
    {
        if(Snapshot.size()>2048)return false;
        std::istringstream In(Snapshot);In.imbue(std::locale::classic());std::string Magic;int Version=0,R=-1,P=-1,Pending=-1;State S;
        if(!(In>>Magic>>Version>>R>>P>>S.PlayerHealth>>S.OpponentHealth>>S.Stamina>>S.SecondsLeft>>S.PhaseLeft>>S.AttackCooldown>>S.GuardBreak>>S.Sequence>>S.Wins>>S.Losses>>S.Draws>>Pending))return false;
        In>>std::ws;if(!In.eof()||Magic!="ROW_SPARRING"||Version!=1||R<0||R>4||P<0||P>3||Pending<0||Pending>1)return false;
        S.Result=static_cast<Outcome>(R);S.AI=static_cast<Phase>(P);S.ResultPending=Pending!=0;
        if(!Bound(S.Stamina,100)||!Bound(S.SecondsLeft,120)||!Bound(S.PhaseLeft,1.05)||!Bound(S.AttackCooldown,.5)||!Bound(S.GuardBreak,1))return false;
        if(S.PlayerHealth<0||S.PlayerHealth>100||S.OpponentHealth<0||S.OpponentHealth>100||S.Sequence<0||S.Sequence>=6)return false;
        if(S.Wins<0||S.Wins>100000||S.Losses<0||S.Losses>100000||S.Draws<0||S.Draws>100000)return false;
        if((S.Result==Outcome::Idle||S.Result==Outcome::Active)&&S.ResultPending)return false;
        if(S.Result==Outcome::Active&&(S.SecondsLeft<=0||S.PlayerHealth<=0||S.OpponentHealth<=0))return false;
        if(S.Result==Outcome::Idle&&(S.PlayerHealth!=100||S.OpponentHealth!=100||S.SecondsLeft!=120||S.Wins||S.Losses||S.Draws))return false;
        if(S.Result==Outcome::Won&&(S.Wins==0||S.PlayerHealth<=S.OpponentHealth))return false;
        if(S.Result==Outcome::Lost&&S.Losses==0)return false; // A forfeit may retain full health.
        if(S.Result==Outcome::Draw&&(S.Draws==0||S.SecondsLeft!=0||S.PlayerHealth!=S.OpponentHealth))return false;
        if(S.AI==Phase::Closing&&S.PhaseLeft!=0)return false;
        Data=S;Feedback="Match restored. Re-press guard to resume defending.";return true;
    }
private:
    State Data;
    static bool Bound(double V,double Maximum){return std::isfinite(V)&&V>=0&&V<=Maximum;}
    static bool ValidFrame(const Frame& F){return std::isfinite(F.Distance)&&F.Distance>=0&&std::isfinite(F.Facing)&&F.Facing>=-1&&F.Facing<=1&&std::isfinite(F.HeightDifference);}
    static bool InReach(const Frame& F,double Range){return F.Distance<=Range&&std::abs(F.HeightDifference)<100&&F.LineOfSight;}
    void Finish(Outcome Result,const char* Message)
    {
        if(!IsActive())return;
        Data.Result=Result;Data.ResultPending=true;Feedback=Message;
        int* Count=Result==Outcome::Won?&Data.Wins:Result==Outcome::Lost?&Data.Losses:&Data.Draws;
        *Count=std::min(100000,*Count+1);
    }
};
}
