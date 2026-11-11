#pragma once
#include <cmath>
#include <vector>

// Centimetres, Z up. Shared geometry makes reachability checks test the actual layout.
namespace ROWBlockout
{
struct Box {double X,Y,Z,W,D,H;int Palette=0;};
struct Point {const char* Name;double X,Y;};
inline std::vector<Box> Geometry()
{
    std::vector<Box> B;
    auto Add=[&](double X,double Y,double Z,double W,double D,double H,int Color=0){B.push_back({X,Y,Z,W,D,H,Color});};
    Add(0,0,-80,9000,9000,60,0);
    Add(-4450,0,400,100,9000,900,2);Add(4450,0,400,100,9000,900,2);
    Add(0,-4450,400,9000,100,900,2);Add(0,4450,400,9000,100,900,2);
    // Academy paving and crossing into the licensed field-exercise district.
    Add(0,1400,-48,6500,2100,4,1);
    Add(0,0,-45,8750,400,10,3);Add(0,-2200,-45,450,4300,10,3);
    Add(0,-2500,-44,8700,400,12,3);
    for(int I=-4;I<=4;++I)Add(I*90,0,-38,40,380,2,4);
    for(int I=-3;I<=3;++I)Add(0,I*750-1700,-38,20,180,2,4);
    // Open classrooms/administration, not impassable solid buildings.
    auto Room=[&](double X,double Y,double W,double D,int Color)
    {
        Add(X,Y,-44,W,D,12,1);
        Add(X-W/2,Y,150,35,D,400,Color);Add(X+W/2,Y,150,35,D,400,Color);
        Add(X,Y+D/2,150,W,35,400,Color);
        const double Wing=(W-300)/2;
        Add(X-(300+Wing)/2,Y-D/2,150,Wing,35,400,Color);
        Add(X+(300+Wing)/2,Y-D/2,150,Wing,35,400,Color);
        Add(X,Y-D/2,310,300,35,80,Color); // 300 cm clear door, 310 cm headroom.
        Add(X,Y,365,W+70,D+70,30,Color);
    };
    Room(0,3400,1200,800,2); // Evaluation room; front door Y=3000.
    Room(1300,3400,900,700,2); // Procurement, door Y=3050.
    // South Ward: market, cafe, oversight and records office with accessible interiors.
    Room(-2900,-1550,1400,900,5);
    Room(2800,-1400,1300,700,5);
    Room(-2400,-3350,1400,1200,2);
    Room(2400,-3450,1400,1200,2);
    // Oversight/records entrances face the south perimeter lane. Side passages remain open.
    Add(-2900,-1400,0,550,100,100,6);Add(2800,-1300,0,500,100,100,6);
    Add(-2400,-3200,0,400,100,100,6);Add(2400,-3300,0,400,100,100,6);
    // Courtyard seating, planters, archway pillars. Keep approach paths unobstructed.
    for(int Side:{-1,1})
    {
        Add(Side*1400,1050,-2,320,80,90,6);
        Add(Side*1400,1300,0,340,180,100,7);
        Add(Side*650,150,150,100,100,400,2);
    }
    // Optional sparring ring; painted border and corner pads, no invisible enclosure.
    Add(-2700,3300,-42,1400,1300,16,8);
    for(int Side:{-1,1})
    {
        Add(-2700+Side*680,3300,-32,30,1300,4,4);
        Add(-2700,3300+Side*630,-32,1400,30,4,4);
        for(int Other:{-1,1})Add(-2700+Side*680,3300+Other*630,30,65,65,150,8);
    }
    return B;
}
inline std::vector<Point> Destinations()
{
    return {{"Spawn",0,0},{"General Ward",0,900},{"Coach Brooks",-600,600},
        {"Jordan Cross",600,600},{"Visiting contact",-900,-100},{"Evaluation",0,3300},
        {"Assignment",1600,600},{"Records",2400,-3500},{"Procurement",1300,3300},
        {"Oversight",-2400,-3500},{"Fixer",3000,-2000},{"Market",-2900,-1700},
        {"Cafe",2800,-1500},{"Sparring",-2700,2475},
        {"Karate",-2400,1800},{"Judo",0,1800},{"Taekwondo",2400,1800},
        {"Archery",-2400,-800},{"Fencing",0,-800},{"Sport shooting",2400,-800}};
}
inline bool Walkable(double X,double Y)
{
    // Conservative upright capsule footprint: 47 cm radius and 180 cm height.
    // Ignore floors/steps <= 25 cm and overhead ceilings; UE collision is still a separate gate.
    for(const auto& B:Geometry())
        if(B.Z+B.H/2>-25&&B.Z-B.H/2<140&&std::abs(X-B.X)<B.W/2+47&&std::abs(Y-B.Y)<B.D/2+47)return false;
    return std::abs(X)<4350&&std::abs(Y)<4350;
}
}
