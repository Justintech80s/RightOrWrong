#pragma once
// Engine-independent story routing. Sport selection is NOT a moral alignment.
namespace ROWStoryRouting {
 enum class ContactNetwork { Unselected, LawEnforcement, CriminalUnderworld };
 enum class ContactTier { Unavailable, Freelance, Neighborhood, Established, Elite };
 inline ContactNetwork NetworkForSport(int sport) {
   switch (sport) {
     case 1: case 3: case 5: return ContactNetwork::LawEnforcement; // Judo, Archery, Sport Shooting
     case 0: case 2: case 4: return ContactNetwork::CriminalUnderworld; // Karate, Taekwondo, Fencing
     default: return ContactNetwork::Unselected;
   }
 }
 inline ContactTier TierForPerformance(int xp, int mistakes) {
   if (xp < 0 || mistakes < 0) return ContactTier::Unavailable;
   const int score = xp - mistakes * 15;
   if (score >= 240) return ContactTier::Elite;
   if (score >= 150) return ContactTier::Established;
   if (score >= 60) return ContactTier::Neighborhood;
   return ContactTier::Freelance;
 }
 inline const char* ContactDescription(ContactNetwork network, ContactTier tier) {
   if (network == ContactNetwork::Unselected || tier == ContactTier::Unavailable) return "No contacts yet";
   if (network == ContactNetwork::LawEnforcement) {
     switch (tier) {
       case ContactTier::Elite: return "Specialized investigative unit";
       case ContactTier::Established: return "Experienced detectives";
       case ContactTier::Neighborhood: return "Local police division";
       default: return "Underfunded community precinct";
     }
   }
   switch (tier) {
     case ContactTier::Elite: return "Elite disciplined criminal organization";
     case ContactTier::Established: return "Established organized crime network";
     case ContactTier::Neighborhood: return "Neighborhood criminal group";
     default: return "Knowledgeable but disorganized freelance criminals";
   }
 }
}
