# Windows demo release gate — NOT RUN

This document is a test procedure, not a passed certificate. Record the exact source ZIP hash, engine/compiler versions, package hash, Windows version, GPU/driver, resolution, frame-time measurements and logs. Do not mark a stage passed solely because its source exists or the package command returned success.

| Gate | Procedure | Required evidence | Current result |
| --- | --- | --- | --- |
| Build | Run Build_Windows_Demo.ps1; repair any UHT/C++ failures; generate level/material | Complete build and asset logs | BLOCKED: no Windows/Unreal host |
| Engine tests | Run both RightOrWrong.Narrative.SaveGameRoundTrip and RightOrWrong.Combat.SaveGameRoundTrip | Fresh successful automation export; no failed/skipped tests | NOT RUN |
| Package | Cook/archive the game; validate PE64 launcher, game binary and cooked content | BuildOutput pipeline_result.json plus package hashes | NOT CREATED |
| Movement/camera | Walk/run/jump across campus; collide with walls; look up/down; navigate every doorway | Recorded test on target PC, no stuck or falling player | NOT RUN |
| One complete training | Choose any program, complete mentoring and assessment, fail/retry, earn grade | Runtime recording/log; correct controls and target aim | NOT RUN |
| Other first-program choices | Repeat first-program selection for all six; never require karate first | Six successful starts; appropriate station and controls | NOT RUN |
| Sparring | After passing, visit northwest ring; win with counters; lose, guard-break, evade, forfeit, timeout, rematch | Health/stamina/timer/cues agree; opponent cannot walk through collision | NOT RUN |
| Written mission | Meet Jordan/contact; try contact decline; inspect records; complete each evidence route and evaluation | Entire playable sequence in packaged game; readable text and reachable choices | NOT RUN |
| District | Enter market/café/records/oversight rooms; navigate the southern entrances; inspect local text | No inaccessible mission point or camera lockup | NOT RUN |
| Save/restart | Save mid-dialogue, mid-assessment, mid-windup and after result; close process; relaunch/load | Same choices, clock, positions, match record; no repeated reward | NOT RUN |
| Recovery | Save twice; back up test save files; corrupt ONLY the newest test checkpoint; load; then test two invalid checkpoints | Older valid save recovered; both invalid preserve current session | NOT RUN |
| Upgrade | Load a schema-1 Written Narrative save, then save/reload schema 2 | Narrative retained; fresh sparring state; safe migrated position | NOT RUN |
| UI | Test 1280×720, 1920×1080, 800×600; 100/120/140% text; keyboard and mouse | No clipped/unreachable responses; typing never selects unintended answer | NOT RUN |
| Performance | Complete loop for at least 20 minutes on stated minimum-target hardware | Frame time/memory/errors recorded; choose performance targets from actual hardware results | NOT RUN |
| PlugVersal | Discover/add the packaged top-level RightOrWrong.exe; launch with a working directory outside the package; return focus after Alt-Tab; operate movement/dialogue; save, close, relaunch/load | Tested PlugVersal version, exact package hash, launch and save evidence | NOT RUN |

Use a disposable test save profile for corruption tests; keep copies of real saves. The generated runtime uses Unreal SaveGame storage. For an unpackaged Windows install, check the actual game's Saved/SaveGames location; do not assume a hard-coded path for every distribution.

The engine tests use memory serialization. They do not prove disk checkpoint rotation, physical collision, input focus, GPU rendering or PlugVersal operation. The package checker validates file structure and presence; it never claims that gameplay was observed.

After every gate passes on the same source/package, the product can be called a **verified Windows demo**. Finished character art, animation, a populated city and the later campaign remain separate milestones.
