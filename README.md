# MDF: Multiplayer Discipline Framework

MDF is an Unreal Engine gameplay framework for building MMO-lite RPG combat, progression, equipment, and world systems.

The framework is built around a discipline-based character model where players can swap between combat archetypes, use discipline-specific skill loadouts, transition between combo chains, and carry persistent character progress across temporary shard-style worlds.

This project is currently focused on reusable framework architecture, not a finished game.

## Current Status

MDF currently includes a working Combat V1 and presentation foundation:

- Discipline-based combat framework
- Archetype and discipline swapping
- Basic combo chains
- Transition combos between disciplines
- Identity actions such as Block and Zoom
- Skill cost and cooldown handling
- Skill execution handlers
- Combat effects and timed states
- Combat cue presentation for montage, VFX, audio, and impact visuals
- Native animation instance foundation
- Discipline-specific linked animation layers
- Archer aim offset support
- Persistent weapon visual components
- Sheathed and unsheathed equipment socket swapping
- Equip and unequip attach notify support
- Modular armor visual swapping
- Discipline swap concealment VFX
- Client-side visual refresh on replicated discipline changes
- Core definition registry subsystem
- Early item definition and equipment foundation in progress

## Design Goals

MDF is designed around a few core principles:

- Data-driven authoring through definition assets and gameplay tags
- Clear separation between gameplay state, presentation, debug, and authored data
- Multiplayer-safe ownership boundaries
- PlayerState-owned persistent state where appropriate
- Character and Controller classes kept thin
- Components for actor-owned state and presentation
- Subsystems for global lookup, registry, and service-style ownership
- Event-driven logic over ticking where possible
- Shallow inheritance, composition-first architecture
- Practical, production-shaped systems without unnecessary abstraction

## High-Level Concept

MDF is intended for MMO-lite RPGs where a player owns a persistent character that can enter temporary shard-based worlds.

A future game built on this framework could support:

- Persistent character progression
- Per-discipline equipment loadouts
- Temporary worlds with 20 to 30 players
- World modifiers and difficulty spikes
- Rogue-lite world runs
- Loot, crafting, gathering, and meta progression
- Combat archetype swapping and class-style gameplay identity

The current focus is the foundation needed to support that long-term structure.

## Combat Overview

Combat is built around disciplines and archetypes.

An archetype represents a broad role or combat category. A discipline represents the specific equipped combat identity within that archetype.

Example archetypes:

- Vanguard
- Striker
- Channeler
- Tactician

Example disciplines:

- Guardian
- Archer

Each discipline can define its own:

- Skill loadout
- Basic combo chain
- Identity action
- Transition combo behavior
- Animation set
- Visual set
- Equipment presentation
- Armor presentation
- Swap concealment VFX

## Implemented Combat Features

### Skill Activation

Skills are authored through definition assets and executed through combat handlers. Skill activation supports:

- Validation
- Costs
- Cooldowns
- Execution handlers
- Targeting modes
- Effects
- Timed states
- Cues and presentation

### Basic Combos

Disciplines can define basic combo chains with authored timing windows.

Combo state is owned by the combat action system rather than by the Character class.

### Transition Combos

Players can swap disciplines during specific combo windows to trigger transition skills. These transition combos can bridge into the destination discipline’s combo chain.

This allows combat flow such as:

```text
Guardian combo step 2
-> swap to Archer during transition window
-> play Guardian-to-Archer transition skill
-> continue into Archer combo flow
```

### Identity Actions

Identity actions are discipline-specific held actions.

Current examples:

- Guardian Block
- Archer Zoom

Identity behavior supports overlay-style actions, allowing some identities to permit attacks while active and others to block attacks.

For example:

- Zoom allows attacks
- Block prevents attacks

## Animation Foundation

MDF uses a native animation state class with minimal AnimBP logic.

The recommended setup is:

```text
Base AnimBP
-> universal locomotion
-> linked discipline combat locomotion layer
-> full-body action slot
-> hit/death slots later
```

The base AnimBP remains a thin pose graph. Gameplay state is calculated in C++.

Current animation features include:

- Native `UMDFCharacterAnimInstance`
- Native discipline animation layer base
- Discipline-specific linked layers
- Base locomotion when sheathed or out of combat
- Discipline combat locomotion while in combat
- Archer aim offset
- Zoom-aware aim offset strength
- Root-motion-friendly action flow

## Equipment and Presentation

The presentation layer currently supports starter/default discipline visuals.

A discipline can provide a visual set containing:

- Weapon visuals
- Shield or offhand visuals
- Sheathed and unsheathed attachment points
- Modular armor visuals
- Swap concealment VFX

Weapon visuals are persistent components. They are not destroyed and respawned when entering or leaving combat. Instead, they reattach between sheathed and unsheathed sockets.

Example:

```text
Out of combat:
    sword attached to hip or back socket

Enter combat:
    sword reattached to hand socket

Leave combat:
    sword reattached to sheathed socket
```

Modular armor uses skeletal mesh components that follow the base character mesh through leader pose.

Supported modular armor slots include:

- Head
- Chest
- Hands
- Legs
- Feet
- Back

## Current Module Layout

The framework is organized into plugin modules.

### MDFFrameworkCore

Shared foundation code.

Includes:

- Base definition asset
- Native gameplay tags
- Core definition registry subsystem
- Shared helpers and core types

### MDFFrameworkProgression

Progression and authored RPG data.

Currently includes:

- Discipline definitions
- Skill definitions
- Animation sets
- Visual sets
- Equipment visual types
- Early item definition work

Planned expansion:

- Inventory
- Equipment loadouts
- Loot tables
- Crafting recipes
- Life skills
- Rewards
- Meta progression

### MDFFrameworkCombat

Combat runtime systems.

Includes:

- Player skill component
- Combat action component
- Combatant component
- Skill execution handlers
- Cooldowns
- Costs
- Effects
- Timed states
- Identity actions
- Combo logic
- Transition combo logic

### MDFFrameworkQuickstart

Reference gameplay integration.

Includes:

- Quickstart character
- Quickstart controller
- Quickstart PlayerState
- Animation presentation component
- Equipment presentation component
- Identity presentation component
- Anim notifies
- Example setup assets

This module is intended as a working integration example, not the only way to use the framework.

### MDFFrameworkDebug

Debug and runtime inspection tools.

Includes:

- Debug HUD
- Debug world subsystem
- Debug snapshots
- Runtime combat state display

Debug logic is kept separate from core gameplay runtime where possible.

### MDFFrameworkEditor

Editor-side utilities and validation.

Currently minimal, planned to grow as authoring workflows expand.

## Definition Registry

MDF includes a lightweight Core definition registry subsystem.

The registry is intended for authored definition lookup, not gameplay ownership.

It supports explicit registration and lookup of definitions.

The framework avoids hidden content scanning for now. Definitions are expected to be registered through project settings, developer settings, or feature-specific registration systems.

## Planned Phase 14: Inventory and Equipment

The next major implementation area is Inventory and Equipment Loadouts.

Planned structure:

```text
UMDFItemDefinition
UMDFEquipmentItemDefinition
UMDFItemDefinitionSubsystem
UMDFInventoryComponent
UMDFEquipmentLoadoutComponent
```

The current discipline visual sets will become fallback starter visuals.

Final equipment flow should become:

```text
Active Discipline
+ player equipment loadout for that discipline
+ fallback discipline visual set
-> resolved visual presentation
```

This allows each discipline to maintain its own current equipment loadout.

## Future Systems

Planned major systems include:

### Inventory and Equipment

- Item definitions
- Item stacks
- Equipment item definitions
- Per-discipline equipment loadouts
- Equipped visuals
- Equipment stat support later

### Shard and World Instances

- Temporary world instances
- 20 to 30 player shard targets
- World definitions
- World seeds
- Difficulty tiers
- World modifiers
- Expiration and reset behavior

### Rewards and Loot

- Reward bundles
- Loot tables
- Enemy rewards
- Objective rewards
- World completion rewards

### Objectives and Contracts

- World objectives
- Personal objectives
- Contracts
- Event goals
- Daily and weekly style tasks later

### Life Skills

- Gathering nodes
- Crafting recipes
- Refining
- Crafting stations
- Life skill progression
- World modifier influence

### Stats and Combat V2

Combat V1 is intentionally focused on action flow and framework structure.

Combat V2 is expected to add:

- Full stats and attributes
- Equipment stat bonuses
- Skill scaling
- Enemy scaling
- Archetype synergies
- More advanced discipline interactions

## Networking Notes

MDF is being built with multiplayer ownership boundaries in mind.

Current direction:

- Player-owned persistent combat/progression state lives on PlayerState components
- Character owns pawn-local presentation and runtime visuals
- GameState should own world-instance runtime state
- Subsystems should own global lookup and service-style behavior
- Client presentation refreshes from replicated active discipline state
- Runtime presentation avoids unnecessary ticking

The framework is intended to support listen and dedicated server use, with future backend integration for shard/world persistence.

## Shard-Based MMO-lite Direction

MDF is being shaped around an MMO-lite structure rather than a traditional always-on MMO world.

The intended model is:

```text
Persistent character
-> enters temporary world shard
-> plays with up to 20 to 30 players
-> completes objectives, gathers resources, earns rewards
-> exits or world expires
-> character persists
```

This enables rogue-lite and meta progression elements while avoiding the cost and complexity of a fully persistent world simulation.

## Development Philosophy

MDF favors practical architecture over excessive abstraction.

Preferred patterns:

- Use subsystems for global registry, lookup, and service-style ownership
- Use components for actor, PlayerState, GameState, pawn, and presentation-owned state
- Keep Character, Controller, and PlayerState classes thin
- Avoid needless wrapper functions
- Avoid deep inheritance
- Avoid ticking unless it is clearly justified
- Keep debug code out of core runtime systems
- Build systems in complete vertical slices before expanding breadth

## Current Roadmap

```text
Combat V1
    Complete

Animation and presentation foundation
    Complete

Weapon and modular armor presentation
    Complete

Swap concealment VFX
    Complete

Phase 14
    Inventory and equipment loadouts

Phase 15
    Shard/world instance runtime

Phase 16
    Rewards and loot

Phase 17
    Objectives and contracts

Phase 18
    Gathering

Phase 19
    Crafting

Phase 20
    Stats and Combat V2
```

## Installation

This project is structured as an Unreal Engine plugin.

To use it in a project:

1. Copy the MDF plugin into your Unreal project’s `Plugins` folder.
2. Enable the MDF plugin modules in Unreal.
3. Regenerate project files.
4. Build the project.
5. Use the Quickstart classes or integrate the runtime components into your own PlayerState, Character, Controller, and GameState setup.

## Requirements

- Unreal Engine 5
- C++ project support
- Gameplay Tags enabled
- Niagara enabled for presentation VFX
- Enhanced Input recommended for quickstart player controls

## License

License to be determined.

Copyright Kyle Cuss and Cuss Programming 2026.
