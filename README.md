# MacroPath

A Geode mod for Geometry Dash (2.2081) that records and replays frame-perfect
input macros, with a pathfinding auto-solver planned as a feature.

Forked from [MacroBot](https://github.com/malikwill/some-vibecoded-bot) by
malikwill (MIT licensed, see LICENSE).

## Using it

Pause a level and tap the round button in the bottom-left corner.

- **Record** enters practice mode and captures your inputs. Checkpoint
  respawns roll the recording back correctly.
- **Save** writes the finished run to disk (only enabled once a run is on
  standby). **Auto-save on completion** does this for you the moment you beat
  the level.
- **Play** replays the loaded macro from the start of the attempt.
  **Block inputs while playing** ignores your own clicks during playback.
- **Load** opens the macro manager: tap a name to load and play it, **Edit** to
  rename, **Del** to delete, **Open Folder** to import/export, **Refresh** to
  rescan after dropping files in.
- **Debug HUD** shows the live frame and event counters.

Macros are `.mbf` files in the mod's save folder, under `macros/`. To import a
macro, copy its `.mbf` into that folder (Load -> Open Folder) and hit Refresh.
To export, copy it out.

## Building

Push to GitHub and the included Actions workflow builds the Windows `.geode`
and uploads it as a workflow artifact. The `geode` version in `mod.json` must
match (or be older than) the Geode you have installed.

## Roadmap

- Pathfind button: a search-based solver that steps physics faster than real
  time, rewinds on death, and saves the winning inputs as a macro.
- File-picker import/export, randomness option, per-macro metadata.
