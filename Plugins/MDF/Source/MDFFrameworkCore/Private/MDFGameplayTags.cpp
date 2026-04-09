// Kyle Cuss and Cuss Programming 2026

#include "MDFGameplayTags.h"
#include "NativeGameplayTags.h"

namespace MDFGameplayTags
{
	// --------------------
	// Disciplines
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat, "Discipline.Combat");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Warrior, "Discipline.Combat.Warrior");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Archer, "Discipline.Combat.Archer");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Combat_Mage, "Discipline.Combat.Mage");
	
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Gathering, "Discipline.Gathering");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Gathering_Miner, "Discipline.Gathering.Miner");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Gathering_Fisher, "Discipline.Gathering.Fisher");
	
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Crafting, "Discipline.Crafting");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Crafting_Blacksmith, "Discipline.Crafting.Blacksmith");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Crafting_Alchemy, "Discipline.Crafting.Alchemy");
	
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Social, "Discipline.Social");
	UE_DEFINE_GAMEPLAY_TAG(Discipline_Special, "Discipline.Special");
	
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
	// Stats / Resources
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Resource_Health, "Resource.Health");
	UE_DEFINE_GAMEPLAY_TAG(Resource_Mana, "Resource.Mana");
	UE_DEFINE_GAMEPLAY_TAG(Resource_Stamina, "Resource.Stamina");
	
	UE_DEFINE_GAMEPLAY_TAG(Stat_Primary_Strength, "Stat.Primary.Strength");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Primary_Intelligence, "Stat.Primary.Intelligence");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Primary_Dexterity, "Stat.Primary.Dexterity");
	
	UE_DEFINE_GAMEPLAY_TAG(Stat_Secondary_Crit, "Stat.Secondary.Crit");
	UE_DEFINE_GAMEPLAY_TAG(Stat_Secondary_Haste, "Stat.Secondary.Haste");

	// --------------------
	// Combat State
	// --------------------
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_InCombat, "Combat.State.InCombat");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Casting, "Combat.State.Casting");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Channeling, "Combat.State.Channeling");
	UE_DEFINE_GAMEPLAY_TAG(Combat_State_Dead, "Combat.State.Dead");

	UE_DEFINE_GAMEPLAY_TAG(Combat_Target_Hostile, "Combat.Target.Hostile");
	UE_DEFINE_GAMEPLAY_TAG(Combat_Target_Friendly, "Combat.Target.Friendly");

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
}
