#include "../Source/RightOrWrong/ROWPedagogyRules.h"
#include <cassert>
#include <iostream>
int main()
{
    using namespace ROWPedagogy;
    assert(!SeminarChoice(0).Valid && !MentorChoice(4).Valid);
    assert(SeminarChoice(1).Knowledge == 20);
    assert(SeminarChoice(2).InvestorFavor == 5);
    assert(MentorChoice(1).Knowledge == 10);
    assert(MentorChoice(2).Knowledge == 10);
    assert(!EligibleForGraduation(0, 60, 30, true, true));
    assert(!EligibleForGraduation(0, 90, 20, true, true));
    assert(!EligibleForGraduation(0, 90, 30, false, true));
    assert(!EligibleForGraduation(0, 90, 30, true, false));
    assert(EligibleForGraduation(0, 90, 30, true, true));
    assert(EligibleForGraduation(5, 300, 500, true, true));
    assert(!EligibleForGraduation(6, 300, 500, true, true));
    std::cout << "PASS: 13 pedagogy/graduation assertions.\n";
}
