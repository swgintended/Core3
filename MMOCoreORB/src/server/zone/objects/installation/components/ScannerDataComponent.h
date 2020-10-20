/*
 * ScannerDataComponent.h
 *
 *  Created on: Nov 24, 2012
 *      Author: root
 */

#ifndef SCANNERDATACOMPONENT_H_
#define SCANNERDATACOMPONENT_H_

#include "engine/engine.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/scene/components/DataObjectComponent.h"
#include "templates/installation/SharedInstallationObjectTemplate.h"


class ScannerDataComponent : public DataObjectComponent, public Logger {
protected:
	Time nextScanTime;
	ManagedWeakReference<CreatureObject*> lastScanTarget;
	Reference<Task*> scannerScanTask;
	SharedInstallationObjectTemplate* templateData;
	AtomicInteger numberOfPlayersInRange;

public:
	ScannerDataComponent()  {
		nextScanTime = Time();
		templateData = nullptr;
		this->setLoggingName("ScannerData");

	}

	void writeJSON(nlohmann::json& j) const {
		DataObjectComponent::writeJSON(j);

		SERIALIZE_JSON_MEMBER(nextScanTime);
		SERIALIZE_JSON_MEMBER(numberOfPlayersInRange);
	}

	void initializeTransientMembers();

	int getRescheduleDelay() {
		int delay = 0;

		if (nextScanTime.isFuture())
			delay = Time().miliDifference(nextScanTime);

		return delay;
	}

	bool checkTarget(CreatureObject* creature, TangibleObject* scanner);

	void updateScanCooldown(float secondsToAdd);

	bool isScannerData(){
		return true;
	}

	Vector<CreatureObject*> getAvailableTargets();
	CreatureObject* selectTarget();

	void scheduleScanTask(CreatureObject* target, TangibleObject* scanner, int delay = 0);
	void rescheduleScanTask();

	int getCovertScannerRadius();
	int getCovertScannerDelay();

	uint32 getNumberOfPlayersInRange() {
		return numberOfPlayersInRange.get();
	}

	uint32 incrementNumberOfPlayersInRange() {
		return numberOfPlayersInRange.increment();
	}

	uint32 decrementNumberOfPlayersInRange() {
		return numberOfPlayersInRange.decrement();
	}

	bool compareAndSetNumberOfPlayersInRange(uint32 oldVal, uint32 newVal) {
		return numberOfPlayersInRange.compareAndSet(oldVal, newVal);
	}

};


#endif /* SCANNERDATACOMPONENT_H_ */
