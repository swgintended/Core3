#ifndef ENEMYFLAGREMOVALTASK_H_
#define ENEMYFLAGREMOVALTASK_H_

#include "engine/engine.h"
#include "server/zone/managers/combat/PvPManager.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/pvp/EnemyFlags.h"

class EnemyFlagRemovalTask: public Task {

public:
	EnemyFlagRemovalTask(CreatureObject* player) {
		weakPlayerRef = player;
	}

	void run() {
		ManagedReference<CreatureObject*> player = weakPlayerRef.get();
		if (player == nullptr) {
			return;
		}

		ManagedReference<PlayerObject*> ghost = player->getPlayerObject();
		if (ghost == nullptr) {
			return;
		}

		EnemyFlags* flags = ghost->getEnemyFlags();
		flags->removeExpiredFlags();
		if (flags->hasTEF()) {
			// Schedule another task to remove the next temporary enemy flag
			PvPManager::instance()->scheduleEnemyFlagRemoval(player);
		}
	}

private:
	ManagedWeakReference<CreatureObject*> weakPlayerRef;

};

#endif /* ENEMYFLAGREMOVALTASK_H_ */
