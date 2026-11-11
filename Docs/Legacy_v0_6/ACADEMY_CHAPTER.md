# Chapter 1 — The Invitation (v0.3 graybox walkthrough)

## Narrative premise

The 18-year-old protagonist was raised in Greyhaven, a divided city. The new state-backed Vanguard Response Directorate accepts Olympic-track athletes into a supervised recruiting program. Decorated commander **General Adrian Ward** sets strict standards; investor **Elias Vale** provides politically sensitive funding. The player is free to compete, serve, question authority, or pursue illicit opportunities—but their actions affect future access and reputation.

**Design safeguard:** an 18-year-old athlete is an *unsworn recruit*. Vanguard's current gameplay focuses on sport qualification and an introductory case. The prototype does not confer actual police authority or teach real-world paramilitary tactics.

## First playable sequence

| Beat | Player action | Prototype result |
|---|---|---|
| 1. Arrival | Choose `1` male / `2` female | Profile flag stored; same placeholder geometry for both profiles |
| 2. Induction | Walk north to Vanguard Academy / General Ward; `E` | General's short introduction, academy stage unlock |
| 3. Specialization | Choose sport with keys `1`–`6` | Permanent initial specialization, no appearance customization yet |
| 4. The cohort | Talk with Jordan Cross and coach Imani Brooks (`E`) | Dialogue text reveals opposing views about duty and inequality |
| 5. Training | Visit your green sport station and press `E` | Three round 12-second keyboard-response tutorial |
| 6. Certification | Respond to prompts with `1` position / `2` technique / `3` control | 30 XP in that sport; after your **primary** drill first succeeds, field assignment unlocks |
| 7. The first file | Visit police station, collect southeast evidence marker | Evidence decision opens: command / oversight / fixer |
| 8. The split | Choose `1`, `2`, or `3` | Records first choice, adjusts trust/favor/reputation |
| 9. Consequence mission | Travel to branch-specific destination | Two distinct follow-up choices (`1 or 2`) and additional effects |
| 10. Continue | Keep training, enter dojo, explore districts | Progress is saved with `F5` / loaded with `F9` |

**For the follow-up destination:**

- **Command path (1):** Vanguard procurement office, northern Highcrest. Approve the official account or challenge the contract.
- **Oversight path (2):** civilian oversight office, southwest South Ward. Disclose a full record or shield witness names.
- **Fixer path (3):** Docklands contact. Take payment or reverse course and provide a tip to oversight.

### Sports training cues

The six stations are **playable tutorial interaction prototypes**, *not realistic simulations yet*:

- **Karate:** position → technique → control.
- **Judo:** technique → position → control.
- **Taekwondo:** position → control → technique.
- **Archery:** control → position → technique.
- **Fencing:** position → technique → control.
- **Sport shooting:** control → technique → position.

A missed response or response after 12 seconds counts as an error; after two errors the drill ends. A completed drill adds **30 XP up to 300**. The character can continue to train other disciplines after passing their first assessment.

Sport-specific stance animation, judges, opponent AI, genuine biomechanics, legal rules, arrows/foils/projectiles, and world-scale facilities have **not** been implemented.

## Story characters (working names)

**General Adrian Ward:** former commander who sincerely values public service, but depends on disputed private funding.

**Elias Vale:** investor who publicly supports the city's safety initiatives, with interests linked to procurement contracts.

**Jordan Cross:** rival recruit whose loyalty to command may conflict with personal relationships.

**Imani Brooks:** South Ward youth coach who sees the dangers of funding inequality.

**Dr. Mara Chen:** independent oversight official. Her office is represented by a marker, not an NPC yet.

**Nico Serrano:** criminal intermediary. His meeting is represented by a marker, not an NPC yet.

## What we test next inside Unreal Editor

Verify collision around the six training platforms, readability of floating text, camera clipping, interaction distances, key bindings `1` through `6`, correct XP persistence after `F5`/`F9`, and every evidence → follow-up branch with a **fresh** save. Verify the playable character never falls through the basic ground and that runtime-spawned geometry appears after compiling.
