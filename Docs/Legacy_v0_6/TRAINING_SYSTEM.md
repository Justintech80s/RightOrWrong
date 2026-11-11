# RIGHT OR WRONG — Authentic Sport Training Design

**Design document only.** v0.2 has a simplified placeholder karate fight but not these training systems. The goal is credible sporting mechanics, without conflating regulated sport rules with military or crime gameplay.

## Universal training loop

**Enroll → Learn a technique → Practice in a supervised drill → Demonstrate control → Spar or take timed assessment → Enter sanctioned events → Earn rankings and selection opportunities.**

Implement coaches with instructions, stamina management, valid/invalid actions, injury prevention, equipment checks, judges, and replayable tutorials. Record discipline-specific ratings rather than a universal combat rating.

| Event | Training gameplay to develop | Competition gameplay to develop | What is currently coded |
|---|---|---|---|
| Karate | Distance management, stance, guard, controlled strikes, timing | Kumite with legal-target scoring, referee, fouls, timer; kata as a separate judged path | Basic hit/block/health placeholder only |
| Judo | Footwork, grip exchanges, balance breaks, safe falls, holds | Throws, transitions, scoring, referee resets, penalties | Not implemented |
| Taekwondo | Stance, distance, protected target zones, kicking precision, endurance | Structured sparring, timing, electronic-style scoring, penalties | Not implemented |
| Archery | Consistent setup, draw/release timing, bow type, distance and wind simulations | Rounds, target rings, scoring, equipment class and event format | Not implemented |
| Fencing | Advance/retreat/lunge, guards, point control, defensive timing | Foil/épée/sabre rule sets, priority where applicable, scoring and piste boundaries | Not implemented |
| Sport Shooting | Safe range procedure, stable sight picture, controlled trigger interaction, competition focus | Regulated air pistol / air rifle target scoring and timed competition formats; optional clay disciplines later | Not implemented |

## Why this matters to the story

An elite recruit who specializes in Judo should feel different from an athlete whose greatest skill is fencing, and their respective sports should still be worth playing after induction. A law-enforcement career may reward observation and restraint; an athletics career may reward formal rankings; criminal-world missions may offer money but damage eligibility or public trust. These consequences are narrative systems, not claims that regulated sport directly trains illicit violence.

## Rules fidelity

Use the **current official rulebooks before implementing competition logic**: WKF (Karate), IJF (Judo), World Taekwondo, World Archery, FIE (Fencing), and ISSF (Sport Shooting). The competition formats and scoring rules can change; verify the rules and obtain any required licensing before commercial release. Do not market the game as an officially licensed Olympic title unless a license has been secured.

## Next demonstrable milestone

1. Implement the first karate **timed, judged match**, with valid target recognition, legal move list, event scoring, referee reset, and replay.
2. Implement one **non-combat training drill** (archery scoring or fencing footwork) to prove the modular sport framework.
3. Integrate training achievement into the Vanguard introductory assessment as a *skill check*, not a separate minigame.
4. Implement NPC coach and beginner tutorial UI.
