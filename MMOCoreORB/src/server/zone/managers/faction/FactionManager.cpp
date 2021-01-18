/*
 * FactionManager.cpp
 *
 *  Created on: Mar 17, 2011
 *      Author: crush
 */

#include "FactionManager.h"
#include "FactionMap.h"
#include "server/zone/managers/skill/SkillManager.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "templates/faction/Factions.h"
#include "templates/manager/TemplateManager.h"

FactionManager::FactionManager() {
	setLoggingName("FactionManager");
	setGlobalLogging(false);
	setLogging(false);
}

void FactionManager::loadData() {
	loadFactionRanks();
	loadLuaConfig();
}

void FactionManager::loadFactionRanks() {
	IffStream* iffStream = TemplateManager::instance()->openIffFile("datatables/faction/rank.iff");

	if (iffStream == nullptr) {
		warning("Faction ranks could not be found.");
		return;
	}

	DataTableIff dtiff;
	dtiff.readObject(iffStream);

	factionRanks.readObject(&dtiff);

	delete iffStream;

	info("loaded " + String::valueOf(factionRanks.getCount()) + " ranks", true);
}

void FactionManager::loadLuaConfig() {
	info("Loading config file.", true);

	FactionMap* fMap = getFactionMap();

	Lua* lua = new Lua();
	lua->init();

	//Load the faction manager lua file.
	lua->runFile("scripts/managers/faction_manager.lua");

	LuaObject luaObject = lua->getGlobalObject("factionList");

	if (luaObject.isValidTable()) {
		for (int i = 1; i <= luaObject.getTableSize(); ++i) {
			LuaObject factionData = luaObject.getObjectAt(i);

			if (factionData.isValidTable()) {
				String factionName = factionData.getStringAt(1);
				bool playerAllowed = factionData.getBooleanAt(2);
				String enemies = factionData.getStringAt(3);
				String allies = factionData.getStringAt(4);
				float adjustFactor = factionData.getFloatAt(5);

				Faction faction(factionName);
				faction.setAdjustFactor(adjustFactor);
				faction.setPlayerAllowed(playerAllowed);
				faction.parseEnemiesFromList(enemies);
				faction.parseAlliesFromList(allies);

				fMap->addFaction(faction);
			}

			factionData.pop();
		}
	}

	luaObject.pop();

	maxFactionRank = lua->getGlobalInt("maxFactionRank");
	factionSkillTree = lua->getGlobalBoolean("factionSkillTree");
	if (factionSkillTree) {
		LuaObject skillTreeNames = lua->getGlobalObject("factionSkillTreeNames");
		LuaObject imperial = skillTreeNames.getObjectField("imperial");
		populateFactionSkillTree("imperial", imperial);
		imperial.pop();
		LuaObject rebel = skillTreeNames.getObjectField("rebel");
		populateFactionSkillTree("rebel", rebel);
		rebel.pop();
		skillTreeNames.pop();
	}

	delete lua;
	lua = nullptr;
}

void FactionManager::populateFactionSkillTree(String faction, LuaObject object) {
	if (!object.isValidTable()) {
		warning("factionSkillTreeNames." + faction + " is not a valid lua table");
		return;
	}

	Vector<String>* skillTreeNames = new Vector<String>();
	for (int i = 1; i <= object.getTableSize(); i++) {
		String skillName = object.getStringAt(i);
		skillTreeNames->add(skillName);
	}

	if (skillTreeNames->size() < getHighestRank() + 1) {
		warning("factionSkillTreeNames." + faction + "does not have at least " + String::valueOf(getHighestRank() + 1) + " rank skill names");
	}

	factionSkillTreeNames.put(faction, skillTreeNames);
}

FactionMap* FactionManager::getFactionMap() {
	return &factionMap;
}

void FactionManager::awardFactionStanding(CreatureObject* player, const String& factionName, int level) {
	if (player == nullptr)
		return;

	ManagedReference<PlayerObject*> ghost = player->getPlayerObject();

	if (!factionMap.contains(factionName))
		return;

	const Faction& faction = factionMap.get(factionName);
	const SortedVector<String>* enemies = faction.getEnemies();
	const SortedVector<String>* allies = faction.getAllies();

	if (!faction.isPlayerAllowed())
		return;

	float gain = level * faction.getAdjustFactor();
	float lose = gain * 2;

	ghost->decreaseFactionStanding(factionName, lose);

	//Lose faction standing to allies of the creature.
	for (int i = 0; i < allies->size(); ++i) {
		const String& ally = allies->get(i);

		if ((ally == "rebel" || ally == "imperial")) {
			continue;
		}

		if (!factionMap.contains(ally))
			continue;

		const Faction& allyFaction = factionMap.get(ally);

		if (!allyFaction.isPlayerAllowed())
			continue;

		ghost->decreaseFactionStanding(ally, lose);
	}

	bool gcw = false;
	if (factionName == "rebel" || factionName == "imperial") {
		gcw = true;
	}

	//Gain faction standing to enemies of the creature.
	for (int i = 0; i < enemies->size(); ++i) {
		const String& enemy = enemies->get(i);

		if ((enemy == "rebel" || enemy == "imperial") && !gcw) {
			continue;
		}

		if (!factionMap.contains(enemy))
			continue;

		const Faction& enemyFaction = factionMap.get(enemy);

		if (!enemyFaction.isPlayerAllowed())
			continue;

		ghost->increaseFactionStanding(enemy, gain);
	}
}


void FactionManager::awardPvpFactionPoints(TangibleObject* killer, CreatureObject* destructedObject) {
	if (killer->isPlayerCreature() && destructedObject->isPlayerCreature()) {
		CreatureObject* killerCreature = cast<CreatureObject*>(killer);
		ManagedReference<PlayerObject*> ghost = killerCreature->getPlayerObject();

		ManagedReference<PlayerObject*> killedGhost = destructedObject->getPlayerObject();

		if (killer->isRebel() && destructedObject->isImperial()) {
			ghost->increaseFactionStanding("rebel", 30);
			ghost->decreaseFactionStanding("imperial", 45);

			killedGhost->decreaseFactionStanding("imperial", 45);
		} else if (killer->isImperial() && destructedObject->isRebel()) {
			ghost->increaseFactionStanding("imperial", 30);
			ghost->decreaseFactionStanding("rebel", 45);

			killedGhost->decreaseFactionStanding("rebel", 45);
		}
	}
}

String FactionManager::getRankName(int idx) {
	if (idx >= factionRanks.getCount())
		return "";

	return factionRanks.getRank(idx).getName();
}

int FactionManager::getRankCost(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return factionRanks.getRank(rank).getCost();
}

int FactionManager::getRankDelegateRatioFrom(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return factionRanks.getRank(rank).getDelegateRatioFrom();
}

int FactionManager::getRankDelegateRatioTo(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return factionRanks.getRank(rank).getDelegateRatioTo();
}

String FactionManager::getRankSkillName(const String& faction, int rank) const {
	if (!factionSkillTreeNames.contains(faction)) {
		return "";
	}

	const Vector<String>* skillTreeNames = factionSkillTreeNames.get(faction);
	if (rank >= skillTreeNames->size()) {
		return "";
	}

	return skillTreeNames->get(rank);
}

int FactionManager::getFactionPointsCap(int rank) {
	if (rank >= factionRanks.getCount())
		return -1;

	return Math::max(1000, getRankCost(rank) * 20);
}

bool FactionManager::isFaction(const String& faction) {
	if (factionMap.contains(faction))
		return true;

	return false;
}

bool FactionManager::isEnemy(const String& faction1, const String& faction2) {

	if (!factionMap.contains(faction1) || !factionMap.contains(faction2))
		return false;

	Faction* faction = factionMap.getFaction(faction1);

	return faction->getEnemies()->contains(faction2);
}

bool FactionManager::isAlly(const String& faction1, const String& faction2) {

	if (!factionMap.contains(faction1) || !factionMap.contains(faction2))
		return false;

	Faction* faction = factionMap.getFaction(faction1);

	return faction->getAllies()->contains(faction2);
}

void FactionManager::onPlayerLoggedIn(CreatureObject* player) const {
	PlayerObject* ghost = player->getPlayerObject();
	if (ghost == nullptr) {
		return;
	}

	if (isFactionSkillTreeEnabled()) {
		// Synchronize faction and faction XP for upgrade
		int expectedImperialXp = floor(ghost->getFactionStanding("imperial"));
		int currentImperialXp = ghost->getExperience("faction_imperial");
		int expectedRebelXp = floor(ghost->getFactionStanding("rebel"));
		int currentRebelXp = ghost->getExperience("faction_rebel");
		if (expectedImperialXp != currentImperialXp) {
			ghost->addExperience("faction_imperial", expectedImperialXp - currentImperialXp);
		}

		if (expectedRebelXp != currentRebelXp) {
			ghost->addExperience("faction_rebel", expectedRebelXp - currentRebelXp);
		}
	} else {
		// If disabling, drop faction XP. This is a no-op if the player does not
		// have any of these XP types.
		ghost->removeExperience("faction_imperial");
		ghost->removeExperience("faction_rebel");
	}

	updatePlayerFactionSkills(player);
}

void FactionManager::updatePlayerFactionSkills(CreatureObject* player, bool notifyClient) const {
	byte playerRank = player->getFactionRank();
	bool isImperial = false;
	bool isRebel = false;
	switch (player->getFaction()) {
		case Factions::FACTIONIMPERIAL:
			isImperial = true;
			break;
		case Factions::FACTIONREBEL:
			isRebel = true;
			break;
	}

	for (int rank = factionRanks.getCount() - 1; rank >= 0; rank--) {
		String imperialSkill = getRankSkillName("imperial", rank);
		String rebelSkill = getRankSkillName("rebel", rank);
		// Remove faction mismatches
		if (!imperialSkill.isEmpty() && player->hasSkill(imperialSkill) && !isImperial) {
			SkillManager::instance()->surrenderSkill(imperialSkill, player, notifyClient, /* verifyFrs */false);
		}

		if (!rebelSkill.isEmpty() && player->hasSkill(rebelSkill) && !isRebel) {
			SkillManager::instance()->surrenderSkill(rebelSkill, player, notifyClient, /* verifyFrs */false);
		}

		// Remove ranks above the player's rank, or if the faction skill tree is not
		// enabled.
		bool shouldRemove = rank > playerRank || !isFactionSkillTreeEnabled();
		if (shouldRemove) {
			if (!imperialSkill.isEmpty()) {
				SkillManager::instance()->surrenderSkill(imperialSkill, player, notifyClient, /* verifyFrs */false);
			}
			if (!rebelSkill.isEmpty()) {
				SkillManager::instance()->surrenderSkill(rebelSkill, player, notifyClient, /* verifyFrs */false);
			}
		}

		bool shouldAdd = rank <= playerRank && isFactionSkillTreeEnabled();
		if (shouldAdd) {
			if (isImperial && !imperialSkill.isEmpty()) {
				SkillManager::instance()->awardSkill(imperialSkill, player, notifyClient, /* awardRequiredSkills */true, /* noXpRequired */true);
			}

			if (isRebel && !rebelSkill.isEmpty()) {
				SkillManager::instance()->awardSkill(rebelSkill, player, notifyClient, /* awardRequiredSkills */true, /* noXpRequired */true);
			}
		}
	}
}
