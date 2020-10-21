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
		nextScanTime = Time(0);
		templateData = nullptr;
		this->setLoggingName("ScannerData");

	}

	~ScannerDataComponent() {

	}

	void writeJSON(nlohmann::json& j) const {
		DataObjectComponent::writeJSON(j);

		if (templateData) {
			j["templateData"] = templateData->getTemplateFileName();
		} else {
			j["templateData"] = "";
		}

		SERIALIZE_JSON_MEMBER(nextScanTime);
		SERIALIZE_JSON_MEMBER(numberOfPlayersInRange);
	}

	void initializeTransientMembers();

	bool isScannerData(){
		return true;
	}

	Task* getScanTask() {
		return scannerScanTask;
	}

	bool canScan();
	bool checkTarget(CreatureObject* creature, TangibleObject* scanner);

	void updateScanCooldown();

	Vector<CreatureObject*> getAvailableTargets();
	CreatureObject* selectTarget();

	void scheduleScanTask(CreatureObject* target);

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
