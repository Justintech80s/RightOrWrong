# Editor test checklist (execute on Windows PC with UE 5.8 installed)

Static checks can run here, but the following tests require the actual Unreal Editor:

1. Open `RightOrWrong.uproject`, regenerate C++ project files if requested, compile `Development Editor`, verify no errors.
2. Create and save an Empty Level at `/Game/Maps/ROW_City`. Configure Editor Startup and Game Default Maps to that level; press PIE.
3. Verify the floor, building blocks, Academy, 6 training markers, police station, dojo, underground club, and 3 follow-up sites are visible.
4. With a new save: choose male (`1`) or female (`2`) and check stored profile on HUD.
5. Speak to Ward (`E`), choose each of the six sports in six clean runs; ensure correct station is identified.
6. At chosen station, start drill (`E`). Check all 3 cue responses, wrong response, and >12-second response; verify 2 errors ends drill.
7. Finish drill: validate +30 primary sport XP, graduation, police mission unlock. Train a second sport to test independent XP.
8. Save `F5`; restart PIE; load `F9`; verify profile, sport, XP, graduation, reputation and location.
9. Follow police assignment, find clue, test initial branches 1,2,3 on separate fresh saves.
10. For each initial branch, travel to the named follow-up marker; verify 1 and 2 outcomes and persistence (six tests total).
11. Check repeated E interactions cannot re-award follow-up reputation after a branch is resolved.
12. Verify map is navigable without collision traps, camera isn't clipped, HUD text fits screen; profile can still move and fight.
13. Check all project logs; enable Development build and package for Windows only *after* PIE success.

Report failures with `Saved/Logs/RightOrWrong.log` and Unreal's build output. Do not consider this source project a verified game executable until these tests pass.
