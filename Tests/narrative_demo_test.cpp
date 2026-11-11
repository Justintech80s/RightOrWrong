#include "../Source/RightOrWrong/ROWNarrative.h"
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
using namespace ROWNarrative;
static int checks = 0;
#define REQUIRE(x) do { ++checks; if (!(x)) throw std::runtime_error(std::string(#x) + " at line " + std::to_string(__LINE__)); } while(false)
static void RoundTrip(Session& session)
{
    const auto bytes = session.Serialize(); Session loaded;
    REQUIRE(loaded.Restore(bytes)); REQUIRE(loaded.Serialize() == bytes);
    session = loaded;
}
static Frame GoodFrame(int sport, double aim = 1.0)
{
    Frame f; f.Distance = sport == 2 ? 300.0 : (sport == 3 || sport == 5) ? 1000.0 : 180.0;
    f.Aim = aim; f.HoldSeconds = 1.0; f.LateralTravel = 110.0; return f;
}
static Session AtTraining(int sport)
{
    Session s; REQUIRE(s.SetName("Justin")); RoundTrip(s);
    REQUIRE(s.Choose(1)); RoundTrip(s); REQUIRE(s.Choose(2)); RoundTrip(s);
    REQUIRE(!s.Visit(Place::Training, sport));
    REQUIRE(s.Visit(Place::Academy)); RoundTrip(s); REQUIRE(s.Choose(1));
    REQUIRE(s.GetView().Options.size() == 6); REQUIRE(s.Choose(sport + 1));
    REQUIRE(s.GetState().Sport == sport); REQUIRE(!s.Visit(Place::Training, sport));
    REQUIRE(s.Visit(Place::Mentor)); RoundTrip(s); REQUIRE(s.Choose(1));
    REQUIRE(s.Visit(Place::Training, sport)); RoundTrip(s); REQUIRE(s.Choose(1));
    return s;
}
static void FinishAssessment(Session& s, double aim = 1.0)
{
    const int sport = s.GetAssessment().Sport;
    while (s.GetAssessment().Active)
    {
        RoundTrip(s); REQUIRE(!s.Visit(Place::Recruit));
        const auto action = s.ExpectedAction();
        REQUIRE(s.Act(action, GoodFrame(sport, action == Action::Footwork ? 1.0 : aim)));
    }
    REQUIRE(s.GetDialogue() == Dialogue::Result); RoundTrip(s); REQUIRE(s.Choose(1));
}
static Session Complete(int sport, int recruit, int contact, int evidence, int follow)
{
    auto s = AtTraining(sport); FinishAssessment(s);
    REQUIRE(s.GetState().PlacementScore == 100);
    REQUIRE(s.Visit(Place::Recruit)); RoundTrip(s); REQUIRE(s.Choose(recruit));
    REQUIRE(s.Visit(Place::Contact)); RoundTrip(s);
    REQUIRE(s.GetView().Speaker == ((sport % 2) ? "Captain Mara Keene" : "Nico Rellan"));
    REQUIRE(s.Choose(contact)); REQUIRE(!s.Visit(Place::Evidence));
    REQUIRE(s.Visit(contact == 2 ? Place::Academy : Place::Contact)); RoundTrip(s);
    REQUIRE(s.Choose(1)); REQUIRE(s.GetState().Assigned); RoundTrip(s);
    REQUIRE(!s.Visit(Place::Training, sport));
    REQUIRE(s.Visit(Place::Evidence)); RoundTrip(s); REQUIRE(s.Choose(evidence));
    const auto target = evidence == 1 ? Place::Command : evidence == 2 ? Place::Oversight : Place::Fixer;
    REQUIRE(!s.Visit(evidence == 1 ? Place::Fixer : Place::Command));
    REQUIRE(s.Visit(target)); RoundTrip(s); REQUIRE(s.Choose(follow));
    const int cash = s.GetState().Money; const int law = s.GetState().Law;
    REQUIRE(!s.Choose(follow)); REQUIRE(!s.Visit(target)); REQUIRE(s.GetState().Money == cash); REQUIRE(s.GetState().Law == law);
    REQUIRE(s.Visit(Place::Graduation)); RoundTrip(s); REQUIRE(s.Choose(1));
    REQUIRE(s.GetState().Graduated); RoundTrip(s); return s;
}
int main()
{
    try
    {
        int routes = 0;
        for (int sport = 0; sport < 6; ++sport)
            for (int recruit = 1; recruit <= 2; ++recruit)
                for (int contact = 1; contact <= 2; ++contact)
                    for (int evidence = 1; evidence <= 3; ++evidence)
                        for (int follow = 1; follow <= 2; ++follow)
                        { auto s = Complete(sport,recruit,contact,evidence,follow); REQUIRE(!s.GetJournal().empty()); ++routes; }
        for (int sport = 0; sport < 6; ++sport)
        {
            auto s = AtTraining(sport); const auto before = s.Serialize();
            REQUIRE(!s.Choose(1)); REQUIRE(before == s.Serialize());
            Frame invalid = GoodFrame(sport); invalid.Aim = std::numeric_limits<double>::quiet_NaN();
            REQUIRE(!s.Act(s.ExpectedAction(), invalid));
            s.Tick(12.0, Frame{}); REQUIRE(s.GetAssessment().Step == 1);
            s.Tick(60.0, Frame{}); REQUIRE(!s.GetAssessment().Active); REQUIRE(s.GetState().LastScore == 0);
            REQUIRE(s.GetState().PlacementScore == -1); REQUIRE(s.Choose(1));
            REQUIRE(!s.Visit(Place::Recruit)); REQUIRE(s.Visit(Place::Training, sport)); REQUIRE(s.Choose(1));
            FinishAssessment(s); REQUIRE(s.GetState().PlacementScore == 100);
        }
        // Expiring a cue must not reuse old movement to complete the newly opened cue.
        auto expiredJudo = AtTraining(1); expiredJudo.Tick(12.0, GoodFrame(1));
        REQUIRE(expiredJudo.GetAssessment().Step == 1);
        // Each discipline enforces its relevant interaction constraints.
        auto bow = AtTraining(3); Frame shot = GoodFrame(3); shot.HoldSeconds = 0.1;
        REQUIRE(bow.Act(Action::Release,shot)); REQUIRE(bow.GetAssessment().Total == 0);
        shot.HoldSeconds = 1.0; REQUIRE(bow.Act(Action::Release,shot)); REQUIRE(bow.GetAssessment().Total == 100);
        auto range = AtTraining(5); shot = GoodFrame(5); shot.HoldSeconds = 0.1;
        REQUIRE(range.Act(Action::Shot,shot)); REQUIRE(range.GetAssessment().Total == 0);
        shot.HoldSeconds = 1.0; shot.Distance = 100.0;
        REQUIRE(range.Act(Action::Shot,shot)); REQUIRE(range.GetAssessment().Total == 0);
        auto judo = AtTraining(1); REQUIRE(judo.Act(Action::Guard,GoodFrame(1)));
        Frame footwork = GoodFrame(1); footwork.LateralTravel = 10.0;
        REQUIRE(!judo.Act(Action::Footwork,footwork)); REQUIRE(judo.GetAssessment().Step == 1);
        auto karate = AtTraining(0); Frame outOfRange = GoodFrame(0); outOfRange.Distance = 450;
        REQUIRE(karate.Act(Action::Strike,outOfRange)); REQUIRE(karate.GetAssessment().Total == 0);
        // Failing aim and correct action types must not masquerade as a passing grade.
        auto poor = AtTraining(3); FinishAssessment(poor,0.4); REQUIRE(poor.GetState().LastScore == 40); REQUIRE(poor.GetState().PlacementScore == -1);
        // Written dialogue is persistent and does not expire as time passes.
        Session read; const auto first = read.Serialize(); read.Tick(100000.0,Frame{}); REQUIRE(read.Serialize() == first);
        REQUIRE(!read.SetName("\n")); REQUIRE(!read.SetName("   ")); REQUIRE(!read.SetName(std::string(97,'a'))); REQUIRE(read.SetName("Zoë")); RoundTrip(read);
        // Restore is transactional: malformed saves never replace the active session.
        auto finished = Complete(0,1,2,3,2); const auto saved = finished.Serialize();
        for (std::size_t length = 0; length < saved.size(); length += 79) { REQUIRE(!finished.Restore(saved.substr(0,length))); REQUIRE(finished.Serialize() == saved); }
        REQUIRE(!finished.Restore(saved + "unexpected")); REQUIRE(!finished.Restore("ROW_NARRATIVE 999\n")); REQUIRE(!finished.Restore(std::string(2*1024*1024+1,'x')));
        REQUIRE(finished.Serialize() == saved);
        // Later sport practice never changes the first program or its placement score.
        REQUIRE(finished.Visit(Place::Training,5)); REQUIRE(finished.Choose(1)); FinishAssessment(finished);
        REQUIRE(finished.GetState().Sport == 0); REQUIRE(finished.GetState().PlacementScore == 100); REQUIRE(finished.GetState().XP[5] == 30);
        auto modest = AtTraining(0); FinishAssessment(modest,0.60);
        REQUIRE(modest.GetState().PlacementScore == 60);
        REQUIRE(modest.Visit(Place::Recruit)); REQUIRE(modest.Choose(2)); REQUIRE(modest.Visit(Place::Contact));
        REQUIRE(modest.GetView().Text.find("freelancers") != std::string::npos);
        REQUIRE(modest.Choose(1)); REQUIRE(modest.Visit(Place::Contact)); REQUIRE(modest.Choose(1));
        REQUIRE(modest.Visit(Place::Evidence)); REQUIRE(modest.Choose(2)); REQUIRE(modest.Visit(Place::Oversight)); REQUIRE(modest.Choose(2));
        REQUIRE(modest.Visit(Place::Graduation)); REQUIRE(modest.Choose(1));
        REQUIRE(modest.Visit(Place::Training,0)); REQUIRE(modest.Choose(1)); FinishAssessment(modest);
        REQUIRE(modest.GetState().LastScore == 100); REQUIRE(modest.GetState().PlacementScore == 60);
        REQUIRE(modest.ContactDescription().find("freelance") != std::string::npos);
        REQUIRE(std::string(Grade(59)) == "Retry"); REQUIRE(std::string(Grade(60)) == "C"); REQUIRE(std::string(Grade(75)) == "B"); REQUIRE(std::string(Grade(90)) == "A");
        REQUIRE(Tier(60) == ROWStoryRouting::ContactTier::Freelance); REQUIRE(Tier(65) == ROWStoryRouting::ContactTier::Neighborhood); REQUIRE(Tier(80) == ROWStoryRouting::ContactTier::Established); REQUIRE(Tier(95) == ROWStoryRouting::ContactTier::Elite);
        std::cout << "PASS: " << routes << " complete narrative paths and " << checks << " assertions, including persistence, expiry, retries, choices and free first-sport selection.\n";
    }
    catch (const std::exception& e) { std::cerr << "FAIL: " << e.what() << '\n'; return EXIT_FAILURE; }
}
