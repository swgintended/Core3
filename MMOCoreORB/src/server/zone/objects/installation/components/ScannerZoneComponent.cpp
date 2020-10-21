/*
 * ScannerZoneComponent.cpp
 *
 *  Created on: Dec 17, 2012
 *      Author: root
 */

#include "ScannerDataComponent.h"
#include "ScannerZoneComponent.h"

#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/installation/components/ScannerObserver.h"
#include "server/zone/objects/installation/InstallationObject.h"
#include "server/zone/Zone.h"


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

void ScannerZoneComponent::notifyInsert(SceneObject* sceneObject, QuadTreeEntry* entry) const {
	ManagedReference<SceneObject*> target = cast<SceneObject*>(entry);

	if (!sceneObject->isScanner() || target == nullptr || !target->isPlayerCreature())
		return;

	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(sceneObject);
	ScannerDataComponent* scannerData = cast <ScannerDataComponent*>(sceneObject->getDataObjectComponent()->get());
	CreatureObject* player = target.castTo<CreatureObject*>();

	if (scanner == nullptr || scannerData == nullptr || player == nullptr || player->isInvisible()) {
		return;
	}

	int newValue = (int) scannerData->incrementNumberOfPlayersInRange();

	if (newValue == 1) {
		Core::getTaskManager()->executeTask([=] () {
			Locker locker(scanner);

			ScannerDataComponent* data = cast<ScannerDataComponent*>(scanner->getDataObjectComponent()->get());

			if (data == nullptr) {
				return;
			}

			if (data) {
				data->scheduleScanTask(nullptr);
			}
		}, "ScheduleScannerScanTaskLambda");
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
