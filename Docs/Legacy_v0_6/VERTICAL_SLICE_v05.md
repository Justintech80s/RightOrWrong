# Right or Wrong — Demo vertical slice v0.5

## Player experience (target: 15–30 minutes)
1. Customize recruit profile; spawn at academy.
2. Meet General Ward and mentor Imani.
3. Complete timed karate practice using existing graybox station.
4. Choose whether to report or protect a recruit.
5. Travel to gala district; investigate disputed housing documents.
6. Choose command, independent oversight, or fixer.
7. Attend an ethics seminar and confront a compromised professor.
8. Finish with a consequence screen and a teaser for graduation.

## Production reality
Mission pack is **design/data only**. Missions 2 and 3 are not wired into Unreal runtime. Existing v0.4 code is retained unchanged. No animations, playable vehicle, cinematic scene, or real 3D character were added.

## Implementation order
- Import JSON mission definitions into Unreal Data Assets or a validated runtime loader.
- Build a mission director with objective triggers, state transitions and autosave.
- Block out gala and lecture hall.
- Implement dialogue UI, branching aftermath and basic NPC interactions.
- Integrate an animation blueprint and mannequin-compatible skeleton for character movement and karate.
- Add deterministic mission progression tests and perform in-editor playtesting.

## Release gates
- Clean Unreal compile; 15–30 minute playthrough; no soft locks.
- Character movement, training, mission branching and save/load verified.
- Performance and accessibility checks on target PC.
