#include "../Source/RightOrWrong/ROWBlockout.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <queue>
#include <utility>
static void Check(bool V,const char* S){if(!V){std::cerr<<"FAIL: "<<S<<'\n';std::exit(1);}}
int main()
{
    constexpr int N=175;constexpr double Step=50,Min=-4350;
    std::array<std::array<bool,N>,N> Allowed{},Seen{};
    for(int X=0;X<N;++X)for(int Y=0;Y<N;++Y)Allowed[X][Y]=ROWBlockout::Walkable(Min+X*Step,Min+Y*Step);
    std::queue<std::pair<int,int>> Q;Q.push({87,87});Seen[87][87]=true;
    const int DX[]={1,-1,0,0},DY[]={0,0,1,-1};int Reachable=0;
    while(!Q.empty())
    {
        const auto P=Q.front();Q.pop();++Reachable;
        for(int I=0;I<4;++I)
        {const int X=P.first+DX[I],Y=P.second+DY[I];if(X>=0&&X<N&&Y>=0&&Y<N&&Allowed[X][Y]&&!Seen[X][Y]){Seen[X][Y]=true;Q.push({X,Y});}}
    }
    for(const auto& P:ROWBlockout::Destinations())
    {
        Check(ROWBlockout::Walkable(P.X,P.Y),P.Name);
        const int X=static_cast<int>(std::round((P.X-Min)/Step)),Y=static_cast<int>(std::round((P.Y-Min)/Step));
        Check(Seen[X][Y],P.Name);std::cout<<"Reachable: "<<P.Name<<'\n';
    }
    Check(!ROWBlockout::Walkable(4450,0),"perimeter blocks exit");
    Check(!ROWBlockout::Walkable(600,3400),"classroom wall blocks capsule");
    Check(ROWBlockout::Walkable(0,3000),"classroom doorway fits capsule");
    Check(ROWBlockout::Walkable(-2700,3000)&&ROWBlockout::Walkable(-2700,3600),"ring spawns clear");
    std::cout<<"PASS: all 20 destinations reachable across "<<Reachable<<" sampled capsule locations; geometry only, not Unreal physics.\n";
}
