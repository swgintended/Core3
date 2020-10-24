/*
 * ScannerZoneComponent.cpp
 *
 *  Created on: Dec 17, 2012
 *      Author: root
 */

#include "ScannerDataComponent.h"
#include "ScannerZoneComponent.h"

#include "server/zone/managers/collision/CollisionManager.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/installation/components/ScannerObserver.h"
#include "server/zone/objects/installation/InstallationObject.h"
#include "server/zone/objects/player/FactionStatus.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/packets/scene/PlayClientEffectLocMessage.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/Zone.h"
#include "templates/faction/Factions.h"

void ScannerZoneComponent::notifyInsertToZone(SceneObject* sceneObject, Zone* zne) const {
	if (zne == nullptr) {
		return;
	}

	ManagedReference<InstallationObject*> installation = cast<InstallationObject*>(sceneObject);

	if (installation == nullptr) {
		return;
	}

	SortedVector<ManagedReference<Observer*> > destructionObservers = installation->getObservers(ObserverEventType::OBJECTDESTRUCTION);

	for (int i = 0; i < destructionObservers.size(); i++) {
		ScannerObserver* scannerObserver = destructionObservers.get(i).castTo<ScannerObserver*>();

		if (scannerObserver != nullptr) {
			return;
		}
	}

	ManagedReference<ScannerObserver*> observer = new ScannerObserver();

	installation->registerObserver(ObserverEventType::OBJECTDESTRUCTION, observer);
}

void ScannerZoneComponent::notifyPositionUpdate(SceneObject* sceneObject, QuadTreeEntry* entry) const {

	ManagedReference<SceneObject*> target = cast<SceneObject*>(entry);

	if (!sceneObject->isTangibleObject() || target == nullptr) {
		return;
	}

	ManagedReference<TangibleObject*> tano = cast<TangibleObject*>(sceneObject);

	if (tano == nullptr) {
		return;
	}

	DataObjectComponentReference* ref = sceneObject->getDataObjectComponent();

	if (ref == nullptr) {
		info("data is null",true);
		return;
	}

	ScannerDataComponent* scannerData = cast<ScannerDataComponent*>(ref->get());

	if (scannerData == nullptr) {
		return;
	}

	float radius = scannerData->getCovertScannerRadius() * 1.f;

	if ((scannerData->canScan() > 0) || !sceneObject->isScanner() || !target->isPlayerCreature() || !sceneObject->isInRange(target, radius)) {
		return;
	}

	if (!CollisionManager::checkLineOfSight(target, sceneObject)) {
		return;
	}

	ManagedReference<CreatureObject*> creature = cast<CreatureObject*>(entry);

	if (creature == nullptr || creature->isIncapacitated() || creature->isDead() || !creature->isOnline()) {
		return;
	}

	ManagedReference<PlayerObject*> playerObject = creature->getPlayerObject();

	if (playerObject == nullptr) {
		return ;
	}

	if (playerObject->isPrivileged() || playerObject->hasRealGcwTef()) {
		return;
	}

	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(sceneObject);

	if ((scanner->getFaction() != creature->getFaction() || creature->getFaction() != 0) && creature->getFactionStatus() == FactionStatus::COVERT) {

		PlayClientEffectLoc* explodeLoc = new PlayClientEffectLoc("clienteffect/survey_effect.cef", tano->getZone()->getZoneName(), tano->getPositionX(), tano->getPositionZ(), tano->getPositionY());
		tano->broadcastMessage(explodeLoc, false);

		int revealChance = scannerData->getCovertScannerRevealChance();

		if (System::random(100) < revealChance)  {

			scannerData->updateScanCooldown(scannerData->getCovertScannerDelay());

			if (creature->getFaction() == Factions::FACTIONREBEL) {
				creature->sendSystemMessage("The Faction Scanner has revealed you as a Covert Member of Rebel Alliance!");
			} else {
				creature->sendSystemMessage("The Faction Scanner has detected your Covert affiliation with the Empire!");
			}
			playerObject->updateLastPvpCombatActionTimestamp(false, false, true, false);
		} else {
			creature->sendSystemMessage("The Faction Scanner has failed to detect your Covert Faction status.");
		}
	}
}

void ScannerZoneComponent::notifyInsert(SceneObject* sceneObject, QuadTreeEntry* entry) const {
	ManagedReference<SceneObject*> target = cast<SceneObject*>(entry);

	if (!sceneObject->isScanner() || target == nullptr || !target->isPlayerCreature())
		return;

	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(sceneObject);
	ScannerDataComponent* scannerData = cast <ScannerDataComponent*>(sceneObject->getDataObjectComponent()->get());
	CreatureObject* player = target.castTo<CreatureObject*>();

	if (scanner == nullptr || scannerData == nullptr || player == nullptr || player->isInvisible()) {
		player->sendSystemMessage("scanner, data or player are null");
		return;
	}

	int newValue = (int) scannerData->incrementNumberOfPlayersInRange();

	if (newValue == 1) {

			ScannerDataComponent* data = cast<ScannerDataComponent*>(scanner->getDataObjectComponent()->get());

			if (data == nullptr) {
				player->sendSystemMessage("Data is null");
				return;
			}
	}
}

void ScannerZoneComponent::notifyDissapear(SceneObject* sceneObject, QuadTreeEntry* entry) const {
	ManagedReference<SceneObject*> target = cast<SceneObject*>(entry);

	if (!sceneObject->isScanner() || target == nullptr || !target->isPlayerCreature())
		return;

	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(sceneObject);
	ScannerDataComponent* scannerData = cast<ScannerDataComponent*>(sceneObject->getDataObjectComponent()->get());
	CreatureObject* player = target.castTo<CreatureObject*>();

	if (scanner == nullptr || scannerData == nullptr || player == nullptr || player->isInvisible())
		return;

	int32 newValue = (int32) scannerData->decrementNumberOfPlayersInRange();

	if (newValue < 0) {
		int oldValue;

		do {
			oldValue = (int)scannerData->getNumberOfPlayersInRange();

			newValue = oldValue;

			if (newValue < 0)
				newValue = 0;
		} while (!scannerData->compareAndSetNumberOfPlayersInRange((uint32)oldValue, (uint32)newValue));
	}
}
