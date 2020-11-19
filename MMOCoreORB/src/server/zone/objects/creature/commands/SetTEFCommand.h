/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef SETTEFCOMMAND_H_
#define SETTEFCOMMAND_H_

#include "templates/faction/Factions.h"

class SetTEFCommand : public QueueCommand {
public:

	SetTEFCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;


		StringTokenizer args(arguments.toString());
		PlayerManager* playerManager = server->getPlayerManager();

		PlayerObject* targetPlayer = nullptr;
		Reference<CreatureObject*> enemyCreature = nullptr;
		PlayerObject* enemyPlayer = nullptr;
		String faction = "server";
		uint64 duration = 0;

		if (target != 0) {
			Reference<SceneObject*> targetSceneObject = server->getZoneServer()->getObject(target);
			if (targetSceneObject != nullptr && targetSceneObject->isPlayerCreature()) {
				targetPlayer = targetSceneObject.castTo<CreatureObject*>()->getPlayerObject();
			}
		} else {
			String playerName;
			if (!args.hasMoreTokens()) {
				// If there is not selected target and no provided player name, show
				// syntax help.
				return INVALIDSYNTAX;
			}

			args.getStringToken(playerName);
			Reference<CreatureObject*> targetPlayerCreature = playerManager->getPlayer(playerName);
			if (targetPlayerCreature != nullptr) {
				targetPlayer = targetPlayerCreature->getPlayerObject();
			}
		}

		if (targetPlayer == nullptr) {
			creature->sendSystemMessage("Target must be a player");
			return INVALIDTARGET;
		}

		if (!args.hasMoreTokens()) {
			// Expecting at least an enemy player name
			creature->sendSystemMessage("An enemy player name or one of the 'imperial', 'rebel', or 'server' factions is required");
			return INVALIDSYNTAX;
		}

		String enemyPlayerNameOrFaction;
		args.getStringToken(enemyPlayerNameOrFaction);
		if (isFactionString(enemyPlayerNameOrFaction)) {
			if (isEnemyLessFactionString(enemyPlayerNameOrFaction)) {
				faction = enemyPlayerNameOrFaction;
			} else {
				creature->sendSystemMessage("An enemy player name is required for the provided faction");
				return INVALIDSYNTAX;
			}
		} else {
			enemyCreature = playerManager->getPlayer(enemyPlayerNameOrFaction);
			if (enemyCreature != nullptr) {
				enemyPlayer = enemyCreature->getPlayerObject();
			}

			if (enemyPlayer == nullptr) {
				creature->sendSystemMessage("Enemy target must be a player");
				return INVALIDSYNTAX;
			}
		}

		if (enemyPlayer == nullptr) {
			// Provided an enemy-less faction. Should check for a duration
			duration = parseDuration(&args);
		} else {
			// Check for faction and duration
			faction = parseFaction(&args, creature);
			if (faction.isEmpty()) {
				return INVALIDSYNTAX;
			}

			duration = parseDuration(&args);
		}

		uint64 enemyID = 0;
		uint32 factionID = String::hashCode(faction);
		if (enemyPlayer != nullptr) {
			if (factionID == Factions::FACTIONGROUP) {
				enemyID = enemyCreature->getGroupID();
				if (enemyID == 0) {
					creature->sendSystemMessage("Enemy target does not have a group for a group TEF");
					return INVALIDTARGET;
				}
			} else {
				enemyID = enemyPlayer->getObjectID();
			}
		}

		targetPlayer->getEnemyFlags()->addEnemy(enemyID, factionID, duration);
		creature->sendSystemMessage("TEF added to " + targetPlayer->getObjectName()->toString());

		return SUCCESS;
	}

	String getSyntax() const {
		String command = "/setTEF <targetPlayer> <enemy> [duel | boss | bounty | group | imperial | rebel | server*] [duration (s)]";
		String desc = "Adds or updates a Temporary Enemy Flag for the target player. Enemy should be another player, or left blank with the 'imperial' or 'rebel' faction for a GCW TEF.";
		return command + ": " + desc;
	}

private:

	bool isFactionString(String value) const {
		switch (String::hashCode(value)) {
			case Factions::FACTIONDUEL:
			case Factions::FACTIONBOSS:
			case Factions::FACTIONBOUNTY:
			case Factions::FACTIONGROUP:
			case Factions::FACTIONIMPERIAL:
			case Factions::FACTIONREBEL:
			case Factions::FACTIONSERVER:
				return true;
			default:
				return false;
		}
	}

	bool isEnemyLessFactionString(String value) const {
		switch (String::hashCode(value)) {
			case Factions::FACTIONIMPERIAL:
			case Factions::FACTIONREBEL:
			case Factions::FACTIONSERVER:
				return true;
			default:
				return false;
		}
	}

	String parseFaction(StringTokenizer* args, CreatureObject* creature) const {
		String faction = "server";
		if (args->hasMoreTokens()) {
			args->getStringToken(faction);
			if (!isFactionString(faction)) {
				creature->sendSystemMessage("'" + faction + "' is not a valid faction");
				return "";
			}
		}

		return faction;
	}

	uint64 parseDuration(StringTokenizer* args) const {
		uint64 duration = 0;
		if (args->hasMoreTokens()) {
			duration = args->getUnsignedLongToken();
		}

		return duration;
	}
};

#endif //SETTEFCOMMAND_H_
