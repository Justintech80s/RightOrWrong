#pragma once

// Tiny engine-independent rules core: this file is deliberately standard C++
// so training cues and branch outcome arithmetic can be tested without Unreal.
namespace ROWRules
{
    constexpr int SportCount = 6;
    constexpr int DrillLength = 3;
    constexpr int TrainingRewardXP = 30;

    inline const char* SportName(int Index)
    {
        constexpr const char* Names[SportCount] = {
            "Karate", "Judo", "Taekwondo", "Archery", "Fencing", "Sport Shooting"
        };
        return Index >= 0 && Index < SportCount ? Names[Index] : "Unselected";
    }

    // 1 = footwork/position, 2 = technique, 3 = control/precision.
    inline int ExpectedTrainingInput(int SportIndex, int Step)
    {
        constexpr int Cues[SportCount][DrillLength] = {
            {1, 2, 3}, // Karate: stance, strike form, control
            {2, 1, 3}, // Judo: grip, position, controlled finish
            {1, 3, 2}, // Taekwondo: range, balance, kicking form
            {3, 1, 2}, // Archery: breath, stance, release
            {1, 2, 3}, // Fencing: advance, parry, safe touch
            {3, 2, 1}  // Sport shooting: breath, trigger discipline, stance
        };
        return SportIndex >= 0 && SportIndex < SportCount && Step >= 0 && Step < DrillLength
            ? Cues[SportIndex][Step] : 0;
    }

    struct ReputationDelta
    {
        int Law = 0;
        int Crime = 0;
        int PublicTrust = 0;
        int InvestorFavor = 0;
        int Cash = 0;
        bool Valid = false;
    };

    // Follow-up branches are fictional moral choices, not procedural guidance.
    inline ReputationDelta FollowUpOutcome(int InitialChoice, int FollowUpChoice)
    {
        if (FollowUpChoice != 1 && FollowUpChoice != 2) return {};
        if (InitialChoice == 1)
        {
            return FollowUpChoice == 1 ? ReputationDelta{10, 0, -10, 15, 0, true}
                                       : ReputationDelta{5, 0, 15, -10, 0, true};
        }
        if (InitialChoice == 2)
        {
            return FollowUpChoice == 1 ? ReputationDelta{5, 0, 20, -5, 0, true}
                                       : ReputationDelta{10, 0, 5, 5, 0, true};
        }
        if (InitialChoice == 3)
        {
            return FollowUpChoice == 1 ? ReputationDelta{0, 20, -10, 0, 200, true}
                                       : ReputationDelta{10, -5, 10, -5, 0, true};
        }
        return {};
    }
}
