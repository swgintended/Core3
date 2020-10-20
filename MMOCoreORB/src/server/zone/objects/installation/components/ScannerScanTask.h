/*
 * ScannerScanTask.cpp
 *
 *  Created on: Oct 18, 2020
 *      Author: Hakry
 */

#ifndef SCANNERSCANTASK_H_
#define SCANNERSCANTASK_H_

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

	ScannerScanTask(CreatureObject* target, TangibleObject* scanner, int delay) {
		scannerObject = scanner;
	}

	void run() {

		ManagedReference<TangibleObject*> scanner = scannerObject.get();

		if (scanner == nullptr) {
			return;
		}

		DataObjectComponentReference* ref = scanner->getDataObjectComponent();

		if (ref == nullptr) {
			return;
		}

		ScannerDataComponent* scannerData = cast<ScannerDataComponent*>(ref->get());

		if (scannerData == nullptr)
			return;

		Locker lock(scanner);

		ManagedReference<CreatureObject*> target = scannerData->selectTarget();

		if (checkTarget(scanner, target)) {

			PlayClientEffectLoc* explodeLoc = new PlayClientEffectLoc("clienteffect/survey_effect.cef", scanner->getZone()->getZoneName(), scanner->getPositionX(), scanner->getPositionZ(), scanner->getPositionY());
			scanner->broadcastMessage(explodeLoc, false);


			ManagedReference<PlayerObject*> playerObject = target->getPlayerObject();

			if (playerObject == nullptr) {
				return;
			}

			int revealChance = 90;

			if (System::random(100) < revealChance)  {
				scannerData->rescheduleScanTask();

				if (target->getFaction() == Factions::FACTIONREBEL) {
					target->sendSystemMessage("The Faction Scanner has revealed you as a Covert Member of Rebel Alliance!");
				} else {
					target->sendSystemMessage("The Faction Scanner has detected your Covert affiliation with the Empire!");
				}

				playerObject->updateLastPvpCombatActionTimestamp(false, false, true, false);
			} else {

				target->sendSystemMessage("The Faction Scanner has failed to detect your Covert Faction status.");
			}
		}
	}

	bool checkTarget(TangibleObject* scanner, CreatureObject* player) {
		if (scanner == nullptr || player == nullptr)
			return false;

		ScannerDataComponent* scannerData = cast<ScannerDataComponent*>(scanner->getDataObjectComponent()->get());

		if (!CollisionManager::checkLineOfSight(player, scanner) || !scanner->isInRange(player, scannerData->getCovertScannerRadius())) {
			return false;
		}

		if (!player->isPlayerCreature() || !scanner->isScanner()) {
			return false;
		}

		if (player->isIncapacitated() || player->isDead()) {
			return false;
		}

		ManagedReference<PlayerObject*> playerObject = player->getPlayerObject();

		if (playerObject == nullptr) {
			return false;
		}

		if (playerObject->isPrivileged() || scanner->getFaction() == player->getFaction() || player->getFaction() == 0 || player->getFactionStatus() != FactionStatus::COVERT) {
			return false;
		}

		return true;
	}
};

#endif /* SCANNERSCANTASK_H_ */
