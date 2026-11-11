#pragma once

// Engine-independent foundation for the Academy's teaching and graduation rules.
// These are abstract narrative/stat decisions, never tactical instruction.
namespace ROWPedagogy
{
    constexpr int LessonXP = 20;
    constexpr int MentorXP = 10;
    constexpr int GraduationPrimarySportXP = 90;
    constexpr int GraduationKnowledgeXP = 30;

    struct LearningOutcome
    {
        int Knowledge = 0;
        int PublicTrust = 0;
        int InvestorFavor = 0;
        bool Valid = false;
    };

    // A classroom dilemma: the instructor presents two defensible ways to
    // handle a fictional public accountability case, without a 'good/evil' tag.
    inline LearningOutcome SeminarChoice(int Choice)
    {
        if (Choice == 1) return {LessonXP, 5, -5, true}; // evidence-led discussion
        if (Choice == 2) return {LessonXP, -2, 5, true}; // institutional discretion
        return {};
    }

    inline LearningOutcome MentorChoice(int Choice)
    {
        if (Choice == 1) return {MentorXP, 3, 0, true}; // listen and ask for feedback
        if (Choice == 2) return {MentorXP, 0, 0, true}; // challenge a claim respectfully
        return {};
    }

    inline bool EligibleForGraduation(int PrimarySport, int SportXP, int KnowledgeXP,
                                       bool HasMentorConversation, bool CompletedOpeningCase)
    {
        return PrimarySport >= 0 && PrimarySport < 6 &&
            SportXP >= GraduationPrimarySportXP &&
            KnowledgeXP >= GraduationKnowledgeXP &&
            HasMentorConversation && CompletedOpeningCase;
    }
}
