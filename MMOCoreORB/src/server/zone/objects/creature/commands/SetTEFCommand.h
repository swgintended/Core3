/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef SETTEFCOMMAND_H_
#define SETTEFCOMMAND_H_

#include "templates/faction/Factions.h"
#include "server/zone/objects/player/sui/inputbox/SuiInputBox.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"

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

		CreatureObject* targetPlayer = nullptr;

		if (target != 0) {
			Reference<SceneObject*> targetSceneObject = server->getZoneServer()->getObject(target);
			if (targetSceneObject != nullptr && targetSceneObject->isPlayerCreature()) {
				targetPlayer = targetSceneObject.castTo<CreatureObject*>();
			}
		} else {
			String playerName;
			if (!args.hasMoreTokens()) {
				// If there is not selected target and no provided player name, show
				// syntax help.
				return INVALIDSYNTAX;
			}

			args.getStringToken(playerName);
			targetPlayer = playerManager->getPlayer(playerName);
		}

		if (targetPlayer == nullptr) {
			creature->sendSystemMessage("Target must be a player");
			return INVALIDTARGET;
		}

		SetTEFCommand::showListEnemyFlags(creature, targetPlayer);

		return SUCCESS;
	}

	String getSyntax() const {
		return "/setTEF <target>";
	}

	static void showListEnemyFlags(CreatureObject* admin, CreatureObject* player) {
		Reference<PlayerObject*> ghost = player->getPlayerObject();
		if (ghost == nullptr) {
			admin->sendSystemMessage(SetTEFCommand::ERROR_NULL_GHOST);
			return;
		}

		admin->getPlayerObject()->closeSuiWindowType(SuiWindowType::ADMIN_SET_TEF);

		ManagedReference<SuiListBox*> listbox = new SuiListBox(admin, SuiWindowType::ADMIN_SET_TEF, SuiListBox::HANDLETHREEBUTTON);
		listbox->setPromptTitle("Enemy Flags");
		String playerAndFaction = SetTEFCommand::formatPlayerAndGCWFaction(player);
		String prompt = "\n\nSelect an enemy flag and press OK to modify or remove it, or select an option and press OK to continue.";
		listbox->setPromptText(playerAndFaction + prompt);
		listbox->setOkButton(true, "@ok");
		listbox->setCancelButton(true, "@cancel");
		listbox->setOtherButton(true, "@refresh");

		EnemyFlags* flags = ghost->getEnemyFlags();
		for (int i = 0; i < flags->size(); i++) {
			EnemyFlag* flag = flags->get(i);
			uint64 flagID = SetTEFCommand::generateEnemyFlagID(flag);
			listbox->addMenuItem(SetTEFCommand::formatEnemyFlag(flag, admin->getZoneServer()), flagID);
		}

		listbox->addMenuItem("Add Flag", SetTEFCommand::OPTION_ADDFLAG);
		listbox->addMenuItem("Remove All", SetTEFCommand::OPTION_REMOVEALL);

		listbox->setCallback(new LambdaSuiCallback([player, flags](CreatureObject* admin, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
			if (!sui->isListBox() || !admin->getPlayerObject()->isPrivileged()) {
				return;
			}

			if (SetTEFCommand::isOtherPressed(args)) {
				// Refresh window
				SetTEFCommand::showListEnemyFlags(admin, player);
				return;
			}

			bool hasOtherButton = true;
			if (SetTEFCommand::isSuiCancelled(eventIndex, args, hasOtherButton)) {
				return;
			}

			SuiListBox* listbox = cast<SuiListBox*>(sui);
			uint64 actionOrID = listbox->getMenuObjectID(SetTEFCommand::getSuiSelectedIndex(args, hasOtherButton));
			switch (actionOrID) {
				case SetTEFCommand::OPTION_ADDFLAG:
					SetTEFCommand::showAddEnemyFlag(admin, player);
					break;
				case SetTEFCommand::OPTION_REMOVEALL:
					flags->removeAll();
					SetTEFCommand::showListEnemyFlags(admin, player);
					break;
				default:
					// Selected an EnemyFlag to edit
					for (int i = 0; i < flags->size(); i++) {
						EnemyFlag* flag = flags->get(i);
						uint64 flagID = SetTEFCommand::generateEnemyFlagID(flag);
						if (flagID == actionOrID) {
							SetTEFCommand::showEditEnemyFlag(admin, player, flag);
							return;
						}
					}

					admin->sendSystemMessage("Failed to find the selected flag.");
					SetTEFCommand::showListEnemyFlags(admin, player);
					break;
			}
		}, admin->getZoneServer(), "SetTEFCallback"));

		admin->getPlayerObject()->addSuiBox(listbox);
		admin->sendMessage(listbox->generateMessage());
	}

	static void showEditEnemyFlag(CreatureObject* admin, CreatureObject* player, EnemyFlag* flag) {
		admin->getPlayerObject()->closeSuiWindowType(SuiWindowType::ADMIN_SET_TEF);

		Reference<PlayerObject*> ghost = player->getPlayerObject();
		if (ghost == nullptr) {
			admin->sendSystemMessage(SetTEFCommand::ERROR_NULL_GHOST);
			return;
		}

		ManagedReference<SuiListBox*> listbox = new SuiListBox(admin, SuiWindowType::ADMIN_SET_TEF, SuiListBox::HANDLETWOBUTTON);
		listbox->setPromptTitle("Edit Enemy Flag");
		String playerAndFaction = SetTEFCommand::formatPlayerAndGCWFaction(player);
		String flagLine = "\n\nFlag:" + SetTEFCommand::formatEnemyFlag(flag, admin->getZoneServer());
		String prompt = "\n\nSelect an option and press OK to continue.";
		listbox->setPromptText(playerAndFaction + flagLine + prompt);
		listbox->setOkButton(true, "@ok");
		listbox->setCancelButton(true, "@back");

		if (flag->isTemporary()) {
			listbox->addMenuItem("@refresh", SetTEFCommand::OPTION_REFRESH);
			listbox->addMenuItem("Make Persistent", SetTEFCommand::OPTION_MAKE_PERM);
		} else {
			listbox->addMenuItem("Make Temporary (TEF)", SetTEFCommand::OPTION_MAKE_TEMP);
		}
		listbox->addMenuItem("@remove", SetTEFCommand::OPTION_REMOVE);

		listbox->setCallback(new LambdaSuiCallback([player, ghost, flag](CreatureObject* admin, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
			if (!sui->isListBox() || !admin->getPlayerObject()->isPrivileged()) {
				return;
			}

			if (SetTEFCommand::isSuiCancelled(eventIndex, args)) {
				SetTEFCommand::showListEnemyFlags(admin, player);
				return;
			}

			SuiListBox* listbox = cast<SuiListBox*>(sui);
			switch (listbox->getMenuObjectID(SetTEFCommand::getSuiSelectedIndex(args))) {
				case SetTEFCommand::OPTION_REFRESH:
				case SetTEFCommand::OPTION_MAKE_TEMP:
					flag->updateExpiration();
					SetTEFCommand::showEditEnemyFlag(admin, player, flag);
					break;
				case SetTEFCommand::OPTION_MAKE_PERM:
					flag->setPersistent();
					SetTEFCommand::showEditEnemyFlag(admin, player, flag);
					break;
				case SetTEFCommand::OPTION_REMOVE:
					ghost->getEnemyFlags()->removeEnemy(flag->getEnemyID(), flag->getFaction());
					SetTEFCommand::showListEnemyFlags(admin, player);
					break;
				default:
					admin->sendSystemMessage(SetTEFCommand::ERROR_UNKNOWN_OPTION);
					break;
			}
		}, admin->getZoneServer(), "SetTEFCallback"));

		admin->getPlayerObject()->addSuiBox(listbox);
		admin->sendMessage(listbox->generateMessage());
	}

	static void showAddEnemyFlag(CreatureObject* admin, CreatureObject* player) {
		admin->getPlayerObject()->closeSuiWindowType(SuiWindowType::ADMIN_SET_TEF);

		ManagedReference<SuiListBox*> listbox = new SuiListBox(admin, SuiWindowType::ADMIN_SET_TEF, SuiListBox::HANDLETWOBUTTON);
		listbox->setPromptTitle("Add Enemy Flag");
		String playerAndFaction = SetTEFCommand::formatPlayerAndGCWFaction(player);
		String prompt = "\n\nSelect a faction and press Next to continue.";
		listbox->setPromptText(playerAndFaction + prompt);
		listbox->setOkButton(true, "@next");
		listbox->setCancelButton(true, "@back");

		bool addColor = true;
		listbox->addMenuItem(SetTEFCommand::formatFaction(Factions::FACTIONIMPERIAL, addColor), Factions::FACTIONIMPERIAL);
		listbox->addMenuItem(SetTEFCommand::formatFaction(Factions::FACTIONREBEL, addColor), Factions::FACTIONREBEL);
		listbox->addMenuItem(SetTEFCommand::formatFaction(Factions::FACTIONBOUNTY, addColor), Factions::FACTIONBOUNTY);
		listbox->addMenuItem(SetTEFCommand::formatFaction(Factions::FACTIONBOSS, addColor), Factions::FACTIONBOSS);
		listbox->addMenuItem(SetTEFCommand::formatFaction(Factions::FACTIONDUEL, addColor), Factions::FACTIONDUEL);
		listbox->addMenuItem(SetTEFCommand::formatFaction(Factions::FACTIONSERVER, addColor), Factions::FACTIONSERVER);

		listbox->setCallback(new LambdaSuiCallback([player](CreatureObject* admin, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
			if (!sui->isListBox() || !admin->getPlayerObject()->isPrivileged()) {
				return;
			}

			if (SetTEFCommand::isSuiCancelled(eventIndex, args)) {
				SetTEFCommand::showListEnemyFlags(admin, player);
				return;
			}

			SuiListBox* listbox = cast<SuiListBox*>(sui);
			uint32 faction = listbox->getMenuObjectID(SetTEFCommand::getSuiSelectedIndex(args));
			SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
		}, admin->getZoneServer(), "SetTEFCallback"));

		admin->getPlayerObject()->addSuiBox(listbox);
		admin->sendMessage(listbox->generateMessage());
	}

private:

	static void showAddEnemyFlagChooseEnemy(CreatureObject* admin, CreatureObject* player, uint32 faction) {
		ManagedReference<SuiListBox*> listbox = new SuiListBox(admin, SuiWindowType::ADMIN_SET_TEF, SuiListBox::HANDLETWOBUTTON);
		listbox->setPromptTitle("Add Enemy Flag");
		String playerAndFaction = SetTEFCommand::formatPlayerAndGCWFaction(player);
		String flag = "\nFlag: " + SetTEFCommand::formatFaction(faction, /*addColor*/true, /*addParens*/true);
		String prompt = "\n\nSelect an enemy type and press Next to continue.";
		listbox->setPromptText(playerAndFaction + "\n" + flag + prompt);
		listbox->setOkButton(true, "@next");
		listbox->setCancelButton(true, "@back");

		switch (faction) {
			case Factions::FACTIONIMPERIAL:
			case Factions::FACTIONREBEL:
				listbox->addMenuItem("Target", SetTEFCommand::OPTION_ENEMY_TARGET);
				listbox->addMenuItem("@player_name", SetTEFCommand::OPTION_ENEMY_NAME);
				listbox->addMenuItem("Group (Target)", SetTEFCommand::OPTION_ENEMY_GROUPTARGET);
				listbox->addMenuItem("Group (Player Name)", SetTEFCommand::OPTION_ENEMY_GROUPNAME);
				listbox->addMenuItem("None", SetTEFCommand::OPTION_ENEMY_NONE);
				break;
			case Factions::FACTIONBOUNTY:
				listbox->addMenuItem("Target", SetTEFCommand::OPTION_ENEMY_TARGET);
				listbox->addMenuItem("@player_name", SetTEFCommand::OPTION_ENEMY_NAME);
				listbox->addMenuItem("Group (Target)", SetTEFCommand::OPTION_ENEMY_GROUPTARGET);
				listbox->addMenuItem("Group (Player Name)", SetTEFCommand::OPTION_ENEMY_GROUPNAME);
				break;
			case Factions::FACTIONBOSS:
				listbox->addMenuItem("Target", SetTEFCommand::OPTION_ENEMY_TARGET);
				break;
			case Factions::FACTIONDUEL:
				listbox->addMenuItem("Target", SetTEFCommand::OPTION_ENEMY_TARGET);
				listbox->addMenuItem("@player_name", SetTEFCommand::OPTION_ENEMY_NAME);
				break;
			case Factions::FACTIONSERVER:
				listbox->addMenuItem("None", SetTEFCommand::OPTION_ENEMY_NONE);
				break;
		}

		listbox->setCallback(new LambdaSuiCallback([player, faction](CreatureObject* admin, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
			if (!sui->isListBox() || !admin->getPlayerObject()->isPrivileged()) {
				return;
			}

			if (SetTEFCommand::isSuiCancelled(eventIndex, args)) {
				SetTEFCommand::showAddEnemyFlag(admin, player);
				return;
			}

			SuiListBox* listbox = cast<SuiListBox*>(sui);
			uint64 option = listbox->getMenuObjectID(SetTEFCommand::getSuiSelectedIndex(args));
			if (option == SetTEFCommand::OPTION_ENEMY_NONE) {
				SetTEFCommand::showAddEnemyFlagChooseDuration(admin, player, faction, 0);
				return;
			}

			if (option == SetTEFCommand::OPTION_ENEMY_TARGET || option == SetTEFCommand::OPTION_ENEMY_GROUPTARGET) {
				uint64 targetID = admin->getTargetID();
				if (targetID == player->getObjectID()) {
					admin->sendSystemMessage(SetTEFCommand::ERROR_ENEMY_IS_PLAYER);
					SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
					return;
				}

				Reference<SceneObject*> target = admin->getZoneServer()->getObject(targetID);
				if (target == nullptr) {
					admin->sendSystemMessage("@error_message:targetting_error");
					SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
					return;
				}

				if (faction == Factions::FACTIONBOSS && (!target->isCreatureObject() || target->isPlayerCreature())) {
					// Boss faction cannot select players as the target enemy, must be a
					// creature.
					admin->sendSystemMessage("@error_message:target_not_creature");
					SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
					return;
				} else if (faction != Factions::FACTIONBOSS && !target->isPlayerCreature()) {
					// All other factions cannot select creatures as the target enemy,
					// must be players.
					admin->sendSystemMessage("@error_message:target_not_player");
					SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
					return;
				}

				if (option == SetTEFCommand::OPTION_ENEMY_TARGET) {
					SetTEFCommand::showAddEnemyFlagChooseDuration(admin, player, faction, targetID);
					return;
				}

				CreatureObject* enemy = target.castTo<CreatureObject*>();
				uint64 groupID = enemy->getGroupID();
				if (groupID == 0) {
					admin->sendSystemMessage(SetTEFCommand::ERROR_NO_GROUP);
					SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
					return;
				}

				SetTEFCommand::showAddEnemyFlagChooseDuration(admin, player, faction, groupID);
				return;
			}

			SetTEFCommand::showAddEnemyFlagChooseEnemyName(admin, player, faction, option);
		}, admin->getZoneServer(), "SetTEFCallback"));

		admin->getPlayerObject()->addSuiBox(listbox);
		admin->sendMessage(listbox->generateMessage());
	}

	static void showAddEnemyFlagChooseEnemyName(CreatureObject* admin, CreatureObject* player, uint32 faction, uint64 option) {
		ManagedReference<SuiInputBox*> inputbox = new SuiInputBox(admin, SuiWindowType::ADMIN_SET_TEF);
		inputbox->setPromptTitle("Enemy Name");
		String prompt;
		if (option == SetTEFCommand::OPTION_ENEMY_NAME) {
			prompt = "Enter the first name of the enemy player.";
		} else {
			prompt = "Enter the first name of a player in the enemy group.";
		}

		inputbox->setPromptText(prompt);
		inputbox->setOkButton(true, "@ok");
		inputbox->setCancelButton(true, "@back");
		inputbox->setCallback(new LambdaSuiCallback([player, faction, option](CreatureObject* admin, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
			if (!admin->getPlayerObject()->isPrivileged()) {
				return;
			}

			if (eventIndex == 1) {
				SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
				return;
			}

			String enemyName = args->get(0).toString();
			CreatureObject* enemy = admin->getZoneServer()->getPlayerManager()->getPlayer(enemyName);
			if (enemy == nullptr) {
				StringIdChatParameter error("@ui:action_target_not_found_prose");
				error.setTT(enemyName);
				admin->sendSystemMessage(error);
				SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
				return;
			}

			if (enemy->getObjectID() == player->getObjectID()) {
				admin->sendSystemMessage(SetTEFCommand::ERROR_ENEMY_IS_PLAYER);
				SetTEFCommand::showAddEnemyFlagChooseEnemyName(admin, player, faction, option);
				return;
			}

			if (option == SetTEFCommand::OPTION_ENEMY_NAME) {
				SetTEFCommand::showAddEnemyFlagChooseDuration(admin, player, faction, enemy->getObjectID());
				return;
			}

			uint64 groupID = enemy->getGroupID();
			if (groupID == 0) {
				admin->sendSystemMessage(SetTEFCommand::ERROR_NO_GROUP);
				SetTEFCommand::showAddEnemyFlagChooseEnemy(admin, player, faction);
				return;
			}

			SetTEFCommand::showAddEnemyFlagChooseDuration(admin, player, faction, groupID);
		}, admin->getZoneServer(), "SetTEFCallback"));

		admin->getPlayerObject()->addSuiBox(inputbox);
		admin->sendMessage(inputbox->generateMessage());
	}

	static void showAddEnemyFlagChooseDuration(CreatureObject* admin, CreatureObject* player, uint32 faction, uint64 enemyID) {
		Reference<PlayerObject*> ghost = player->getPlayerObject();
		if (ghost == nullptr) {
			admin->sendSystemMessage(SetTEFCommand::ERROR_NULL_GHOST);
			return;
		}

		ManagedReference<SuiListBox*> listbox = new SuiListBox(admin, SuiWindowType::ADMIN_SET_TEF, SuiListBox::HANDLETWOBUTTON);
		listbox->setPromptTitle("Flag Duration");
		String playerAndFaction = SetTEFCommand::formatPlayerAndGCWFaction(player);
		String flagLine = "\n\nFlag: " +
			SetTEFCommand::formatEnemy(enemyID, admin->getZoneServer()) + " " +
			SetTEFCommand::formatFaction(faction, /*addColor*/true, /*addParens*/true);
		String prompt = "\n\nSelect a duration and press Add to finish.";
		listbox->setPromptText(playerAndFaction + flagLine + prompt);
		listbox->setOkButton(true, "@add");
		listbox->setCancelButton(true, "@back");

		listbox->addMenuItem("Temporary (TEF)", SetTEFCommand::OPTION_MAKE_TEMP);
		listbox->addMenuItem("Persistent", SetTEFCommand::OPTION_MAKE_PERM);

		listbox->setCallback(new LambdaSuiCallback([player, ghost, faction, enemyID](CreatureObject* admin, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) {
			if (!sui->isListBox() || !admin->getPlayerObject()->isPrivileged()) {
				return;
			}

			if (SetTEFCommand::isSuiCancelled(eventIndex, args)) {
				SetTEFCommand::showAddEnemyFlag(admin, player);
				return;
			}

			SuiListBox* listbox = cast<SuiListBox*>(sui);
			switch (listbox->getMenuObjectID(SetTEFCommand::getSuiSelectedIndex(args))) {
				case SetTEFCommand::OPTION_MAKE_TEMP:
					ghost->getEnemyFlags()->addEnemy(enemyID, faction, EnemyFlag::TEFTIMER);
					SetTEFCommand::showListEnemyFlags(admin, player);
					break;
				case SetTEFCommand::OPTION_MAKE_PERM:
					ghost->getEnemyFlags()->addEnemy(enemyID, faction);
					SetTEFCommand::showListEnemyFlags(admin, player);
					break;
				default:
					admin->sendSystemMessage(SetTEFCommand::ERROR_UNKNOWN_OPTION);
					break;
			}
		}, admin->getZoneServer(), "SetTEFCallback"));

		admin->getPlayerObject()->addSuiBox(listbox);
		admin->sendMessage(listbox->generateMessage());
	}

	static uint64 generateEnemyFlagID(EnemyFlag* flag) {
		return String::hashCode(String::valueOf(flag->getEnemyID()) + String::valueOf(flag->getFaction()));
	}

	static String formatPlayerAndGCWFaction(CreatureObject* player) {
		String playerName = SetTEFCommand::formatCreatureName(player);
		String faction = SetTEFCommand::formatGCWFaction(player);
		String factionColor = SetTEFCommand::formatFactionColor(player->getFaction());
		return playerName + factionColor + " (" + faction + ") \\#ptext1 ";
	}

	static String formatEnemyFlag(EnemyFlag* flag, ZoneServer* zoneServer) {
		String enemyName = SetTEFCommand::formatEnemy(flag->getEnemyID(), zoneServer);
		if (!enemyName.isEmpty()) {
			enemyName += " ";
		}

		String faction = SetTEFCommand::formatFaction(flag->getFaction());
		String factionColor = SetTEFCommand::formatFactionColor(flag->getFaction());
		String expires = SetTEFCommand::formatExpiration(flag);

		return enemyName + factionColor + "(" + faction + ") " + expires;
	}

	static String formatEnemy(uint64 enemyID, ZoneServer* zoneServer) {
		if (enemyID == 0) {
			return "";
		}

		Reference<SceneObject*> enemy = zoneServer->getObject(enemyID);
		if (enemy == nullptr) {
			return "(NULL: " + String::valueOf(enemyID) + ")";
		} else if (enemy->isGroupObject()) {
			return SetTEFCommand::formatGroupName(enemy.castTo<GroupObject*>());
		} else if (enemy->isCreatureObject())  {
			return SetTEFCommand::formatCreatureName(enemy.castTo<CreatureObject*>());
		} else {
			return "(Unknown: " + String::valueOf(enemyID) + ")";
		}
	}

	static String formatCreatureName(CreatureObject* player) {
		return player->getCustomObjectName().toString();
	}

	static String formatGroupName(GroupObject* group) {
		CreatureObject* leader = group->getLeader();
		if (leader == nullptr) {
			return "Group (Disbanded)";
		}

		return "Group (" + SetTEFCommand::formatCreatureName(leader) + ")";
	}

	static String formatFaction(uint32 faction, bool addColor = false, bool addParens = false) {
		String factionStr;
		switch (faction) {
			case Factions::FACTIONIMPERIAL:
				factionStr = "Imperial";
				break;
			case Factions::FACTIONREBEL:
				factionStr = "Rebel";
				break;
			case Factions::FACTIONDUEL:
				factionStr = "Duel";
				break;
			case Factions::FACTIONBOSS:
				factionStr = "Boss";
				break;
			case Factions::FACTIONBOUNTY:
				factionStr = "Bounty";
				break;
			case Factions::FACTIONSERVER:
				factionStr = "Server";
				break;
			default:
				factionStr = "Unknown";
				break;
		}

		if (addParens) {
			factionStr = "(" + factionStr + ")";
		}

		if (addColor) {
			factionStr = SetTEFCommand::formatFactionColor(faction) + factionStr + " \\#ptext1 ";
		}

		return factionStr;
	}

	static String formatGCWFaction(CreatureObject* player) {
		bool isGCW = false;
		String faction = "Neutral";
		switch (player->getFaction()) {
			case Factions::FACTIONIMPERIAL:
				faction = "Imperial";
				isGCW = true;
				break;
			case Factions::FACTIONREBEL:
				faction = "Rebel";
				isGCW = true;
				break;
		}

		if (isGCW) {
			switch (player->getFactionStatus()) {
				case FactionStatus::COVERT:
					faction += " Covert";
					break;
				case FactionStatus::ONLEAVE:
					faction += " On Leave";
					break;
				case FactionStatus::OVERT:
					faction += " Overt";
					break;
			}
		}

		return faction;
	}

	static String formatFactionColor(uint32 faction) {
		switch (faction) {
			case Factions::FACTIONIMPERIAL:
				return "\\#pmind ";
			case Factions::FACTIONREBEL:
				return "\\#phealth ";
			default:
				return "\\#pstamina ";
		}
	}

	static String formatExpiration(EnemyFlag* flag) {
		String when;
		if (flag->isTemporary()) {
			int diff = flag->getExpiration() - Time().getTime();
			if (diff < 0) {
				diff = 0;
			}

			int seconds = diff % 60;
			int minutes = (diff - seconds) / 60;
			String secondsStr = String::valueOf(abs(seconds));
			if (abs(seconds) < 10) {
				secondsStr = "0" + secondsStr;
			}

			when = String::valueOf(minutes) + ":" + secondsStr;
		} else {
			when = "Never";
		}

		return "\\#pcontrast2 Expires: " + when + "\\#ptext1 ";
	}

	static const uint32 OPTION_ADDFLAG = STRING_HASHCODE("addflag");
	static const uint32 OPTION_REMOVEALL = STRING_HASHCODE("removeall");
	static const uint32 OPTION_REMOVE = STRING_HASHCODE("remove");
	static const uint32 OPTION_REFRESH = STRING_HASHCODE("refresh");
	static const uint32 OPTION_MAKE_TEMP = STRING_HASHCODE("maketemp");
	static const uint32 OPTION_MAKE_PERM = STRING_HASHCODE("makeperm");
	static const uint32 OPTION_ENEMY_TARGET = STRING_HASHCODE("enemytarget");
	static const uint32 OPTION_ENEMY_NAME = STRING_HASHCODE("enemyname");
	static const uint32 OPTION_ENEMY_GROUPTARGET = STRING_HASHCODE("enemygrouptarget");
	static const uint32 OPTION_ENEMY_GROUPNAME = STRING_HASHCODE("enemygroupname");
	static const uint32 OPTION_ENEMY_NONE = STRING_HASHCODE("enemynone");
	static const String ERROR_NULL_GHOST;
	static const String ERROR_UNKNOWN_OPTION;
	static const String ERROR_NO_GROUP;
	static const String ERROR_ENEMY_IS_PLAYER;

	static bool isSuiCancelled(uint32 eventIndex, Vector<UnicodeString>* args, bool hasOtherButton = false) {
		if (SetTEFCommand::getSuiSelectedIndex(args, hasOtherButton) == -1) {
			return true;
		}

		return eventIndex == 1;
	}

	static bool isOtherPressed(Vector<UnicodeString>* args) {
		return Bool::valueOf(args->get(0).toString());
	}

	static int getSuiSelectedIndex(Vector<UnicodeString>* args, bool hasOtherButton = false) {
		int sizeCheck = hasOtherButton ? 2 : 1;
		if (args->size() < sizeCheck) {
			return -1;
		}

		int index = hasOtherButton ? 1 : 0;
		return Integer::valueOf(args->get(hasOtherButton).toString());
	}
};

const String SetTEFCommand::ERROR_NULL_GHOST = "Player ghost is unexpectedly null, cannot retrieve enemy flags.";
const String SetTEFCommand::ERROR_UNKNOWN_OPTION = "Unhandled option selected.";
const String SetTEFCommand::ERROR_NO_GROUP = "The selected enemy is not grouped.";
const String SetTEFCommand::ERROR_ENEMY_IS_PLAYER = "The selected enemy cannot be the same as the player.";

#endif //SETTEFCOMMAND_H_
