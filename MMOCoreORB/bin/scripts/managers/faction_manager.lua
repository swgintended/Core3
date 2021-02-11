--Factional Relationships determine enemies and allies of factions.
--If you kill a factional NPC, and it is allied with another faction, then you also lose faction with the ally.
--If you kill a factional NPC, and it is enemies with another faction, then you gain faction with the enemy.

-- { faction,playerAllowed,enemies,allies,adjustFactor },
--	faction       - This is the string key faction that the relationship describes.
-- playerAllowed - Player's are able to earn or lose points with this faction.
-- enemies       - This is a comma delimited list of string key factions that this faction is enemies with.
-- allies        - This is a comma delimited list of string key factions that this faction is allies with.
-- adjustFactor  - This factor affects the amount of faction gained/lost when killing a mobile

factionList = {
	{ "rebel", true, "imperial", "", 1.0 },
	{ "imperial", true, "rebel", "", 1.0 },
	{ "aakuans", true, "binayre,fed_dub,liberation_party", "", 1.0 },
	{ "afarathu", true, "corsec", "", 1.0 },
	{ "alkhara", true, "tusken_raider", "", 1.0 },
	{ "bandit", true, "townsperson", "", 1.0 },
	{ "beldonnas_league", true, "followers_of_lord_nyax,lost_aqualish", "corsec", 1.0 },
	{ "binayre", true, "aakuans,corsec,fed_dub", "", 1.0 },
	{ "bloodrazor", true, "canyon_corsair,nym", "", 1.0 },
	{ "borvo", true, "gungan,jabba,trade_federation", "", 1.0 },
	{ "canyon_corsair", true, "bloodrazor,nym", "", 1.0 },
	{ "cobral", true, "restuss", "", 1.0 },
	{ "cor_swoop", true, "smashball", "", 1.0 },
	{ "corsec", true, "afarathu,followers_of_lord_nyax,monumenter,rogue_corsec,lost_aqualish", "beldonnas_league", 1.0 },
	{ "dantari_raiders", true, "kunga_tribe,mokk_tribe,janta_tribe", "", 1.0 },
	{ "desert_demon", true, "swoop", "", 1.0 },
	{ "donkuwah_tribe", true, "gondula_tribe,panshee_tribe", "", 1.0 },
	{ "drall", true, "corsec,fed_dub", "", 1.0 },
	{ "endor_marauder", true, "gondula_tribe,panshee_tribe", "", 1.0 },
	{ "fed_dub", true, "binayre,drall,liberation_party,lost_aqualish", "", 1.0 },
	{ "flail", true, "hidden_daggers", "", 1.0 },
	{ "followers_of_lord_nyax", true, "beldonnas_league,corsec", "", 1.0 },
	{ "fs_villager", true, "sith_shadow,sith_shadow_nonaggro", "", 0.0 },
	{ "garyn", true, "restuss", "", 1.0 },
	{ "gondula_tribe", true, "donkuwah_tribe,korga_tribe,pubam,endor_marauder,gorax", "panshee_tribe", 1.0 },
	{ "gorax", true, "panshee_tribe,gondula_tribe", "", 1.0 },
	{ "gungan", true, "borvo,plasma_thief,swamp_rat", "", 1.0 },
	{ "hidden_daggers", true, "beldonnas_league,corsec,flail", "rogue_corsec", 1.0 },
	{ "hutt", true, "naboo_security_force,narmle,nym,corsec,imperial", "jabba,borvo", 1.0 },
	{ "jabba", true, "borvo,valarian", "hutt", 1.0 },
	{ "janta_tribe", true, "kunga_tribe,dantari_raiders", "", 1.0 },
	{ "jawa", true, "tusken_raider", "", 1.0 },
	{ "kobola", true, "narmle,spice_collective", "", 1.0 },
	{ "korga_tribe", true, "gondula_tribe,panshee_tribe", "", 1.0 },
	{ "kunga_tribe", true, "dantari_raiders,mokk_tribe", "", 1.0 },
	{ "liberation_party", true, "corsec,fed_dub", "", 1.0 },
	{ "lok_mercenaries", true, "bloodrazor,canyon_corsair", "", 1.0 },
	{ "lost_aqualish", true, "beldonnas_league,corsec,fed_dub", "", 1.0 },
	{ "meatlump", true, "beldonnas_league,corsec,rogue_corsec", "", 1.0 },
	{ "mokk_tribe", true, "dantari_raiders,janta_tribe", "", 1.0 },
	{ "monumenter", true, "beldonnas_league,corsec", "", 1.0 },
	{ "mtn_clan", true, "nightsister", "", 1.0 },
	{ "naboo", true, "borvo", "", 1.0 },
	{ "naboo_pirate", true, "naboo_security_force", "", 1.0 },
	{ "naboo_security_force", true, "borvo,naboo_pirate,plasma_thief,swamp_rat,trade_federation", "", 1.0 },
	{ "narmle", true, "kobola,spice_collective", "restuss", 1.0 },
	{ "nightsister", true, "mtn_clan,imperial", "", 1.0 },
	{ "nym", true, "bloodrazor,canyon_corsair", "", 1.0 },
	{ "olag_greck", true, "beldonnas_league,corsec", "", 1.0 },
	{ "panshee_tribe", true, "donkuwah_tribe,korga_tribe,pubam,endor_marauder,gorax", "gondula_tribe", 1.0 },
	{ "pirate", true, "", "", 1.0 },
	{ "plasma_thief", true, "gungan", "", 1.0 },
	{ "pubam", true, "gondula_tribe,panshee_tribe", "", 1.0 },
	{ "restuss", true, "cobral,garyn", "narmle", 1.0 },
	{ "rogue_corsec", true, "corsec", "hidden_daggers", 1.0 },
	{ "rorgungan", true, "spice_collective", "", 1.0 },
	{ "sif", true, "rebel,imperial", "hutt", 1.0 },
	{ "sith_shadow", true, "fs_villager", "rebel,imperial", 0.0 },
	{ "sith_shadow_nonaggro", true, "fs_villager", "rebel,imperial", 0.0 },
	{ "smashball", true, "cor_swoop,corsec", "", 1.0 },
	{ "spice_collective", true, "narmle,kobola,rorgungan", "", 1.0 },
	{ "spider_nightsister", true, "mtn_clan", "", 1.0 },
	{ "swamp_rat", true, "gungan", "", 1.0 },
	{ "swoop", true, "desert_demon", "", 1.0 },
	{ "thug", true, "townsperson", "", 1.0 },
	{ "townsperson", true, "bandit,thug", "rebel,imperial", 1.0 },
	{ "trade_federation", true, "borvo,naboo_security_force", "", 1.0 },
	{ "tusken_raider", true, "alkhara,jawa", "", 1.0 },
	{ "valarian", true, "jabba", "", 1.0 },
}

maxFactionRank = 15
-- Where to determine the amount of faction points to award for missions. Valid
-- options are "min_level" (SWG default), "difficulty_level" (the actual level
-- rolled for the mission), or "difficulty_display" (what the player sees in the
-- mission details).
-- missionRewardType = "min_level"
-- The maximum amount of faction points (before scaling) to award for missions.
-- missionRewardCap = 32
-- When set to true, each rank will award slightly more faction points based on
-- the delegate ratios.
-- missionRewardRankScaling = false
-- When true, displays the player's PvP officer faction ranks as a skill tree.
-- Requires .tre patch with "faction_rank" skill trees ()
-- factionSkillTree = false
-- When enabled, the default skill tree is a oneByFour graph. Override this if
-- using a different graph type, different skill names, or new/different ranks.
-- factionSkillTreeNames = {
-- 	imperial = {
-- 		"", -- recruit
-- 		"", -- private
-- 		"", -- lance_corporal
-- 		"", -- corporal
-- 		"", -- staff_corporal
-- 		"", -- sergeant
-- 		"", -- staff_sergeant
-- 		"", -- master_sergeant
-- 		"", -- warrant_officer_2
-- 		"", -- warrant_officer_1
-- 		"faction_rank_imperial_novice", -- second_lieutenant
-- 		"faction_rank_imperial_rank_01", -- lieutenant
-- 		"faction_rank_imperial_rank_02", -- captain
-- 		"faction_rank_imperial_rank_03", -- major
-- 		"faction_rank_imperial_rank_04", -- lieutenant_colonel
-- 		"faction_rank_imperial_master", -- colonel
-- 		"", -- brigadier_general
-- 		"", -- major_general
-- 		"", -- lieutenant_general
-- 		"", -- general
-- 		"", -- high_general
-- 		"" -- surface_marshal
-- 	},
-- 	rebel = {
-- 		"", -- recruit
-- 		"", -- private
-- 		"", -- lance_corporal
-- 		"", -- corporal
-- 		"", -- staff_corporal
-- 		"", -- sergeant
-- 		"", -- staff_sergeant
-- 		"", -- master_sergeant
-- 		"", -- warrant_officer_2
-- 		"", -- warrant_officer_1
-- 		"faction_rank_rebel_novice", -- second_lieutenant
-- 		"faction_rank_rebel_rank_01", -- lieutenant
-- 		"faction_rank_rebel_rank_02", -- captain
-- 		"faction_rank_rebel_rank_03", -- major
-- 		"faction_rank_rebel_rank_04", -- lieutenant_colonel
-- 		"faction_rank_rebel_master", -- colonel
-- 		"", -- brigadier_general
-- 		"", -- major_general
-- 		"", -- lieutenant_general
-- 		"", -- general
-- 		"", -- high_general
-- 		"" -- surface_marshal
-- 	}
-- }

-- SWGIntended: GCW Revamp
-- maxFactionRank = 21
missionRewardType = "difficulty_display"
missionRewardCap = -1
missionRewardRankScaling = true
factionSkillTree = true
factionSkillTreeNames = {
	imperial = {
		"", -- recruit
		"", -- private
		"", -- lance_corporal
		"", -- corporal
		"", -- staff_corporal
		"", -- sergeant
		"", -- staff_sergeant
		"", -- master_sergeant
		"", -- warrant_officer_2
		"", -- warrant_officer_1
		"faction_rank_imperial_novice", -- second_lieutenant
		"faction_rank_imperial_rank_01", -- lieutenant
		"faction_rank_imperial_rank_02", -- captain
		"faction_rank_imperial_rank_03", -- major
		"faction_rank_imperial_rank_04", -- lieutenant_colonel
		"faction_rank_imperial_rank_05", -- colonel
		"faction_rank_imperial_rank_06", -- brigadier_general
		"faction_rank_imperial_rank_07", -- major_general
		"faction_rank_imperial_rank_08", -- lieutenant_general
		"faction_rank_imperial_rank_09", -- general
		"faction_rank_imperial_rank_10", -- high_general
		"faction_rank_imperial_master" -- surface_marshal
	},
	rebel = {
		"", -- recruit
		"", -- private
		"", -- lance_corporal
		"", -- corporal
		"", -- staff_corporal
		"", -- sergeant
		"", -- staff_sergeant
		"", -- master_sergeant
		"", -- warrant_officer_2
		"", -- warrant_officer_1
		"faction_rank_rebel_novice", -- second_lieutenant
		"faction_rank_rebel_rank_01", -- lieutenant
		"faction_rank_rebel_rank_02", -- captain
		"faction_rank_rebel_rank_03", -- major
		"faction_rank_rebel_rank_04", -- lieutenant_colonel
		"faction_rank_rebel_rank_05", -- colonel
		"faction_rank_rebel_rank_06", -- brigadier_general
		"faction_rank_rebel_rank_07", -- major_general
		"faction_rank_rebel_rank_08", -- lieutenant_general
		"faction_rank_rebel_rank_09", -- general
		"faction_rank_rebel_rank_10", -- high_general
		"faction_rank_rebel_master" -- surface_marshal
	}
}
