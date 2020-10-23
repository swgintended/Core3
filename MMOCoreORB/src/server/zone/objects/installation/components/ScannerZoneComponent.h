/*
 * ScannerZoneComponent.h
 *
 *  Created on: Dec 17, 2012
 *      Author: root
 */

#ifndef SCANNERZONECOMPONENT_H_
#define SCANNERZONECOMPONENT_H_

#include "engine/engine.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/QuadTreeEntry.h"


class ScannerZoneComponent : public ZoneComponent {

public:

	void notifyInsertToZone(SceneObject* sceneObject, Zone* zne) const;
	void notifyPositionUpdate(SceneObject* sceneObject, QuadTreeEntry* entry) const;
	void notifyInsert(SceneObject* sceneObject, QuadTreeEntry* entry) const;
	void notifyDissapear(SceneObject* sceneObject, QuadTreeEntry* entry) const;

};

#endif /* SCANNERZONECOMPONENT_H_ */
