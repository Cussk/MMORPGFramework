// Kyle Cuss and Cuss Programming 2026

#include "MDFGameplayTags.h"
#include "NativeGameplayTags.h"

namespace MDFGameplayTags
{
	// --------------------
	// Archetypes
	// -------------------
	UE_DEFINE_GAMEPLAY_TAG(Archetype_Vanguard, "Archetype.Vanguard");
	UE_DEFINE_GAMEPLAY_TAG(Archetype_Striker, "Archetype.Striker");
	UE_DEFINE_GAMEPLAY_TAG(Archetype_Channeler, "Archetype.Channeler");
	UE_DEFINE_GAMEPLAY_TAG(Archetype_Tactician, "Archetype.Tactician");
	
	// --------------------
	// Disciplines
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat, "Discipline.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Guardian, "Discipline.Combat.Guardian");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Warrior, "Discipline.Combat.Warrior");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Archer, "Discipline.Combat.Archer");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Mage, "Discipline.Combat.Mage");
	
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Gathering, "Discipline.Gathering");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Gathering_Mining, "Discipline.Gathering.Mining");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Gathering_Fishing, "Discipline.Gathering.Fishing");
	
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Crafting, "Discipline.Crafting");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Crafting_Blacksmithing, "Discipline.Crafting.Blacksmithing");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Crafting_Alchemy, "Discipline.Crafting.Alchemy");
	
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Social, "Discipline.Social");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Special, "Discipline.Special");
	
	// --------------------
	// Combos
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Basic_Combo_Guardian_One, "Basic.Combo.Guardian.One");
	UE_DEFINE_GAMEPLAY_TAG(Basic_Combo_Guardian_Two, "Basic.Combo.Guardian.Two");
	UE_DEFINE_GAMEPLAY_TAG(Basic_Combo_Guardian_Three, "Basic.Combo.Guardian.Three");
	
	UE_DEFINE_GAMEPLAY_TAG(Basic_Combo_Archer_One, "Basic.Combo.Archer.One");
	UE_DEFINE_GAMEPLAY_TAG(Basic_Combo_Archer_Two, "Basic.Combo.Archer.Two");
	UE_DEFINE_GAMEPLAY_TAG(Basic_Combo_Archer_Three, "Basic.Combo.Archer.Three");
	
	UE_DEFINE_GAMEPLAY_TAG(Transition_Combo_Guardian, "Transition.Combo.Guardian");
	UE_DEFINE_GAMEPLAY_TAG(Transition_Combo_Archer, "Transition.Combo.Archer");
	
	// --------------------
	// Identity
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Identity_Guardian_Block, "Identity.Guardian.Block");
	UE_DEFINE_GAMEPLAY_TAG(Identity_Archer_Zoom, "Identity.Archer.Zoom");
	
	// --------------------
	// Skills
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Skill_Category_Attack, "Skill.Category.Attack");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Category_Heal, "Skill.Category.Heal");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Category_Buff, "Skill.Category.Buff");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Category_Debuff, "Skill.Category.Debuff");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Category_Gather, "Skill.Category.Gather");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Category_Craft, "Skill.Category.Craft");
	
	UE_DEFINE_GAMEPLAY_TAG(Skill_Family_Melee, "Skill.Family.Melee");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Family_Ranged, "Skill.Family.Ranged");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Family_Magic, "Skill.Family.Magic");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Family_Utility, "Skill.Family.Utility");
	
	UE_DEFINE_GAMEPLAY_TAG(Skill_Guardian_ShieldBash, "Skill.Guardian.ShieldBash");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Guardian_Guard, "Skill.Guardian.Guard");
	
	UE_DEFINE_GAMEPLAY_TAG(Skill_Archer_PiercingShot, "Skill.Archer.PiercingShot");
	UE_DEFINE_GAMEPLAY_TAG(Skill_Archer_ArrowRain, "Skill.Archer.ArrowRain");
	
	// --------------------
	// Execution Family Tags
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Execution_SelfTimedState, "Execution.SelfTimedState");
	UE_DEFINE_GAMEPLAY_TAG(Execution_FrontalMeleeTrace, "Execution.FrontalMeleeTrace");
	UE_DEFINE_GAMEPLAY_TAG(Execution_Projectile, "Execution.Projectile");
    UE_DEFINE_GAMEPLAY_TAG(Execution_AreaPersistent, "Execution.AreaPersistent");
	
	// --------------------
	// Equipment / Tools
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Sword, "Equipment.Weapon.Sword");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Bow, "Equipment.Weapon.Bow");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Weapon_Staff, "Equipment.Weapon.Staff");
	
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Tool_Pickaxe, "Equipment.Tool.Pickaxe");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Tool_FishingRod, "Equipment.Tool.FishingRod");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Tool_Hammer, "Equipment.Tool.Hammer");
	
	// --------------------
	// Attributes
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Resource_Health, "Attribute.Resource.Health");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Resource_Mana, "Attribute.Resource.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Resource_Stamina, "Attribute.Resource.Stamina");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Resource_Focus, "Attribute.Resource.Focus");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Strength, "Attribute.Primary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Intelligence, "Attribute.Primary.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Primary_Dexterity, "Attribute.Primary.Dexterity");
	
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Crit, "Attribute.Secondary.Crit");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Defense, "Attribute.Secondary.Defense");
	UE_DEFINE_GAMEPLAY_TAG(Attribute_Secondary_Haste, "Attribute.Secondary.Haste");
	
	// -------------------
	// Effects
	// -------------------
	UE_DEFINE_GAMEPLAY_TAG(Effect_Damage, "Effect.Damage");
	UE_DEFINE_GAMEPLAY_TAG(Effect_Heal, "Effect.Heal");
	
	// -------------------
	// Cues
	// -------------------
	UE_DEFINE_GAMEPLAY_TAG(Cue_Skill_Execute, "Cue.Skill.Execute");
	UE_DEFINE_GAMEPLAY_TAG(Cue_Skill_Projectile_Spawn, "Cue.Skill.Projectile.Spawn");
	UE_DEFINE_GAMEPLAY_TAG(Cue_Skill_Impact, "Cue.Skill.Impact");
	UE_DEFINE_GAMEPLAY_TAG(Cue_Combat_HitReact, "Cue.Combat.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(Cue_Combat_Death, "Cue.Combat.Death");
	
	UE_DEFINE_GAMEPLAY_TAG(Cue_Identity_Start, "Cue.Identity.Start");
	UE_DEFINE_GAMEPLAY_TAG(Cue_Identity_Loop, "Cue.Identity.Loop");
	UE_DEFINE_GAMEPLAY_TAG(Cue_Identity_End, "Cue.Identity.End");

	// --------------------
	// Combat State
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_InCombat, "Combat.State.InCombat");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Casting, "Combat.State.Casting");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Blocking, "Combat.State.Blocking");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Zooming, "Combat.State.Zooming");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Stealthed, "Combat.State.Stealthed");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Channeling, "Combat.State.Channeling");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Dead, "Combat.State.Dead");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Stunned, "Combat.State.Stunned");

	// --------------------
	// Combat Target
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Combat_Target_Hostile, "Combat.Target.Hostile");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Target_Friendly, "Combat.Target.Friendly");
	
	// --------------------
	// Timed State
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Timed_State_Guarding, "Timed.State.Guarding");

	// --------------------
	// Narrative
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Quest_Type_Main, "Quest.Type.Main");
	UE_DEFINE_GAMEPLAY_TAG(Quest_Type_Side, "Quest.Type.Side");
	UE_DEFINE_GAMEPLAY_TAG(Quest_State_Active, "Quest.State.Active");
	UE_DEFINE_GAMEPLAY_TAG(Quest_State_Completed, "Quest.State.Completed");

	UE_DEFINE_GAMEPLAY_TAG(Dialogue_Flag, "Dialogue.Flag");
	UE_DEFINE_GAMEPLAY_TAG(Faction, "Faction");
	UE_DEFINE_GAMEPLAY_TAG(Relationship, "Relationship");

	// --------------------
	// World / Interaction
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Type_Gather, "Interaction.Type.Gather");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Type_Talk, "Interaction.Type.Talk");
	UE_DEFINE_GAMEPLAY_TAG(Interaction_Type_Craft, "Interaction.Type.Craft");

	UE_DEFINE_GAMEPLAY_TAG(World_Region, "World.Region");
	UE_DEFINE_GAMEPLAY_TAG(World_Event, "World.Event");

	// --------------------
	// AI
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(AI_Role_Enemy, "AI.Role.Enemy");
	UE_DEFINE_GAMEPLAY_TAG(AI_Role_Civilian, "AI.Role.Civilian");
	UE_DEFINE_GAMEPLAY_TAG(AI_Role_Boss, "AI.Role.Boss");
	UE_DEFINE_GAMEPLAY_TAG(AI_Role_Companion, "AI.Role.Companion");

	UE_DEFINE_GAMEPLAY_TAG(AI_State_Idle, "AI.State.Idle");
	UE_DEFINE_GAMEPLAY_TAG(AI_State_Combat, "AI.State.Combat");

	UE_DEFINE_GAMEPLAY_TAG(AI_Intent_Attack, "AI.Intent.Attack");
	UE_DEFINE_GAMEPLAY_TAG(AI_Intent_Flee, "AI.Intent.Flee");
	
	// --------------------
	// Input
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Input_Move, "Input.Move");
	UE_DEFINE_GAMEPLAY_TAG(Input_Look, "Input.Look");
	UE_DEFINE_GAMEPLAY_TAG(Input_Jump, "Input.Jump");
	UE_DEFINE_GAMEPLAY_TAG(Input_BasicCombo, "Input.BasicCombo");
	UE_DEFINE_GAMEPLAY_TAG(Input_IdentitySkill, "Input.IdentitySkill");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_One, "Input.SkillSlot.One");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Two, "Input.SkillSlot.Two");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Three, "Input.SkillSlot.Three");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Four, "Input.SkillSlot.Four");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Five, "Input.SkillSlot.Five");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Six, "Input.SkillSlot.Six");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Seven, "Input.SkillSlot.Seven");
	UE_DEFINE_GAMEPLAY_TAG(Input_SkillSlot_Eight, "Input.SkillSlot.Eight");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapArchetype_Vanguard, "Input.SwapArchetype.Vanguard");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapArchetype_Striker, "Input.SwapArchetype.Striker");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapArchetype_Channeler, "Input.SwapArchetype.Channeler");
	UE_DEFINE_GAMEPLAY_TAG(Input_SwapArchetype_Tactician, "Input.SwapArchetype.Tactician");
	UE_DEFINE_GAMEPLAY_TAG(Input_Interact, "Input.Interact");
	UE_DEFINE_GAMEPLAY_TAG(Input_ToggleTarget, "Input.ToggleTarget");
	UE_DEFINE_GAMEPLAY_TAG(Input_CycleTarget, "Input.CycleTarget");
	
	// --------------------
	// Equipment Visuals
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_MainHand, "Equipment.Visual.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_OffHand, "Equipment.Visual.OffHand");
	
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Head, "Equipment.Visual.Head");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Chest, "Equipment.Visual.Chest");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Hands, "Equipment.Visual.Hands");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Legs, "Equipment.Visual.Legs");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Feet, "Equipment.Visual.Feet");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Back, "Equipment.Visual.Back");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Shoulders, "Equipment.Visual.Shoulders");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Visual_Waist, "Equipment.Visual.Waist");
	
	// --------------------
	// Items
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Item_Category_Equipment, "Item.Category.Equipment");
	UE_DEFINE_GAMEPLAY_TAG(Item_Category_Material, "Item.Category.Material");
	UE_DEFINE_GAMEPLAY_TAG(Item_Category_Consumable, "Item.Category.Consumable");
	UE_DEFINE_GAMEPLAY_TAG(Item_Category_Currency, "Item.Category.Currency");

	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Common, "Item.Rarity.Common");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Uncommon, "Item.Rarity.Uncommon");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Rare, "Item.Rarity.Rare");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Epic, "Item.Rarity.Epic");
	UE_DEFINE_GAMEPLAY_TAG(Item_Rarity_Legendary, "Item.Rarity.Legendary");

	// --------------------
	// Equipment Slots
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_MainHand, "Equipment.Slot.MainHand");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_OffHand, "Equipment.Slot.OffHand");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Head, "Equipment.Slot.Head");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Chest, "Equipment.Slot.Chest");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Hands, "Equipment.Slot.Hands");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Legs, "Equipment.Slot.Legs");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Feet, "Equipment.Slot.Feet");
	UE_DEFINE_GAMEPLAY_TAG(Equipment_Slot_Back, "Equipment.Slot.Back");
}
