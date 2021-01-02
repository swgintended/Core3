/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef REVOKEBADGECOMMAND_H_
#define REVOKEBADGECOMMAND_H_

class RevokeBadgeCommand : public QueueCommand {
public:

	RevokeBadgeCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (arguments.isEmpty() && target == 0) {
			creature->sendSystemMessage("Syntax: /revokeBadge [badge id]");
			return GENERALERROR;
		}

		ManagedReference<SceneObject*> targetObject = server->getZoneServer()->getObject(target);
		StringTokenizer args(arguments.toString());

		float badgeId = args.getFloatToken();

		if (targetObject == nullptr || !targetObject->isPlayerCreature()) {
			creature->sendSystemMessage("Invalid target.");
			return INVALIDTARGET;
		}

		ManagedReference<CreatureObject*> player = cast<CreatureObject*>(targetObject.get());

		if (player != nullptr) {
			Locker crossLocker(player, creature);

			server->getPlayerManager()->revokeBadge(player->getPlayerObject(), badgeId);
			creature->sendSystemMessage("Revoked badge " + String::valueOf(badgeId) + " from " + player->getDisplayedName() + ".");
		}

		return SUCCESS;
	}

};

#endif //REVOKEBADGECOMMAND_H_
