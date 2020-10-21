/*
 * ScannerScanTask.cpp
 *
 *  Created on: Oct 18, 2020
 *      Author: Hakry
 */

#ifndef SCANNERSCANTASK_H_
#define SCANNERSCANTASK_H_

#include "ScannerDataComponent.h"
#include "engine/engine.h"
#include "server/zone/managers/combat/CombatManager.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/installation/components/ScannerDataComponent.h"
#include "server/zone/objects/player/FactionStatus.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/packets/scene/PlayClientEffectLocMessage.h"
#include "templates/faction/Factions.h"
#include "server/zone/Zone.h"


class ScannerScanTask : public Task {
	ManagedWeakReference<TangibleObject*> scannerObject;

public:

	ScannerScanTask(TangibleObject* scanner) {
		scannerObject = scanner;
	}

	void run() {

		ManagedReference<TangibleObject*> scanner = scannerObject.get();

		if (scanner == nullptr) {
			return;
		}

		ScannerDataComponent* scannerData = cast<ScannerDataComponent*>(scanner->getDataObjectComponent()->get());

		if (scannerData == nullptr) {
			return;
		}

		Locker lock(scanner);

		ManagedReference<CreatureObject*> target = scannerData->selectTarget();

		if (checkTarget(target, scanner)) {

			PlayClientEffectLoc* explodeLoc = new PlayClientEffectLoc("clienteffect/survey_effect.cef", scanner->getZone()->getZoneName(), scanner->getPositionX(), scanner->getPositionZ(), scanner->getPositionY());
			scanner->broadcastMessage(explodeLoc, false);

			ManagedReference<PlayerObject*> playerObject = target->getPlayerObject();

			if (playerObject == nullptr) {
				return;
			}

			target = scannerData->selectTarget();

			int revealChance = 90;

			if (System::random(100) < revealChance)  {

				if (target->getFaction() == Factions::FACTIONREBEL) {
					target->sendSystemMessage("The Faction Scanner has revealed you as a Covert Member of Rebel Alliance!");
				} else {
					target->sendSystemMessage("The Faction Scanner has detected your Covert affiliation with the Empire!");
				}

				playerObject->updateLastPvpCombatActionTimestamp(false, false, true, false);
			} else {

				target->sendSystemMessage("The Faction Scanner has failed to detect your Covert Faction status.");
			}
			scannerData->updateScanCooldown();
		}
	}

	bool checkTarget(CreatureObject* creature,  TangibleObject* scanner) {

		if (creature == nullptr || scanner == nullptr) {
			return false;
		}

		ScannerDataComponent* scannerData = cast<ScannerDataComponent*>(scanner->getDataObjectComponent()->get());

		if (scannerData == nullptr) {
			return false;
		}

		if (/*!scannerData->canScan() || */!scanner->isInRange(creature, scannerData->getCovertScannerRadius())) {
			return false;
		}

		if (!creature->isPlayerCreature()) {
			return false;
		}

		if (creature->isIncapacitated() || creature->isDead() || !creature->isOnline()) {
			return false;
		}

		if (!CollisionManager::checkLineOfSight(creature, scanner)) {
			return false;
		}

		ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();

		if (playerObject == nullptr) {
			return false;
		}

		if (playerObject->isPrivileged() || scanner->getFaction() == creature->getFaction() || creature->getFaction() == 0 || creature->getFactionStatus() != FactionStatus::COVERT) {
			return false;
		}

		return true;
	}
};

#endif /* SCANNERSCANTASK_H_ */
