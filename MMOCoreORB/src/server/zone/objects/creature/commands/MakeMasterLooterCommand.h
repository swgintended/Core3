/*
 * MakeMasterLooterCommand.h
 *
 *  Modified on: March 4, 2015
 *      Author: Anakis
 */

#ifndef MAKEMASTERLOOTERCOMMAND_H_
#define MAKEMASTERLOOTERCOMMAND_H_

class MakeMasterLooterCommand : public QueueCommand {
public:

	MakeMasterLooterCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		//Check is in a group.
		ManagedReference<GroupObject*> group = creature->getGroup();
		if (group == nullptr) {
			StringIdChatParameter groupOnly("group","group_only"); //You can only set or check group looting options if you are in a group."
			creature->sendSystemMessage(groupOnly);
			return GENERALERROR;
		}

		Locker glocker(group, creature);

		//Check is group leader.
		if (group->getLeader() != creature) {
			StringIdChatParameter leaderOnly("group","leader_only"); //"Only the Group Leader can set the looting options."
			creature->sendSystemMessage(leaderOnly);
			return GENERALERROR;
		}

		GroupManager* manager = GroupManager::instance();

		//Check if current target is valid to be Master Looter. If not, send the list of potential Master Looters.
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

		if (targetPlayer != nullptr && group->hasMember(targetPlayer)) {
			manager->changeMasterLooter(group, targetPlayer, true);
			return SUCCESS;
		} else
			manager->sendMasterLooterList(group, creature);

		return SUCCESS;
	}

};




#endif /* MAKEMASTERLOOTERCOMMAND_H_ */
