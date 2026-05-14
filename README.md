# Multiplayer Discipline Framework

Multiplayer Discipline Framework, or MDF, is an Unreal Engine multiplayer RPG framework focused on discipline-based combat, progression, and future MMO-style gameplay foundations.

The current development focus is **Combat + Disciplines v1**. LifeSkills, quests, narrative systems, economy systems, and broader MMO/shard architecture are intentionally deferred until the combat foundation reaches a stronger functional stopping point.

## Project Goals

MDF is being built as a reusable framework for multiplayer RPG-style games that need:

- Server-authoritative gameplay
- Discipline/class-style progression
- Combat loadouts and skill decks
- Shallow, composition-first architecture
- Data-authored gameplay definitions
- Clear runtime state ownership
- Debug-first visibility
- Optional quickstart/reference classes

The framework is designed to avoid deep inheritance trees and game-specific hard coupling. Core systems should remain modular, inspectable, and usable without requiring a sample game layer.

## Current Focus

The current working area is **Combat + Disciplines v1**.

Active development includes:

- Archetype input
- Discipline swapping
- Identity overlays
- Transition swaps
- Transitional combo skills
- Basic attacks
- Combat action timing
- Targeting and skill execution flow

The current rule is:

> Input expresses intent. Gameplay systems resolve, validate, and execute.

Input should think in terms of archetype slots, not concrete discipline classes. For example, `Q`, `E`, or other swap inputs should request an archetype such as Vanguard or Striker. Runtime systems then resolve the currently equipped discipline for that archetype slot.

## Combat Deck Model

MDF combat is built around a discipline deck model instead of a single fixed class bar.

The intended combat deck contains four archetype lanes:

| Archetype | Role |
|---|---|
| Vanguard | Tank, protector, guard-oriented frontliner |
| Striker | Direct damage and offensive pressure |
| Channeler | Healing, magic, casting, and resource flow |
| Tactician | Utility, control, setup, buffs, and debuffs |

Each archetype lane has one currently equipped discipline. Each discipline owns its own skill loadout, cooldown state, and future equipment/visual loadout.

The goal is to make discipline swapping part of combat flow, not just pre-combat setup.

## Current Combat Features

The current combat slice includes:

- Player-owned skill runtime on PlayerState
- Learned skill runtime
- Equipped discipline slot runtime
- Active discipline runtime
- Discipline-owned loadout save and restore
- Runtime blocking for wrong-discipline skill equips
- Skill activation requests
- Skill validation
- Per-discipline skill cooldowns
- Cooldown persistence across swaps
- Resource cost validation and spending
- Attribute component support
- Targeting context
- Locked target runtime
- Direct damage and healing effects
- Combat state tags
- Dead-state handling
- Combat cue playback
- Projectile, melee trace, persistent area, and self-timed-state execution families
- Debug HUD and debug subsystem visibility

## Locked Combat Direction

The current long-term combat direction includes:

- Discipline-specific basic combo chains
- Right-click identity actions
- Authored startup, execute, and recovery timing
- A dedicated combat action runtime layer
- Normal discipline swaps
- Transition combo swaps
- Authored swap commit timing
- Equipment and visual swap concealment
- Entry bonuses when transitioning into a new archetype or discipline

Animation notifies may support presentation, but authoritative gameplay timing should come from authored combat data.

## Architecture Principles

MDF follows a strict architecture-first approach.

Core rules:

- Prefer components, subsystems, structs, and plain C++ helpers over deep inheritance.
- Keep PlayerController, PlayerState, Character, GameMode, and GameState thin.
- Use PlayerState for long-lived player-owned runtime state.
- Use Character or Pawn for embodiment-facing state.
- Use subsystems for orchestration, registries, services, and debug aggregation.
- Keep authored definitions separate from runtime mutable state.
- Keep UI presentation-only.
- Avoid hidden magic initialization.
- Avoid stringly typed systems when gameplay tags or typed IDs are better.
- Build one major system deeply before spreading into the next.

MDF should trend toward:

- Explicit ownership
- Shallow inheritance
- Composition-first design
- Subsystem/component-first architecture
- Readable data
- Inspectable runtime state
- Optional quickstart layers
- Server-authoritative multiplayer behavior

## Networking Model

MDF currently assumes Unreal replication first.

The default networking stance is:

- Server authoritative by default
- Clients send requests
- Server validates requests
- Server mutates authoritative state
- Replication informs clients
- UI and debug react to replicated state

Runtime state should replicate. Authored definition payloads should not.

Prefer replicating:

- Gameplay tags
- Stable IDs
- Compact runtime structs
- Progression values
- Equipped/loadout state
- Cooldown state
- Mutable combat state

Avoid replicating:

- Large Data Asset payloads
- Static skill definitions
- Static discipline definitions
- Display strings that can be resolved locally

## Ownership Model

### PlayerController

Use for:

- Input
- Local request entry
- HUD coordination
- Optional debug command routing

Do not use PlayerController as the owner of persistent gameplay state.

### PlayerState

Use for:

- Long-lived player runtime state
- Progression
- Learned skills
- Equipped skills
- Active discipline state
- Saved discipline loadouts
- Cooldowns
- Attributes and resources

### Character / Pawn

Use for:

- Embodiment
- Movement
- Animation-facing state
- Combat presentation
- Targeting embodiment context
- Cue playback
- Future equipment visuals

### GameState

Use for:

- Shared replicated world/session state
- Encounter summaries
- Match or shard-level state where appropriate

### Subsystems

Use for:

- Orchestration
- Registries
- Debug aggregation
- Non-owning services
- World-scoped coordination

Subsystems should not automatically become the home of replicated truth.

## Module Layout

MDF uses the `MDFFramework...` module naming pattern.

Current modules include:

```text
MDFFrameworkCore
MDFFrameworkEntity
MDFFrameworkProgression
MDFFrameworkCombat
MDFFrameworkDebug
MDFFrameworkQuickstart
MDFFrameworkEditor
