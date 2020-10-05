/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef INVITECOMMAND_H_
#define INVITECOMMAND_H_


#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/managers/group/GroupManager.h"
#include "server/zone/ZoneServer.h"

class InviteCommand : public QueueCommand {
public:

	InviteCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		PlayerObject* playerObject = creature->getPlayerObject();
		bool godMode = false;

		if (playerObject)
		{
			if (playerObject->hasGodMode())
				godMode = true;
		}

		GroupManager* groupManager = GroupManager::instance();

		ManagedReference<SceneObject*> object = server->getZoneServer()->getObject(target);
		ManagedReference<CreatureObject*> targetPlayer = nullptr;
		StringTokenizer args(arguments.toString());
		if (object == nullptr || !object->isPlayerCreature()) {
			String firstName;
			if (args.hasMoreTokens()) {
				args.getStringToken(firstName);
				targetPlayer = server->getZoneServer()->getPlayerManager()->getPlayer(firstName);
				if (targetPlayer != nullptr && !targetPlayer->isOnline()) {
					targetPlayer = nullptr;
				}
			}
		} else {
			targetPlayer = cast<CreatureObject*>(object.get());
		}

		if (targetPlayer == nullptr)
			return GENERALERROR;

		if (!targetPlayer->getPlayerObject()->isIgnoring(creature->getFirstName().toLowerCase()) || godMode)
			groupManager->inviteToGroup(creature, targetPlayer);

		return SUCCESS;
	}

};

#endif //INVITECOMMAND_H_
