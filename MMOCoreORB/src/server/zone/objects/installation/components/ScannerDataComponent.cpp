/*
 * ScannerDataComponent.cpp
 *
 *  Created on: Oct. 18, 2020
 *      Author: Hakry
 */

#include "ScannerDataComponent.h"

#include "server/zone/managers/gcw/GCWManager.h"
#include "server/zone/objects/installation/InstallationObject.h"
#include "server/zone/Zone.h"

void ScannerDataComponent::initializeTransientMembers() {
	ManagedReference<SceneObject*> scanner = getParent();

	if (scanner == nullptr) {
		return;
	}

	templateData = dynamic_cast<SharedInstallationObjectTemplate*>(scanner->getObjectTemplate());
}

void ScannerDataComponent::updateScanCooldown(float cooldownSeconds) {
	int miliNextScan = cooldownSeconds * 1000;
	nextScanTime = Time();
	nextScanTime.addMiliTime(miliNextScan);
}

bool ScannerDataComponent::canScan() {
	int delay = 0;

	if (nextScanTime.isFuture()) {
		delay = Time().miliDifference(nextScanTime);
	}
	return delay;
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
	int delay = 2000;

	ManagedReference<InstallationObject*> scanner = cast<InstallationObject*>(getParent());

	if (scanner != nullptr) {
		Zone* zone = scanner->getZone();

		if (zone != nullptr) {
			GCWManager* gcwMan = zone->getGCWManager();

			if (gcwMan != nullptr) {
				delay = gcwMan->getCovertScannerDelay();
			}
		}
	}
	return delay;
}

int ScannerDataComponent::getCovertScannerRevealChance() {
	int chance = 10;

	ManagedReference<InstallationObject*> scanner = cast<InstallationObject*>(getParent());

	if (scanner != nullptr) {
		Zone* zone = scanner->getZone();

		if (zone != nullptr) {
			GCWManager* gcwMan = zone->getGCWManager();

			if (gcwMan != nullptr) {
				chance = gcwMan->getCovertScannerRevealChance();
			}
		}
	}
	return chance;
}
