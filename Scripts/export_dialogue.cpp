#include "../Source/RightOrWrong/ROWNarrative.h"
#include <iostream>
using namespace ROWNarrative;
void Print(const Session& s)
{
    const auto v=s.GetView();
    std::cout<<"## "<<v.Title<<"\n\n**"<<v.Speaker<<"**\n\n"<<v.Text<<"\n\n";
    for(std::size_t i=0;i<v.Options.size();++i)std::cout<<(i+1)<<". "<<v.Options[i]<<"\n";
    std::cout<<"\n";
}
Session Prepare(int sport,int score,bool print)
{
    Session s;s.SetName("Your recruit");if(print)Print(s);s.Choose(1);if(print)Print(s);s.Choose(1);
    s.Visit(Place::Academy);if(print)Print(s);s.Choose(1);if(print)Print(s);s.Choose(sport+1);
    s.Visit(Place::Mentor);if(print)Print(s);s.Choose(1);s.Visit(Place::Training,sport);Print(s);s.Choose(1);
    while(s.GetAssessment().Active)
    {
        Frame f;f.Distance=sport==2?300:(sport==3||sport==5)?1000:180;f.Aim=sport==1||sport==2?(2.0*score-100)/100.0:score/100.0;f.HoldSeconds=1;f.LateralTravel=110;
        if(sport==1)f.Aim=(3.0*score-100)/200.0;
        s.Act(s.ExpectedAction(),f);
    }
    if(print) { Print(s); }
    s.Choose(1); s.Visit(Place::Recruit);
    if(print) { Print(s); }
    s.Choose(1); s.Visit(Place::Contact); Print(s); return s;
}
int main()
{
    std::cout<<"# RIGHT OR WRONG — Written Narrative Demo Script\n\nGenerated from the same C++ dialogue used by the Unreal interface. All lines are read on screen; no voice files or automatic reading deadlines are required. The recruit's name and sport are substituted at runtime.\n\n";
    auto base=Prepare(0,100,true);base.Choose(1);base.Visit(Place::Contact);Print(base);base.Choose(1);base.Visit(Place::Evidence);Print(base);
    for(int branch=1;branch<=3;++branch)
    {
        auto s=base;s.Choose(branch);s.Visit(branch==1?Place::Command:branch==2?Place::Oversight:Place::Fixer);Print(s);s.Choose(1);
        if(branch==1){s.Visit(Place::Graduation);Print(s);}
    }
    std::cout<<"# Other starting programs and contact examples\n\n";
    for(int sport=1;sport<6;++sport)Prepare(sport,100,false);
    std::cout<<"# Lower-performance criminal introduction\n\n";
    Prepare(0,60,false);
}
