/*
 * FactionManager.h
 *
 *  Created on: Mar 17, 2011
 *      Author: crush
 */

#ifndef FACTIONMANAGER_H_
#define FACTIONMANAGER_H_

#include "FactionMap.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "system/lang.h"
#include "templates/faction/FactionRanks.h"

class FactionManager : public Singleton<FactionManager>, public Logger, public Object {
	FactionMap factionMap;
	FactionRanks factionRanks;

public:
	FactionManager();

	static const int TEFTIMER = 600000;

	/**
	 * Loads faction configuration information from the faction manager lua file: managers/faction_manager.lua
	 * Loads faction ranks from datatable
	 * Sets up faction relationships
	 */
	void loadData();

	/**
	 * Awards points to the player based on the faction they killed.
	 * @pre: player locked
	 * @post: player locked
	 * @param player The player receiving the faction points for the kill.
	 * @param faction The string key of the faction that was killed.
	 * @param level The level of the mob that was killed
	 */
	void awardFactionStanding(CreatureObject* player, const String& factionName, int level);

	void awardPvpFactionPoints(TangibleObject* killer, CreatureObject* destructedObject);

	/**
	 * Gets a list of enemy factions to the faction passed to the method.
	 * @param faction The faction to check for enemies.
	 */
	SortedVector<String> getEnemyFactions(const String& faction);

	/**
	 * Gets a list of ally factions to the faction passed to the method.
	 * @param faction The faction to check for enemies.
	 */
	SortedVector<String> getAllyFactions(const String& faction);

	FactionMap* getFactionMap();

	String getRankName(int idx);
	int getRankCost(int rank);
	int getRankDelegateRatioFrom(int rank);
	int getRankDelegateRatioTo(int rank);
	String getRankSkillName(const String& faction, int rank) const;
	int getFactionPointsCap(int rank);

	bool isHighestRank(int rank) {
		return rank >= factionRanks.getCount() - 1 || rank >= getHighestRank();
	}

	int getHighestRank() const {
		return maxFactionRank;
	}

	bool isFaction(const String& faction);
	bool isEnemy(const String& faction1, const String& faction2);
	bool isAlly(const String& faction1, const String& faction2);

	bool isFactionSkillTreeEnabled() const {
		return factionSkillTree;
	}

	void onPlayerLoggedIn(CreatureObject* player) const;
	void updatePlayerFactionSkills(CreatureObject* player, bool notifyClient = true) const;

protected:
	void loadFactionRanks();
	void loadLuaConfig();

private:
	int maxFactionRank = 15;
	bool factionSkillTree = false;
	VectorMap<String, Vector<String>*> factionSkillTreeNames;

	void populateFactionSkillTree(String faction, LuaObject names);

};

#endif /* FACTIONMANAGER_H_ */
