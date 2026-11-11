#include "../Source/RightOrWrong/ROWProgressionRules.h"
#include <cassert>
#include <iostream>
int main()
{
    for (int sport = 0; sport < ROWRules::SportCount; ++sport)
    {
        assert(ROWRules::SportName(sport));
        for (int round = 0; round < ROWRules::DrillLength; ++round)
        {
            const int input = ROWRules::ExpectedTrainingInput(sport, round);
            assert(input >= 1 && input <= 3);
        }
        assert(ROWRules::ExpectedTrainingInput(sport, 3) == 0);
    }
    assert(ROWRules::ExpectedTrainingInput(-1, 0) == 0);
    assert(ROWRules::ExpectedTrainingInput(7, 0) == 0);
    assert(ROWRules::FollowUpOutcome(1, 1).InvestorFavor == 15);
    assert(ROWRules::FollowUpOutcome(1, 2).PublicTrust == 15);
    assert(ROWRules::FollowUpOutcome(2, 1).PublicTrust == 20);
    assert(ROWRules::FollowUpOutcome(2, 2).Law == 10);
    assert(ROWRules::FollowUpOutcome(3, 1).Cash == 200);
    assert(ROWRules::FollowUpOutcome(3, 2).Crime == -5);
    assert(!ROWRules::FollowUpOutcome(0, 1).Valid);
    assert(!ROWRules::FollowUpOutcome(1, 3).Valid);
    std::cout << "PASS: six training cue sequences and six branching aftermath outcomes.\n";
}
