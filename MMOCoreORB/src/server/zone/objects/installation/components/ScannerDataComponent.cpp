/*
 * ScannerDataComponent.cpp
 *
 *  Created on: Oct. 18, 2020
 *      Author: Hakry
 */

#include "ScannerDataComponent.h"

#include "server/zone/managers/collision/CollisionManager.h"
#include "server/zone/managers/gcw/GCWManager.h"
#include "server/zone/objects/building/BuildingObject.h"
#include "server/zone/objects/installation/components/ScannerObserver.h"
#include "server/zone/objects/installation/components/ScannerScanTask.h"
#include "server/zone/objects/installation/InstallationObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/FactionStatus.h"
#include "server/zone/objects/scene/SceneObject.h"

#include "server/zone/Zone.h"

Vector<CreatureObject*> ScannerDataComponent::getAvailableTargets() {
	Vector<CreatureObject*> targets;

	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(getParent());

	if (scanner == nullptr) {
		return targets;
	}

	CloseObjectsVector* vec = (CloseObjectsVector*)scanner->getCloseObjects();

	SortedVector<QuadTreeEntry*> closeObjects;

	vec->safeCopyReceiversTo(closeObjects, CloseObjectsVector::CREOTYPE);

	int targetTotal = 0;

	for (int i = 0; i < closeObjects.size(); ++i) {
		CreatureObject* creo = cast<CreatureObject*>(closeObjects.get(i));

		if (creo != nullptr && checkTarget(creo, scanner)) {
			targets.add(creo);
		}
	}

	return targets;
}

CreatureObject* ScannerDataComponent::selectTarget() {
	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(getParent());

	if (scanner == nullptr)
		return nullptr;

	ManagedReference<CreatureObject*> lastTarget = lastScanTarget.get();

	if (lastTarget == nullptr || !checkTarget(lastTarget, scanner)) {
		lastScanTarget = nullptr;

		Vector<CreatureObject*> targets = getAvailableTargets();

		if (targets.size() == 0)
			return nullptr;

		lastTarget = targets.get(System::random(targets.size() - 1));
		lastScanTarget = lastTarget;
	}

	return lastTarget;
}

bool ScannerDataComponent::checkTarget(CreatureObject* creature,  TangibleObject* scanner) {

	if (creature == nullptr || scanner == nullptr)
		return false;

	if (!CollisionManager::checkLineOfSight(creature, scanner) || !scanner->isInRange(creature, getCovertScannerRadius())) {
		return false;
	}

	if (!creature->isPlayerCreature() || !scanner->isScanner()) {
		return false;
	}

	if (creature->isIncapacitated() || creature->isDead()) {
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

void ScannerDataComponent::updateScanCooldown(float secondsToAdd) {
	int milisecondsToAdd = secondsToAdd*1000;
	nextScanTime = Time();
	nextScanTime.addMiliTime(milisecondsToAdd);
}


int ScannerDataComponent::getCovertScannerRadius() {
	int radius = 5;

	ManagedReference<InstallationObject*> scanner = cast<InstallationObject*>(getParent());

	if (scanner != nullptr) {
		Zone* zone = scanner->getZone();

		if (zone != nullptr) {
			GCWManager* gcwMan = zone->getGCWManager();

			if (gcwMan != nullptr)
				radius = gcwMan->getCovertScannerRadius();
		}
	}

	return radius;
}

int ScannerDataComponent::getCovertScannerDelay() {
	int delay = 15;

	ManagedReference<InstallationObject*> scanner = cast<InstallationObject*>(getParent());

	if (scanner != nullptr) {
		Zone* zone = scanner->getZone();

		if (zone != nullptr) {
			GCWManager* gcwMan = zone->getGCWManager();

			if (gcwMan != nullptr)
				delay = gcwMan->getCovertScannerDelay();
		}
	}

	return delay;
}

void ScannerDataComponent::scheduleScanTask(CreatureObject* target, TangibleObject* scanner, int delay) {

	if (numberOfPlayersInRange.get() < 1)
		return;

	//ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(getParent());

	if (scanner->getZone() == nullptr)
		return;

	if (scannerScanTask == nullptr) {
		scannerScanTask = new ScannerScanTask(target, scanner, delay);
	}

	ScannerScanTask* task = scannerScanTask.castTo<ScannerScanTask*>();

	if (task == nullptr)
		return;

	if (target == nullptr) {
		delay += getRescheduleDelay();
	}

	if (!task->isScheduled()) {
		if (delay == 0)
			task->execute();
	} else {
			task->schedule(delay);
	}
}

void ScannerDataComponent::rescheduleScanTask() {

	if (numberOfPlayersInRange.get() < 1) {
		return;
	}

	ManagedReference<TangibleObject*> scanner = cast<TangibleObject*>(getParent());

	if (scanner == nullptr || scanner->getZone() == nullptr) {
		return;
	}

	ScannerScanTask* task = scannerScanTask.castTo<ScannerScanTask*>();

	if (task == nullptr) {
		return;
	}

	task->reschedule(getRescheduleDelay());
}

