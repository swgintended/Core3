/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef MAKELEADERCOMMAND_H_
#define MAKELEADERCOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/group/GroupObject.h"
#include "server/zone/managers/group/GroupManager.h"
#include "server/zone/objects/creature/CreatureObject.h"


class MakeLeaderCommand : public QueueCommand {
public:

	MakeLeaderCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

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

		GroupObject* group = creature->getGroup();

		if (group == nullptr)
			return GENERALERROR;

		groupManager->makeLeader(group, creature, targetPlayer);

		return SUCCESS;
	}

};

#endif //MAKELEADERCOMMAND_H_
