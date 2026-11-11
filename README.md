# RIGHT OR WRONG — Eight-Stage Demo Source 1

Written-dialogue Unreal Engine source for the requested academy demo. **This ZIP contains source, tests and build scripts, not a Windows executable.** Stages 1–7 have source implementations; Unreal compilation, rendered gameplay and packaged Windows operation remain unverified. Stage 8 requires a Windows computer with the engine and its C++ toolchain.

Provenance: continued from the recovered v0.6 source and Written Narrative Demo 1. The separately mentioned v2.1 was unavailable. This is not represented as a verified v2.1 upgrade. Characters, equipment and buildings are primitive blockouts, not finished realistic art.

## The eight requested stages

| Stage | Implementation in this edition | Evidence / remaining gate |
| --- | --- | --- |
| 1. Movement and camera | WASD, sprint, jump, third-person camera collision, pitch limits, modal input handling | Source present; physical controls/camera require Unreal playtest |
| 2. Vanguard courtyard | Courtyard paving, seating, planters, training stations, evaluation/procurement rooms and sparring ring | Shared geometry and destination reachability tested; rendering/physics untested |
| 3. Athletic program | All six programs selectable; distinct range/aim/movement/timing rules, six actions, grades and retries | Shared runtime rules tested; actual equipment/input must be verified in Unreal |
| 4. AI melee competition | Opponent approaches using CharacterMovement, telegraphs attacks, guards, recovers; player strikes/blocks; stamina, timer, counters, wins/losses, rematch | 1,226 model assertions passed; live opponent physics/animation untested |
| 5. Written dialogue and mission | Recruit setup, Ward/Imani, Jordan dilemma, initial organization contact, branching records mission, journal and evaluation | 144 complete narrative paths passed; Slate visual/input verification pending |
| 6. Greyhaven neighborhood | South Ward roads, crossings, enterable market/café/records/oversight blockouts, two local written encounters | All 20 destinations reachable in the shared capsule-clearance model; real collisions pending |
| 7. Save/load | Two rotating versioned checkpoints, read-back checks, invalid-save fallback, current dialogue/training/match/positions/settings | Core snapshots tested; two Unreal SaveGame automation tests authored but not run; disk fallback needs engine test |
| 8. Windows x64 executable | Compile → generate map/material → require engine tests → cook/package → validate PE64 binaries/content | Build driver and failure gates tested; actual package NOT CREATED |

## Play sequence once built

Create an eighteen-year-old recruit, select a uniform, meet Ward and freely choose Karate, Judo, Taekwondo, Archery, Fencing or Sport Shooting. Meet Coach Brooks and pass the chosen assessment. You may then visit the **northwest supervised sparring ring** for an optional two-minute exhibition; no program change is required.

Speak to Jordan and the visiting contact. Accept the contact's records assignment or decline and take Ward's alternative. Explore South Ward, inspect the records, choose command/oversight/criminal contacts and complete the follow-up. Return to the evaluation board, then save. All dialogue is written and waits for manual advance. No voice actors or speech-generation services are required.

Free exploration is available throughout. South Ward is an academy field-exercise area; the intro evaluation does not simulate the full academy curriculum. Crime/law introductions do not permanently lock future choices. The first passing assessment determines the initial contact placement; sparring results do not rewrite it.

## Assessment controls

| Program | Assessment |
| --- | --- |
| Karate | Close range, facing, alternating LMB strikes / RMB blocks |
| Judo | RMB grip, lateral movement, LMB controlled throw |
| Taekwondo | Longer-range LMB kick, lateral evade |
| Archery | Crosshair aim, hold LMB 0.8–1.8 seconds, release |
| Fencing | Correct distance, alternating LMB touch / RMB parry |
| Sport Shooting | Aim, hold RMB at least 0.5 seconds, LMB shot |

These are simplified training mechanics with primitive targets, not finished simulations of those sports. Six scored actions each have a twelve-second limit. Pass at 60; B at 75; A at 90. Written conversations, journal and menu freeze assessment/match clocks.

## Sparring controls

LMB strikes within 2.2 metres while facing the opponent; RMB guards. Release guard to strike. Read the incoming-attack warning and watch the red opponent, then block or step away. Recovery exposes a counter window. Guarding costs stamina, and holding it indefinitely can break the guard. Countering and resting between attacks are rewarded. Leaving the painted ring forfeits. Higher health wins when two minutes expire; equal health draws. Results are recorded once and rematches retain the record.

## Build on Windows

The project targets **Unreal Engine 5.8**. Install that engine with its compatible Windows C++ toolchain and Python 3. No engine installer, game assets from other titles, or licensed third-party asset pack is included.

From PowerShell in this folder:

```powershell
.\Build_Windows_Demo.ps1 -EngineRoot "C:\Program Files\Epic Games\UE_5.8"
```

Or run the driver directly:

```powershell
python Scripts/build_windows.py --engine-root "C:\Program Files\Epic Games\UE_5.8" --output "C:\ROW_Builds\Demo_01"
```

Use a new output folder per build. `--plan` prints commands only; `--compile-only` stops after editor compilation. `Scripts/setup_demo.py` runs inside Unreal and creates the absent map and color material. The setup script preserves existing assets. Both named Unreal automation tests must pass before packaging. The archive must contain both the launcher and Win64 game binary plus cooked `.pak` content; binaries must have AMD64 PE32+ headers.

Successful packaging is still marked **GAMEPLAY REVIEW REQUIRED**. Follow `Docs/WINDOWS_DEMO_ACCEPTANCE.md` and the engine checklist. Distribute the whole packaged folder, not an isolated `.exe`. PlugVersal discovery, launch, focus/input and saves must be tested against that exact package before claiming compatibility.

## Controls and saves

WASD move; mouse look; Shift run; Space jump; E interact; LMB action/draw; RMB block/aim; 1–6 or mouse choose responses; Enter advances a single response; Tab journal; Esc menu; F5 save; F9 load. The pause menu has 100/120/140% text sizing and a confirmed new-recruit action. A new recruit does not overwrite saves until Save is chosen.

Checkpoints alternate between `RightOrWrong_WrittenNarrative_1` and `RightOrWrong_WrittenNarrative_1_Backup`. Loading selects the newest valid generation and preserves the live session if neither validates. Schema 1 saves from Written Narrative Demo 1 migrate with a fresh sparring record; older prototype slots are left alone. Current conversation, mission choices, assessment round/time, AI windup/cooldown/health/stamina, player/opponent transforms and text size are retained. Re-press held controls after loading. A position obstructed by the revised layout moves to safe ground.

## Verification without Unreal

```sh
python Scripts/run_checks.py --sanitize
```

Requires Python 3 and GCC or Clang with C++17. Six programs compile and run, including the exact narrative/combat/layout code used by the Unreal director. Python tests reject missing/failed/skipped engine reports, missing content and wrong binary architecture. Logs and actual results are in `BuildEvidence`; they are not a substitute for an engine build.

See `Docs/IMPLEMENTATION_AND_VERIFICATION.md` for evidence and limits, `Docs/WRITTEN_DIALOGUE.md` for runtime narrative text, and `Docs/SPARRING_AND_DISTRICT_DIALOGUE.md` for the added encounters. Historical reports remain in `Docs/Legacy_v0_6`.
