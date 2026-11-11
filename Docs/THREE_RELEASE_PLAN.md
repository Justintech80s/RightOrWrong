# RIGHT OR WRONG: Three release plan (design targets, not delivered features)

## Release 1 — Demo: Vanguard Academy
- A small explorable neighborhood; a playable 18-year-old recruit; a memorable commander, investor, coach, rivals and officials.
- Two or three authentic, polished sports as the *demo's high-quality focus*. Other disciplines may have introductory activities.
- Academy attendance, knowledge transfer, a mentor relationship and a graduation milestone.
- Several short open-world missions whose endings vary with the player's choices; dark comedy and cinematic pacing.
- Complete solo save/load loop and accessible controllers, performance pass, and Windows build.
- Acceptance criteria: verified build in Unreal Editor, playable from beginning to end, no blockers or required placeholder HUD prompts.

## Release 2 — Finished single-player game
- Complete six sports, training and tournament ladders, academy graduation, city districts, police and criminal careers.
- Full narrative with character pastiches that parody *archetypes* but have original identities and writing.
- More teaching and mentoring systems; legal/investigative consequences; vehicles, AI, economy, encounters and cinematics.
- Story concludes satisfyingly offline; no always-online requirement for solo content.
- Acceptance criteria: full campaign playtested, key sports validated with subject-matter review, licensed/original assets, performance and accessibility targets met.

## Release 3 — Right or Wrong Online
- New separately versioned network layer, server authority, user accounts, persistence, matchmaking, moderation, abuse prevention and anti-cheat.
- Online sports tournaments, optional cooperative missions, player organizations and teaching/mentorship.
- Expand city/activities only after stable small-group synchronization and server costs are understood.
- Acceptance criteria: security, load, latency, recovery, moderation and matchmaking testing on real hosted servers.

## Architecture rules now
- Use versioned data formats for progression and narrative state.
- Rules live in reusable modules; do not mix networking code into each sports rule.
- Missions refer to stable IDs; document each branch's prerequisites and outcomes.
- Preserve offline story; multiplayer is a separate product milestone, not promised by the demo's local save files.
