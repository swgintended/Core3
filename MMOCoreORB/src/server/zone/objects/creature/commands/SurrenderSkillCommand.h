/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef SURRENDERSKILLCOMMAND_H_
#define SURRENDERSKILLCOMMAND_H_

#include "server/zone/managers/skill/SkillManager.h"

class SurrenderSkillCommand : public QueueCommand {
public:

	SurrenderSkillCommand(const String& name, ZoneProcessServer* server)
		: QueueCommand(name, server) {

	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {

		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		String skillName = arguments.toString();
		if (skillName.beginsWith("faction_")) {
			// Faction ranks cannot be dropped without speaking to a recruiter and resigning
			creature->sendSystemMessage("You must speak with a faction recruiter and resign to remove yourself from this rank.");
			return INVALIDPARAMETERS;
		}

		SkillManager* skillManager = SkillManager::instance();
		skillManager->surrenderSkill(skillName, creature, true);

		return SUCCESS;
	}

};

#endif //SURRENDERSKILLCOMMAND_H_
