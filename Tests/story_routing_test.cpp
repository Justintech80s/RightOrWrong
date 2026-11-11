#include "../Source/RightOrWrong/ROWStoryRoutingRules.h"
#include <cassert>
#include <string>
int main() {
 using namespace ROWStoryRouting;
 assert(NetworkForSport(-1)==ContactNetwork::Unselected);
 assert(NetworkForSport(6)==ContactNetwork::Unselected);
 for (int i=0;i<6;++i) assert(NetworkForSport(i)==((i%2==1)?ContactNetwork::LawEnforcement:ContactNetwork::CriminalUnderworld));
 assert(TierForPerformance(0,0)==ContactTier::Freelance);
 assert(TierForPerformance(90,0)==ContactTier::Neighborhood);
 assert(TierForPerformance(180,0)==ContactTier::Established);
 assert(TierForPerformance(270,0)==ContactTier::Elite);
 assert(TierForPerformance(180,3)==ContactTier::Neighborhood);
 assert(std::string(ContactDescription(ContactNetwork::CriminalUnderworld,ContactTier::Freelance)).find("freelance")!=std::string::npos);
}
