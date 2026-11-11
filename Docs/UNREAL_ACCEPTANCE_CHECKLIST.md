# Unreal acceptance checklist — not executed here

The standalone C++ tests cover state and scoring rules. These checks require the actual Unreal runtime. Record the engine version, Windows/compiler version, source archive hash and logs alongside the result.

## Build and asset generation

- Build Development Editor without compile/UHT errors. Also inspect a build with unity/PCH assistance disabled for missing dependencies.
- Run setup_demo.py and confirm ROW_City.umap and M_ROWColor.uasset exist. Inspect the map's player start and game mode.
- Confirm a second setup run preserves existing assets and does not duplicate actors.
- Run RightOrWrong.Narrative.SaveGameRoundTrip in the engine; verify a nonempty success report.

## Written interface

- At 1280×720, 1920×1080 and 800×600, read every dialogue page and reach every response by scrolling.
- Test 100%, 120% and 140% dialogue text. Confirm no clipped choices.
- Enter a name, select either presentation and each uniform color; ensure the generated material changes the character.
- Test mouse buttons and number-key responses. Name typing must not also select a dialogue response.
- Enter must never automatically select a consequential option on a multi-option page.
- Leave a conversation open for several minutes; text and choices must remain visible.
- Test Esc, journal, F5 and F9 from both gameplay and a pending decision; return to the original conversation after closing the menu.
- Confirm mouse look/movement stop during modal text and return after it closes. Check for stuck block, aim or bow-draw input after menus.

## Movement and sports

- Begin six clean runs, choosing a different starting sport each time. None may force karate first.
- Find Ward, Imani, the chosen station, Jordan and the visiting contact with the waypoint and minimap.
- For each sport, test correct range, incorrect range, facing/aim errors, correct actions, wrong actions, timing expiry and a successful retry.
- Judo: establish grip, move laterally, complete throw. Taekwondo: kicking range and lateral evade. Fencing: touch and parry.
- Archery: short, correct and excessive draw times plus center/edge/missed shots. Shooting: unsteady aim, correct steady hold and target accuracy.
- Test target collision, target reaction/reset and camera aim against the actual rendered center.
- Leave controls untouched until all six deadlines expire; the assessment must finish with a retry result.
- Pause during a round and during a held input; no timer should advance while the panel is open.
- Check perimeter walls, ground, camera collision, jumps and loading a saved location.

## Narrative and persistence

- Cover both Jordan choices, both contact responses, all three evidence destinations and both follow-up outcomes for each destination.
- Confirm Judo/Archery/Sport Shooting initially introduce the law contact and Karate/Taekwondo/Fencing initially introduce Nico.
- Compare weak and strong passing scores for crime-facing programs; initial contact quality must follow the placement score.
- Decline a contact, take Ward's alternative and complete any evidence branch. No permanent morality/career lock.
- Save and reload before and after each decision, while a dialogue is open, during an assessment and while travelling to the records site.
- Reload a completed branch and revisit it; rewards must not be granted twice.
- After demo completion, practice another sport; original sport, original grade and story decisions must remain intact.
- Confirm an unreadable/incompatible save produces a visible message and preserves the active session.
- New recruit must ask for confirmation and must not overwrite the saved slot until Save is used.

## Packaged executable

- Package Win64 Development, then launch RightOrWrong.exe separately from the editor.
- Verify the startup map, UI fonts, generated material and engine primitive meshes are included in cooked content.
- Complete the whole introduction and records assignment in the package; restart and load saved progress.
- Measure frame time/memory on identified target hardware; inspect runtime logs for errors.
- Only after these checks pass, label the result a verified Windows demo. An executable or successful package alone does not establish gameplay quality.

## Eight-stage edition additions

- Run RightOrWrong.Combat.SaveGameRoundTrip as well as the narrative test. Both are required by the build pipeline.
- Pass the first assessment in each chosen program and confirm that supervised sparring unlocks without changing that program.
- Check incoming-attack text/color, block direction, stamina exhaustion, counter windows, missed attacks, time decisions and once-only match records.
- Pause or open journal during a windup; opponent movement and match timer stop. Resume without an immediate unseen barrage.
- Save during a windup and after a result; relaunch, restore and confirm correct state and no duplicate win/loss.
- Inspect the player's primitive strike/guard pose and opponent's lead-glove pose. They are visual timing aids, not finished animation.
- Navigate all 20 destinations from the common layout. Actual capsules, engine camera and mesh collision must agree with the non-engine clearance test.
- Enter the market, café, records and oversight rooms through their south-facing doorways; confirm mission markers can be reached from inside.
- Test both rotating save files, read-back verification, schema-1 migration and invalid-newest-save fallback.
- Verify directional/sky lighting and indoor readability after runtime atmosphere generation; tune exposure based on the real rendered result.
- Execute the final Windows/PlugVersal release gate in WINDOWS_DEMO_ACCEPTANCE.md.
