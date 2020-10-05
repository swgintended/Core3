/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef UNINVITECOMMAND_H_
#define UNINVITECOMMAND_H_

#include "server/zone/objects/scene/SceneObject.h"
#include "server/chat/StringIdChatParameter.h"
#include "server/zone/objects/creature/CreatureObject.h"



class UninviteCommand : public QueueCommand {
public:

	UninviteCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

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

		try {
			Locker clocker(targetPlayer, creature);

			if (targetPlayer->getGroupInviterID() != creature->getObjectID()) {
				creature->sendSystemMessage("@group:must_be_leader");
				return GENERALERROR;
			} else {
				targetPlayer->updateGroupInviterID(0);

				StringIdChatParameter playerStringId;
				playerStringId.setStringId("group", "uninvite_self"); // You cancel %TT's invitation.
				playerStringId.setTT(targetPlayer->getDisplayedName());
				creature->sendSystemMessage(playerStringId);

				StringIdChatParameter targetStringId;
				targetStringId.setStringId("group", "uninvite_target"); // %TT cancels the invitation.
				targetStringId.setTT(creature->getDisplayedName());
				targetPlayer->sendSystemMessage(targetStringId);
			}

		} catch (Exception& e) {
			System::out << "Exception in parseGroupUninvite(CreatureObject* player, Message* pack)\n";
			return GENERALERROR;
		}

		return SUCCESS;
	}

};

#endif //UNINVITECOMMAND_H_
