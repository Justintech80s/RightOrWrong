# Eight-Stage Demo Source 1 — implementation and verification

Date: November 11, 2026. Status: **source implemented; Unreal gameplay and Windows executable not verified or produced**.

## What changed

The current written-narrative source was extended with an actual connected sparring model, a spawned AI opponent, a reachable campus/neighborhood blockout, two-slot checkpoint recovery, clearer wayfinding and stricter Windows packaging checks. The player can still select any of the six first programs. All conversations are written and wait for the player.

This edition continues the recovered v0.6 archive through Written Narrative Demo 1. Version 2.1 was not available; its contents have not been audited or merged. Baseline archive SHA-256: `08a0cdfbd37b1cc7cdbff3216680d0afd35c1d214da28958cc58917618c5a1d0`.

## Stage-by-stage evidence

| Stage | Source work completed | What has actually been tested | What remains unverified |
| --- | --- | --- | --- |
| 1 | Camera pitch limits, collision boom, walking/running/jumping, input reset, strike/guard poses | Source/configuration checks | Engine movement, camera, focus and animation |
| 2 | Common blockout geometry: courtyard, seating/planters, rooms, training facilities, sparring ring, atmosphere/light setup | Conservative capsule-clearance path test | Unreal collisions, floor stepping and rendered lighting |
| 3 | Free selection among six assessments; scoring, deadlines, retries, grades | Actual narrative/rules code, all sports and boundary cases | Physical input, crosshair/target agreement, equipment visuals |
| 4 | Shared deterministic combat model; live director controls; CharacterMovement opponent; telegraphs, guard, stamina, counters, time decisions and rematches | 1,226 assertions on outcomes, ranges, occlusion inputs, guard break, timing, repeated rewards, bad input and checkpoint continuation | Engine AI movement/collision, poses and real controller timing |
| 5 | Entire introductory mission, initial contact and branching records assignment, manual text UI/journal | 144 complete routes and 14,478 assertions | Slate rendering, clicks, typing and packaged end-to-end play |
| 6 | South Ward streets, crossings, four enterable blockout rooms, local market/café text | All 20 destinations reachable over 28,147 sampled capsule-clearance locations using the runtime layout | Actual engine capsules/camera and interior visibility |
| 7 | Save schema 2; rotating generations; verify writes by reading them back; newest-valid selection; schema-1 migration; saved AI and dialogue state | Shared snapshots, malformed inputs, deterministic continuation and decimal-locale portability | Unreal serialization tests authored but NOT RUN; disk failure/recovery not exercised |
| 8 | Windows wrapper and compile/setup/automation/cook/package pipeline; required PE64 binary/content validation | Plan, parser and refusal/failure gates | Actual editor compile, map generation, executable, gameplay and PlugVersal |

The reachability test excludes overhead ceilings and low floor surfaces, uses a conservative upright capsule footprint, and checks connected 50 cm samples. It does not simulate slopes, character stepping, physics, engine navigation or the camera. The opponent follows the player in an open supervised ring; it is not citywide navigation or a general NPC scheduler.

## Actual checks completed

- Six standalone C++ programs compiled and ran with C++17, `-Wall -Wextra -Werror -pedantic` and undefined-behavior sanitization.
- Narrative: 144 complete branch combinations and 14,478 assertions. The assertion count includes repeated state checks and does not mean that many unique play sessions.
- Combat: 1,226 assertions, including a countering strategy that wins a complete match, indefinite guarding that breaks and loses, timer decisions, leaving the ring, deterministic mid-windup reload, invalid snapshots and portable decimal serialization.
- Blockout: 20 named runtime destinations reachable; ring spawn points and doorway/wall constraints checked.
- Python: 11 tests for build report gates and package validation. Binary test fixtures are temporary headers, not runnable programs, and are not distributed as game executables.
- Source preflight: 84 structural/configuration assertions. These do NOT run UnrealHeaderTool or compile Unreal adapters.
- Existing mission JSON and legacy progression/pedagogy/routing tests passed.
- Dialogue exporter compiled with strict warnings and regenerated the written script from current runtime dialogue.
- Every Python source parsed successfully. The Unreal setup script was syntax checked only.
- Windows build driver was actually invoked and refused on this Linux workspace with exit code 1. No Unreal compile or package stage ran. The account's connected-host list was empty.

See `BuildEvidence/Verification.json` and individual logs for exact commands and exit codes. `Windows_Pipeline_Plan.json` is explicitly a plan, not a completed run. `Engine_Environment.json` records the engine/host blocker.

## Save behavior and limitations

Two complete slots alternate. The newest valid checkpoint is preserved while the other slot is written. A save is then read back and compared. Loading validates candidate narrative/combat states before changing the live session. Invalid candidates are ignored; if both fail, the live session remains unchanged. These are implemented engine-side safeguards, not a claim that real disk failure has been tested here.

Combat health, stamina, timers, phase, record, pending result and opponent location are saved. Held buttons are intentionally released on load. Schema-1 narrative saves start with a fresh combat record. A now-obstructed saved position moves to safe ground. New-recruit confirmation does not erase existing saves automatically.

## Remaining gates for a working Windows demo

1. Supply a Windows build computer with the project-targeted Unreal 5.8 and compatible C++ toolchain.
2. Run the build wrapper and fix any UHT/compiler or API issues the actual engine identifies.
3. Generate the map/material in Unreal and pass both authored engine automation tests.
4. Play the complete route, training and sparring in the actual editor and cooked package. Test save/load and invalid-slot recovery on disk.
5. Confirm the package on target Windows hardware and through PlugVersal, including discovery/launch, focus/input and saves.

Until those gates pass, this is **a tested source prototype, not a verified playable Windows demo**. It has no finished human/equipment animation set, traffic simulation, populated city, complete academy curriculum, full campaign or online mode. The realistic art direction is retained as the target; blockout primitives are temporary.

## Primary API references consulted

- [Unreal character movement](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/UCharacterMovementComponent?lang=en-US)
- [Pawn movement input](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/APawn/AddMovementInput)
- [Player camera manager](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/APlayerCameraManager?lang=en-US)
- [Sky atmosphere actor and header](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/ASkyAtmosphere)
- [Sky light](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Engine/USkyLightComponent?lang=en-US)
- [Directional light](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/UDirectionalLightComponent)
- [Slate in game](https://dev.epicgames.com/documentation/unreal-engine/using-slate-in-game-in-unreal-engine?lang=en-US)

Documentation checks do not replace compiling this project against the engine.
