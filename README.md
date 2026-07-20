# Combat System Demo

A third-person action-adventure combat prototype built in Unreal Engine 5, using the Gameplay Ability System (GAS).

## Engine & Setup

- **Engine:** Unreal Engine 5.5.4
- **Build:** Open `CombatSystem.uproject`, generate/compile, press Play.

## Controls

| Input | Action |
|---|---|
| WASD | Move |
| Mouse | Look |
| Space | Jump |
| Tab | Toggle Stance (Melee ↔ Ranged) |
| Left Mouse Button | Attack (melee combo) / Fire (ranged, requires aiming) |
| Right Mouse Button (hold) | Aim (Ranged stance only) |
| R | Reload (Ranged stance only) |
| Q | Special Ability : Dash Attack (Melee stance only) |

## Features

**Stance System** - Player switches between Melee and Ranged stances, each with its own move set and weapon (sword / gun), visually swapped on the character and in the HUD.

**Melee Combat** -  3-hit combo attack with unique animations per stage. Rapid input cancels into the next combo stage immediately rather than waiting. The 3rd hit is a critical strike dealing 2x damage, with on-screen combat text. Attacks aim toward the camera direction.

**Ranged Combat** -  Aim-down-sights style aiming with camera zoom, 30-round magazine, dedicated reload, and crosshair-accurate line-trace shooting.

**Special Ability : Dash Attack** -  An instant, teleport-style forward dash that damages anything in its path, with wind-up and follow-through animations and a cooldown.

**Target Dummy (Enemy AI)** -  Takes damage, dies, and respawns at a random nearby location. Automatically counters with an AOE sweep attack every 5 seconds.

**Player Death & Respawn**  Player death plays a death animation and respawns at the spawn point with full health after a short delay.

**Combat HUD** — Live-updating Player Health, Target Health, Ammo count, combo counter, and special ability status (Active / Disabled / On Cooldown), plus a critical hit flash callout.

**Animation & Audio** — Full animation coverage for melee combo, dash, aim/shoot/reload, hit reactions, and death, layered so lower-body movement continues during upper-body weapon animations. Sound effects for attacks (hit/miss), shooting, reloading, dashing, stance switching, hits taken, and deaths.

**Core Framework** — GameMode, GameState, PlayerState, PlayerController, and GameInstance
