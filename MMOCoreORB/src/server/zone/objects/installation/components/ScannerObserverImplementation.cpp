/*
 * ScannerObserverImplementation.cpp
 *
 *  Created on: Oct 16, 2020
 *      Author: Hakry
 */

#include "server/zone/objects/installation/components/ScannerObserver.h"
#include "server/zone/managers/gcw/tasks/DestroyFactionInstallationTask.h"
#include "server/zone/objects/installation/InstallationObject.h"


int ScannerObserverImplementation::notifyObserverEvent(uint32 eventType, Observable* observable, ManagedObject* arg1, int64 arg2) {

	if (eventType == ObserverEventType::OBJECTDESTRUCTION) {
		ManagedReference<InstallationObject*> installation = cast<InstallationObject*>(observable);

		if (installation != nullptr) {
			DestroyFactionInstallationTask* task = new DestroyFactionInstallationTask(installation);
			task->execute();
		}

		return 1;
	}

	return 0;
}


