#pragma once
// The actual demo state machine. Standard C++ allows testing the same rules used by Unreal.
#include "ROWProgressionRules.h"
#include "ROWStoryRoutingRules.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

namespace ROWNarrative
{
constexpr int SaveSchema = 1;
constexpr int Rounds = 6;
constexpr double CueSeconds = 12.0;
enum class Dialogue { None, Profile, Outfit, Welcome, Sport, Mentor, Drill, Result, Recruit, Contact, Assignment, Evidence, Aftermath, Graduation, Info };
enum class Place { Academy, Mentor, Training, Recruit, Contact, Assignment, Evidence, Command, Oversight, Fixer, Graduation, District };
enum class Action { Strike, Guard, Footwork, Release, Shot };
struct Frame
{
    double Distance = 0.0;
    double Aim = 0.0; // 0 = off target; 1 = exact target center.
    double HoldSeconds = 0.0;
    double LateralTravel = 0.0;
};
struct Assessment
{
    bool Active = false;
    int Sport = -1;
    int Step = 0;
    int Total = 0;
    int Misses = 0;
    double Elapsed = 0.0;
};
struct State
{
    std::string Name = "Recruit";
    int Presentation = 0;
    int Outfit = -1;
    bool Briefed = false;
    int Sport = -1;
    bool Mentored = false;
    std::array<int, 6> XP{};
    std::array<int, 6> Best{};
    int LastScore = -1;
    int PlacementScore = -1; // First successful primary assessment, never XP grinding.
    int RecruitChoice = 0;
    int ContactChoice = 0;
    bool Assigned = false;
    bool EvidenceFound = false;
    int CaseChoice = 0;
    int FollowUpChoice = 0;
    bool Graduated = false;
    int Law = 0, Crime = 0, Trust = 0, Sports = 0, Money = 0;
};
struct View
{
    std::string Speaker;
    std::string Title;
    std::string Text;
    std::vector<std::string> Options;
};
inline const char* Grade(int score)
{
    return score < 0 ? "Not assessed" : score >= 90 ? "A" : score >= 75 ? "B" : score >= 60 ? "C" : "Retry";
}
inline ROWStoryRouting::ContactTier Tier(int score)
{
    using T = ROWStoryRouting::ContactTier;
    return score < 0 ? T::Unavailable : score >= 95 ? T::Elite : score >= 80 ? T::Established : score >= 65 ? T::Neighborhood : T::Freelance;
}
inline bool ValidName(const std::string& name)
{
    if (name.empty() || name.size() > 96) return false;
    for (unsigned char c : name) if (c < 32 || c == 127) return false;
    return name.find_first_not_of(' ') != std::string::npos;
}
class Session
{
public:
    const State& GetState() const { return Data; }
    const Assessment& GetAssessment() const { return Drill; }
    Dialogue GetDialogue() const { return Screen; }
    bool IsModal() const { return Screen != Dialogue::None; }
    bool IsBusy() const { return IsModal() || Drill.Active; }
    const std::vector<std::string>& GetJournal() const { return Journal; }
    std::string Feedback;

    Session() { Record("VANGUARD ACADEMY", "An invitation arrived before your eighteenth birthday. Today you discover what it asks of you."); }
    bool SetName(const std::string& name)
    {
        if (Screen != Dialogue::Profile || !ValidName(name)) return false;
        Data.Name = name;
        return true;
    }
    std::string SportName() const { return ROWRules::SportName(Data.Sport); }
    std::string ContactDescription() const
    {
        return ROWStoryRouting::ContactDescription(ROWStoryRouting::NetworkForSport(Data.Sport), Tier(Data.PlacementScore));
    }
    std::string ContactName() const
    {
        return ROWStoryRouting::NetworkForSport(Data.Sport) == ROWStoryRouting::ContactNetwork::LawEnforcement
            ? "Captain Mara Keene" : "Nico Rellan";
    }
    Action ExpectedAction() const
    {
        if (Drill.Sport == 1) return Drill.Step % 3 == 0 ? Action::Guard : Drill.Step % 3 == 1 ? Action::Footwork : Action::Strike;
        if (Drill.Sport == 2) return Drill.Step % 2 == 0 ? Action::Strike : Action::Footwork;
        if (Drill.Sport == 3) return Action::Release;
        if (Drill.Sport == 5) return Action::Shot;
        return Drill.Step % 2 == 0 ? Action::Strike : Action::Guard;
    }
    std::string TrainingCue() const
    {
        if (!Drill.Active) return "";
        switch (Drill.Sport)
        {
            case 0: return Drill.Step % 2 == 0 ? "KARATE: face the partner, close to 120-280 cm, click LMB to strike." : "KARATE: face the partner and tap RMB to block the incoming attack.";
            case 1: return Drill.Step % 3 == 0 ? "JUDO: close to 90-200 cm; tap RMB to establish your grip." : Drill.Step % 3 == 1 ? "JUDO: keep close and sidestep 70 cm with A/D to break balance." : "JUDO: face the partner at 90-200 cm; LMB to complete a controlled throw.";
            case 2: return Drill.Step % 2 == 0 ? "TAEKWONDO: face the partner at 230-430 cm; LMB to kick." : "TAEKWONDO: sidestep 100 cm with A/D to evade the return kick.";
            case 3: return "ARCHERY: stand 500-2400 cm from the target. Center the crosshair; hold LMB 0.8-1.8 seconds, then release.";
            case 4: return Drill.Step % 2 == 0 ? "FENCING: face the partner at 140-330 cm; LMB to lunge and touch." : "FENCING: face the partner, tap RMB to parry before the cue expires.";
            case 5: return "SPORT SHOOTING: stand 500-2400 cm away. Aim at the target center; hold RMB at least 0.5 seconds, click LMB to fire.";
            default: return "";
        }
    }
    std::string Objective() const
    {
        if (IsModal()) return "Read the conversation and choose a response. There is no reading time limit.";
        if (Drill.Active) return TrainingCue();
        if (Data.Presentation == 0 || Data.Outfit < 0) return "Create your recruit.";
        if (!Data.Briefed) return "Meet General Ward at Vanguard Academy. Approach the marker and press E.";
        if (Data.Sport < 0) return "Choose any of Vanguard's six programs with General Ward.";
        if (!Data.Mentored) return "Meet Coach Imani Brooks before your first assessment.";
        if (Data.PlacementScore < 0) return "Complete your " + SportName() + " assessment. Its station is highlighted by the objective marker.";
        if (Data.RecruitChoice == 0) return "Speak to Jordan Cross about the disputed training result.";
        if (Data.ContactChoice == 0) return "Meet your visiting contact at the academy gate.";
        if (!Data.Assigned) return Data.ContactChoice == 2 ? "Return to Ward for an independent field assignment." : "Meet your contact again to receive the field assignment.";
        if (!Data.EvidenceFound) return "Visit the southeast records marker and inspect the disputed document.";
        if (Data.CaseChoice == 0) return "Decide who receives the evidence.";
        if (Data.FollowUpChoice == 0) return Data.CaseChoice == 1 ? "Take the report to the Vanguard Procurement Office." : Data.CaseChoice == 2 ? "Meet Civilian Oversight in South Ward." : "Meet the Docklands fixer.";
        if (!Data.Graduated) return "Return to the Academy Graduation Board for your written evaluation.";
        return "DEMO COMPLETE. Read your journal, save your progress, or practice any of the six sports.";
    }
    View GetView() const
    {
        switch (Screen)
        {
            case Dialogue::Profile: return {"Vanguard Admissions", "Your recruit", "You are eighteen, arriving with a place in Vanguard's athletic program. Enter your name, then choose your presentation.", {"Masculine presentation", "Feminine presentation"}};
            case Dialogue::Outfit: return {"Vanguard Admissions", "Academy clothing", "Choose the color of your academy uniform. Your appearance does not determine your opportunities.", {"Gold uniform", "Blue uniform", "Slate uniform"}};
            case Dialogue::Welcome: return {"General Adrian Ward", "Welcome to the Program", "The academy is quiet until the training bells ring. Ward closes your file.\n\n\"Talent brought you here, " + Data.Name + ". What you do with it will be your decision. Every program is open to you. Choose the one you want to begin.\"", {"Choose my program"}};
            case Dialogue::Sport: return {"General Adrian Ward", "Your first program", "Choose your starting discipline. Every program teaches different skills and has its own assessment. You will be able to practice other sports after this assignment.", {"Karate", "Judo", "Taekwondo", "Archery", "Fencing", "Sport Shooting"}};
            case Dialogue::Mentor: return {"Coach Imani Brooks", "Before the assessment", "Imani watches recruits cross the courtyard.\n\n\"We teach technique here, but we also teach people what to accept. A good instructor should welcome questions. When someone tells you a score is wrong, listen before you decide.\"", {"Ask how to handle unfair treatment", "Ask how to earn an instructor's trust"}};
            case Dialogue::Drill: return {"Coach Imani Brooks", std::string(ROWRules::SportName(PendingSport)) + " assessment", Intro(PendingSport) + "\n\nSix scored actions. Each action has a twelve-second limit. The grade combines technique, position and precision. A score of 60 passes. Conversations and the pause menu stop the assessment clock.", {"Begin assessment", "Return to the courtyard"}};
            case Dialogue::Result:
                if (Data.Graduated) return {"Coach Imani Brooks", "Practice result", "Score: " + std::to_string(Data.LastScore) + "/100 | Grade: " + Grade(Data.LastScore) + "\n\nYour practice record has been updated. Your original assessment and story decisions remain part of your history.", {"Continue"}};
                return {"Coach Imani Brooks", "Assessment result", "Score: " + std::to_string(Data.LastScore) + "/100 | Grade: " + Grade(Data.LastScore) + "\n\n" + (Data.LastScore >= 60 ? "\"You've met the standard. Jordan wants to talk before the results are filed. The supervised sparring ring in the northwest courtyard is also open to you now. Remember: the score tells us what happened during one test, not everything about a person.\"" : "\"This is a training assessment. Read the instructions, adjust your position, and try again. I will be here.\""), {"Continue"}};
            case Dialogue::Recruit: return {"Jordan Cross", "A disputed result", "Jordan waits beside the training hall.\n\n\"The instructor recorded my attempt as a pass. I missed the final action. My family thinks this place is my way forward. Are you going to tell them?\"\n\nYou have seen the record. You can report the discrepancy or protect Jordan's place. Neither choice erases what happened.", {"Report the discrepancy and ask for a fair retest", "Protect Jordan and keep the discrepancy private"}};
            case Dialogue::Contact: return {ContactName(), "An unexpected introduction", ContactBody(), {"Hear out the offer", "Decline the offer and ask Ward for another assignment"}};
            case Dialogue::Assignment: return {Data.ContactChoice == 2 ? "General Adrian Ward" : ContactName(), "The missing page", "A housing transfer in South Ward lists the Sterling Foundation as a beneficiary. A missing page may show whether residents agreed.\n\n\"Read the original record. Bring back what you find. You decide who deserves to see it.\"\n\nYour sporting background does not decide how you handle the evidence.", {"Accept the records assignment", "Take more time"}};
            case Dialogue::Evidence: return {"Case Record / Your Notes", "Three copies, one signature", "The document describes a transfer of occupied homes. One page bears a signature the residents say they never gave. A margin note references a Vanguard donor.\n\nThere is enough to raise questions, but no single page tells the whole story. Who receives your copy?", {"Give it to Vanguard command for an internal inquiry", "Give it to independent civilian oversight", "Trade it to Nico's criminal contacts"}};
            case Dialogue::Aftermath: return AftermathView();
            case Dialogue::Graduation: return {"General Ward and Coach Imani", "Your first evaluation", "Ward records your " + SportName() + " grade: " + Grade(Data.PlacementScore) + ". Imani adds a note about your choice concerning Jordan.\n\n\"You finished the assignment,\" she says. \"You also began relationships that will outlast it. Some people approve of your decisions. Others will remember them differently.\"\n\nYour academy introduction is complete. The city beyond these gates is the next chapter.", {"Complete the demo"}};
            case Dialogue::Info: return {"Academy Journal", "Courtyard notes", InfoText, {"Return"}};
            default: return {};
        }
    }
    bool Visit(Place place, int sport = -1)
    {
        if (IsBusy()) { Feedback = "Finish the active conversation or assessment first."; return false; }
        if (Data.Presentation == 0 || Data.Outfit < 0) { Open(Data.Presentation == 0 ? Dialogue::Profile : Dialogue::Outfit); return true; }
        if (place == Place::Academy)
        {
            if (!Data.Briefed) Open(Dialogue::Welcome);
            else if (Data.Sport < 0) Open(Dialogue::Sport);
            else if (Data.ContactChoice == 2 && !Data.Assigned) Open(Dialogue::Assignment);
            else Info("Ward: \"Your next step is your choice.\"\n\n" + Objective());
            return true;
        }
        if (!Data.Briefed || Data.Sport < 0) { Feedback = "Meet General Ward and choose your first program."; return false; }
        switch (place)
        {
            case Place::Mentor:
                if (!Data.Mentored) Open(Dialogue::Mentor); else Info("Imani: \"Your training should give you room to think.\"\n\n" + Objective());
                return true;
            case Place::Training:
                if (!Data.Mentored) { Feedback = "Speak to Coach Imani before beginning."; return false; }
                if (sport < 0 || sport >= 6) return false;
                if (!Data.Graduated && (sport != Data.Sport || Data.PlacementScore >= 0)) { Feedback = "Finish your first assignment before additional practice. " + Objective(); return false; }
                PendingSport = sport; Open(Dialogue::Drill); return true;
            case Place::Recruit:
                if (Data.PlacementScore < 0) { Feedback = "Complete your chosen assessment before discussing the result."; return false; }
                if (Data.RecruitChoice == 0) Open(Dialogue::Recruit); else Info(Data.RecruitChoice == 1 ? "Jordan: \"They offered me a retest. I still wish you had spoken to me first.\"" : "Jordan: \"You kept my place safe. I won't forget that. I still have to live with the score.\"");
                return true;
            case Place::Contact:
                if (Data.RecruitChoice == 0) { Feedback = "Resolve the conversation with Jordan first."; return false; }
                if (Data.ContactChoice == 0) Open(Dialogue::Contact);
                else if (!Data.Assigned && Data.ContactChoice == 1) Open(Dialogue::Assignment);
                else Info(ContactName() + ": \"A first introduction is only a beginning.\"\n\n" + Objective());
                return true;
            case Place::Assignment:
                if (Data.ContactChoice == 0) { Feedback = "Meet the visiting contact at the academy gate."; return false; }
                if (!Data.Assigned) Open(Dialogue::Assignment); else Info(Objective());
                return true;
            case Place::Evidence:
                if (!Data.Assigned) { Feedback = "Accept a records assignment before inspecting this file."; return false; }
                if (Data.CaseChoice == 0) { Data.EvidenceFound = true; Open(Dialogue::Evidence); } else Info("Your copy has already been delivered.\n\n" + Objective());
                return true;
            case Place::Command: case Place::Oversight: case Place::Fixer:
            {
                const int branch = place == Place::Command ? 1 : place == Place::Oversight ? 2 : 3;
                if (Data.CaseChoice != branch || Data.FollowUpChoice != 0) { Feedback = "This review is not your active destination. " + Objective(); return false; }
                Open(Dialogue::Aftermath); return true;
            }
            case Place::Graduation:
                if (Data.FollowUpChoice == 0) { Feedback = Objective(); return false; }
                if (!Data.Graduated) Open(Dialogue::Graduation); else Info("Your opening chapter is complete. Your journal and save retain your decisions.");
                return true;
            default: Info("Greyhaven's northern district reflects the city's wealth. South Ward carries the cost of decisions made elsewhere. Vanguard stands between them."); return true;
        }
    }
    bool Choose(int choice)
    {
        const View view = GetView();
        if (choice < 1 || choice > static_cast<int>(view.Options.size())) return false;
        Record(Data.Name, view.Options[static_cast<std::size_t>(choice - 1)]);
        const Dialogue previous = Screen;
        Screen = Dialogue::None;
        switch (previous)
        {
            case Dialogue::Profile: Data.Presentation = choice; Open(Dialogue::Outfit); break;
            case Dialogue::Outfit: Data.Outfit = choice - 1; break;
            case Dialogue::Welcome: Data.Briefed = true; Open(Dialogue::Sport); break;
            case Dialogue::Sport: Data.Sport = choice - 1; break;
            case Dialogue::Mentor: Data.Mentored = true; if (choice == 1) Data.Trust += 2; break;
            case Dialogue::Drill:
                if (choice == 1) { Drill = {}; Drill.Active = true; Drill.Sport = PendingSport; Feedback = TrainingCue(); }
                break;
            case Dialogue::Recruit:
                Data.RecruitChoice = choice;
                if (choice == 1) { Data.Law += 2; Data.Trust += 1; } else { Data.Crime += 1; Data.Trust -= 1; }
                break;
            case Dialogue::Contact: Data.ContactChoice = choice; break;
            case Dialogue::Assignment: if (choice == 1) Data.Assigned = true; break;
            case Dialogue::Evidence:
                Data.CaseChoice = choice;
                if (choice == 1) { Data.Law += 10; Data.Trust -= 5; }
                else if (choice == 2) { Data.Law += 5; Data.Trust += 20; }
                else { Data.Crime += 20; Data.Money += 150; Data.Trust -= 15; }
                break;
            case Dialogue::Aftermath:
            {
                const auto delta = ROWRules::FollowUpOutcome(Data.CaseChoice, choice);
                Data.Law += delta.Law; Data.Crime += delta.Crime; Data.Trust += delta.PublicTrust; Data.Money += delta.Cash;
                Data.FollowUpChoice = choice;
                break;
            }
            case Dialogue::Graduation: Data.Graduated = true; break;
            default: break;
        }
        return true;
    }
    bool Act(Action action, const Frame& frame)
    {
        if (!Drill.Active || IsModal() || action != ExpectedAction()) return false;
        if (!Finite(frame)) return false;
        double score = 0.0;
        const double reaction = std::max(0.5, 1.0 - std::max(0.0, Drill.Elapsed - 2.0) / 20.0);
        const double aim = std::clamp(frame.Aim, 0.0, 1.0);
        if (action == Action::Footwork)
        {
            const double travel = Drill.Sport == 1 ? 70.0 : 100.0;
            if (frame.LateralTravel < travel) return false;
            score = (Drill.Sport != 1 || frame.Distance <= 260.0) ? 100.0 * reaction : 0.0;
        }
        else if (Drill.Sport == 3)
        {
            const double draw = frame.HoldSeconds >= 0.8 && frame.HoldSeconds <= 1.8 ? 1.0 : frame.HoldSeconds >= 0.4 && frame.HoldSeconds <= 2.5 ? 0.6 : 0.0;
            if (frame.Distance >= 500.0 && frame.Distance <= 2400.0) score = 100.0 * aim * draw;
        }
        else if (Drill.Sport == 5)
        {
            if (frame.Distance >= 500.0 && frame.Distance <= 2400.0 && frame.HoldSeconds >= 0.5) score = 100.0 * aim;
        }
        else
        {
            const double lo[] = {120, 90, 230, 0, 140, 0};
            const double hi[] = {280, 200, 430, 0, 330, 0};
            if (frame.Distance >= lo[Drill.Sport] && frame.Distance <= hi[Drill.Sport]) score = 100.0 * aim * reaction;
        }
        Consume(static_cast<int>(std::lround(std::clamp(score, 0.0, 100.0))));
        return true;
    }
    void Tick(double seconds, const Frame& frame)
    {
        if (!Drill.Active || IsModal() || !std::isfinite(seconds) || seconds < 0.0) return;
        // Expire deadlines before considering movement observed in this frame.
        double remaining = std::min(seconds, 3600.0);
        bool expired = false;
        while (Drill.Active && remaining >= CueSeconds - Drill.Elapsed)
        {
            remaining -= CueSeconds - Drill.Elapsed;
            Consume(0); expired = true;
        }
        if (Drill.Active)
        {
            Drill.Elapsed += remaining;
            if (!expired && ExpectedAction() == Action::Footwork) Act(Action::Footwork, frame);
        }
    }
    void ShowInfo(const std::string& text) { if (!IsBusy()) Info(text); }
    std::string Serialize() const
    {
        std::ostringstream out; out.imbue(std::locale::classic()); out << std::setprecision(17);
        out << "ROW_NARRATIVE " << SaveSchema << '\n' << std::quoted(Data.Name) << '\n';
        out << Data.Presentation << ' ' << Data.Outfit << ' ' << Data.Briefed << ' ' << Data.Sport << ' ' << Data.Mentored << ' ';
        for (int x : Data.XP) out << x << ' ';
        for (int x : Data.Best) out << x << ' ';
        out << Data.LastScore << ' ' << Data.PlacementScore << ' ' << Data.RecruitChoice << ' ' << Data.ContactChoice << ' ' << Data.Assigned << ' ' << Data.EvidenceFound << ' ' << Data.CaseChoice << ' ' << Data.FollowUpChoice << ' ' << Data.Graduated << ' ' << Data.Law << ' ' << Data.Crime << ' ' << Data.Trust << ' ' << Data.Sports << ' ' << Data.Money << '\n';
        out << static_cast<int>(Screen) << ' ' << PendingSport << ' ' << Drill.Active << ' ' << Drill.Sport << ' ' << Drill.Step << ' ' << Drill.Total << ' ' << Drill.Misses << ' ' << Drill.Elapsed << '\n';
        out << std::quoted(InfoText) << '\n' << Journal.size() << '\n';
        for (const auto& line : Journal) out << std::quoted(line) << '\n';
        return out.str();
    }
    bool Restore(const std::string& payload)
    {
        if (payload.size() > 2 * 1024 * 1024) return false;
        Session candidate;
        std::istringstream in(payload); in.imbue(std::locale::classic()); std::string tag; int version = 0; int dialog = 0; std::size_t count = 0;
        auto& s = candidate.Data; auto& a = candidate.Drill;
        if (!(in >> tag >> version) || tag != "ROW_NARRATIVE" || version != SaveSchema || !(in >> std::quoted(s.Name))) return false;
        if (!(in >> s.Presentation >> s.Outfit >> s.Briefed >> s.Sport >> s.Mentored)) return false;
        for (int& x : s.XP) if (!(in >> x)) return false;
        for (int& x : s.Best) if (!(in >> x)) return false;
        if (!(in >> s.LastScore >> s.PlacementScore >> s.RecruitChoice >> s.ContactChoice >> s.Assigned >> s.EvidenceFound >> s.CaseChoice >> s.FollowUpChoice >> s.Graduated >> s.Law >> s.Crime >> s.Trust >> s.Sports >> s.Money)) return false;
        if (!(in >> dialog >> candidate.PendingSport >> a.Active >> a.Sport >> a.Step >> a.Total >> a.Misses >> a.Elapsed)) return false;
        if (dialog < 0 || dialog > static_cast<int>(Dialogue::Info)) return false;
        candidate.Screen = static_cast<Dialogue>(dialog);
        if (!(in >> std::quoted(candidate.InfoText)) || candidate.InfoText.size() > 16384 || !(in >> count) || count > 256) return false;
        candidate.Journal.clear();
        for (std::size_t i = 0; i < count; ++i) { std::string entry; if (!(in >> std::quoted(entry)) || entry.size() > 16384) return false; candidate.Journal.push_back(entry); }
        in >> std::ws;
        if (!in.eof() || !candidate.Validate()) return false;
        *this = candidate; Feedback = "Saved progress restored."; return true;
    }
private:
    State Data;
    Assessment Drill;
    Dialogue Screen = Dialogue::Profile;
    int PendingSport = -1;
    std::string InfoText;
    std::vector<std::string> Journal;
    static bool Finite(const Frame& f) { return std::isfinite(f.Distance) && std::isfinite(f.Aim) && std::isfinite(f.HoldSeconds) && std::isfinite(f.LateralTravel); }
    void Record(const std::string& speaker, const std::string& text)
    {
        if (Journal.size() == 256) Journal.erase(Journal.begin());
        Journal.push_back(speaker + ": " + text);
    }
    void Open(Dialogue d) { Screen = d; const auto v = GetView(); Record(v.Speaker, v.Text); }
    void Info(const std::string& text) { InfoText = text; Open(Dialogue::Info); }
    void Consume(int score)
    {
        Drill.Total += score;
        if (score < 60) ++Drill.Misses;
        ++Drill.Step; Drill.Elapsed = 0.0;
        Feedback = score >= 60 ? "Action scored " + std::to_string(score) + "/100." : "Missed action. Reposition, read the next cue, and continue.";
        if (Drill.Step < Rounds) return;
        Drill.Active = false;
        Data.LastScore = Drill.Total / Rounds;
        const auto sport = static_cast<std::size_t>(Drill.Sport);
        Data.Best[sport] = std::max(Data.Best[sport], Data.LastScore);
        if (Data.LastScore >= 60)
        {
            const int old = Data.XP[sport]; Data.XP[sport] = std::min(300, old + 30);
            if (Data.XP[sport] > old) Data.Sports += 5;
            if (Drill.Sport == Data.Sport && Data.PlacementScore < 0) Data.PlacementScore = Data.LastScore;
        }
        Open(Dialogue::Result);
    }
    static std::string Intro(int sport)
    {
        switch (sport)
        {
            case 0: return "Alternate controlled strikes and blocks. Move into range and face the partner; react promptly to each cue.";
            case 1: return "Establish a grip, move sideways to break balance, then complete the throw. Repeat the sequence with control.";
            case 2: return "Use the longer kicking distance, then sidestep the return. Range and lateral movement are scored.";
            case 3: return "Aim through the crosshair, draw the bow, and release. Accuracy and draw duration determine each shot's score.";
            case 4: return "Maintain fencing distance. Alternate a forward touch and a defensive parry while facing your partner.";
            case 5: return "Use the marked sporting target. Hold your aim steady before each shot; the distance from its center determines accuracy.";
            default: return "";
        }
    }
    std::string ContactBody() const
    {
        if (ROWStoryRouting::NetworkForSport(Data.Sport) == ROWStoryRouting::ContactNetwork::LawEnforcement)
            return "Captain Keene introduces herself through " + ContactDescription() + ".\n\n\"Your instructor spoke about your " + SportName() + " assessment. I need someone willing to notice details and question an easy answer. There is a record in South Ward that deserves another look.\"\n\nAn introduction is an invitation, not a career commitment.";
        const std::string group = Data.PlacementScore >= 95 ? "His connections belong to a powerful, disciplined criminal organization." : Data.PlacementScore >= 80 ? "He works with an established, organized criminal group." : Data.PlacementScore >= 65 ? "He knows a neighborhood crew with growing influence." : "His friends know the city, but they work as freelancers and rarely agree on a plan.";
        return "Nico has heard about your " + SportName() + " assessment. " + group + "\n\n" + (Data.PlacementScore >= 80 ? "\"The people I work with are disciplined. They notice control under pressure. There is a document in South Ward that interests them.\"" : "\"We know this city, even if we don't always have a plan. Your training could help. There's a document in South Ward we want to see.\"") + "\n\nYou can listen without promising loyalty, or refuse the introduction.";
    }
    View AftermathView() const
    {
        if (Data.CaseChoice == 1) return {"General Adrian Ward", "The official account", "Ward thanks you for using the academy's channels. The report omits a donor's name.\n\n\"We can approve this account today,\" he says, \"or request a more difficult inquiry.\"", {"Approve the official account", "Challenge the omission and request a fuller inquiry"}};
        if (Data.CaseChoice == 2) return {"Civilian Oversight Reviewer", "What the public sees", "The reviewer believes the document should be examined publicly. A witness asks to remain unnamed.\n\n\"Transparency and protection can pull in different directions. What do you want us to release?\"", {"Release the full document", "Protect the witness's identifying details"}};
        return {"Nico Rellan", "The second offer", "Nico places another offer on the table.\n\n\"The page was useful. There is more money if you keep helping.\"\n\nA first decision does not require you to make the same decision again.", {"Accept the payment and continue the relationship", "Refuse and give a tip to civilian oversight"}};
    }
    bool Validate() const
    {
        const auto& s = Data; const auto& a = Drill;
        if (!ValidName(s.Name) || s.Presentation < 0 || s.Presentation > 2 || s.Outfit < -1 || s.Outfit > 2 || s.Sport < -1 || s.Sport > 5) return false;
        if (s.LastScore < -1 || s.LastScore > 100 || s.PlacementScore < -1 || s.PlacementScore > 100 || (s.PlacementScore >= 0 && s.PlacementScore < 60)) return false;
        if (s.RecruitChoice < 0 || s.RecruitChoice > 2 || s.ContactChoice < 0 || s.ContactChoice > 2 || s.CaseChoice < 0 || s.CaseChoice > 3 || s.FollowUpChoice < 0 || s.FollowUpChoice > 2) return false;
        for (int x : s.XP) if (x < 0 || x > 300) return false;
        for (int x : s.Best) if (x < 0 || x > 100) return false;
        for (int x : {s.Law, s.Crime, s.Trust, s.Sports, s.Money}) if (x < -100000 || x > 100000) return false;
        if (s.Briefed && (s.Presentation == 0 || s.Outfit < 0)) return false;
        if (s.Sport >= 0 && !s.Briefed) return false;
        if (s.Mentored && s.Sport < 0) return false;
        if (s.PlacementScore >= 0 && !s.Mentored) return false;
        if (s.RecruitChoice && s.PlacementScore < 0) return false;
        if (s.ContactChoice && !s.RecruitChoice) return false;
        if (s.Assigned && !s.ContactChoice) return false;
        if (s.EvidenceFound && !s.Assigned) return false;
        if (s.CaseChoice && !s.EvidenceFound) return false;
        if (s.FollowUpChoice && !s.CaseChoice) return false;
        if (s.Graduated && !s.FollowUpChoice) return false;
        if (PendingSport < -1 || PendingSport > 5 || a.Sport < -1 || a.Sport > 5 || a.Step < 0 || a.Step > Rounds || a.Total < 0 || a.Total > 100 * a.Step || a.Misses < 0 || a.Misses > a.Step || !std::isfinite(a.Elapsed) || a.Elapsed < 0.0 || a.Elapsed >= CueSeconds) return false;
        if (a.Active && (a.Sport < 0 || a.Step >= Rounds || !s.Mentored || Screen != Dialogue::None)) return false;
        switch (Screen)
        {
            case Dialogue::Profile: return s.Presentation == 0 && !s.Briefed;
            case Dialogue::Outfit: return s.Presentation != 0 && s.Outfit < 0 && !s.Briefed;
            case Dialogue::Welcome: return s.Outfit >= 0 && !s.Briefed;
            case Dialogue::Sport: return s.Briefed && s.Sport < 0;
            case Dialogue::Mentor: return s.Sport >= 0 && !s.Mentored;
            case Dialogue::Drill: return PendingSport >= 0 && s.Mentored;
            case Dialogue::Result: return s.LastScore >= 0 && !a.Active;
            case Dialogue::Recruit: return s.PlacementScore >= 60 && s.RecruitChoice == 0;
            case Dialogue::Contact: return s.RecruitChoice != 0 && s.ContactChoice == 0;
            case Dialogue::Assignment: return s.ContactChoice != 0 && !s.Assigned;
            case Dialogue::Evidence: return s.EvidenceFound && s.CaseChoice == 0;
            case Dialogue::Aftermath: return s.CaseChoice != 0 && s.FollowUpChoice == 0;
            case Dialogue::Graduation: return s.FollowUpChoice != 0 && !s.Graduated;
            default: return true;
        }
    }
};
}
