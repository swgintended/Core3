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
#include "server/zone/QuadTreeEntry.h"


class ScannerDataComponent : public DataObjectComponent, public Logger {
protected:
	Time nextScanTime;
	SharedInstallationObjectTemplate* templateData;
	AtomicInteger numberOfPlayersInRange;

public:
	ScannerDataComponent()  {
		nextScanTime = Time();
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
	bool canScan();

	void updateScanCooldown(float cooldownSeconds);

	int getCovertScannerRadius();
	int getCovertScannerDelay();
	int getCovertScannerRevealChance();

	Time getNextScanTime() {
		return nextScanTime;
	}

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
