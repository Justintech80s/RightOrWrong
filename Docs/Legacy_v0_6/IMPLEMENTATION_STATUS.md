# Right or Wrong v0.4 — Actual status

## Implemented as source (not Unreal-compiled)
- All features in v0.3, plus two interactive *text-only* pedagogy interactions (a class and a coach).
- Knowledge/teaching XP, mentor and seminar choices, different reputation impacts, and an academy graduation eligibility rule.
- Graduation marker checks 90 primary sport XP, 30 pedagogy XP, mentor conversation, and solved first case aftermath.
- Related new state is stored in SaveGame and rendered by HUD; new engine-independent tests cover pedagogy and graduation rules.
- Narrative/character/mission planning documents and demo/final/online release gates.

## Not implemented
- Real sports simulation, animations, authored levels, cinematic missions, law or crime career trees, police AI, vehicles, multiplayer, voiceover, model/texture assets, physics-grade sport equipment, UMG menus.
- True class content beyond a text choice or actual player teaching other NPCs.
- Unreal compilation, Play-In-Editor, build packaging, Windows installer or dedicated servers.

## Specific risks
- Requires Unreal 5.8 locally and empty map configured by hand.
- Number-key choices are context-sensitive; class/mentor choices should be completed before starting another interaction.
- First case still starts at Police Station once the primary training assessment is passed, while graduation requires the whole branch aftermath.
- Graduation is a **prototype milestone**, not the completion of the planned game's Academy act.
