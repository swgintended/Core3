/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef ANIMALCALMCOMMAND_H_
#define ANIMALCALMCOMMAND_H_

#include "server/zone/objects/creature/commands/JediQueueCommand.h"
#include "ForcePowersQueueCommand.h"
#include "server/zone/objects/creature/ai/Creature.h"
#include "server/zone/managers/combat/CombatManager.h"
#include "server/zone/objects/tangible/threat/ThreatMap.h"

class AnimalCalmCommand : public ForcePowersQueueCommand {
public:

	AnimalCalmCommand(const String& name, ZoneProcessServer* server)
		: ForcePowersQueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		if (isWearingArmor(creature)) {
			return NOJEDIARMOR;
		}

		ManagedReference<SceneObject*> sceneObject = server->getZoneServer()->getObject(target);
		if (sceneObject == nullptr)
			return INVALIDTARGET;

		if (!sceneObject->isCreature()) {
			creature->sendSystemMessage("@error_message:target_not_creature");
			return GENERALERROR;
		}

		return doCombatAction(creature, target);
	}

	void sendAttackCombatSpam(TangibleObject* attacker, TangibleObject* defender, int attackResult, int damage, const CreatureAttackData& data) const {
		// There are no specific animalcalm combat spam messages
	}

};

#endif //ANIMALCALMCOMMAND_H_
