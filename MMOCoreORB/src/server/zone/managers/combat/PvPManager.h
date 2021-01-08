#ifndef PVPMANAGER_H_
#define PVPMANAGER_H_

#include "engine/util/Singleton.h"
#include "engine/log/Logger.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/pvp/EnemyFlag.h"
#include "server/zone/objects/player/pvp/EnemyFlags.h"
#include "system/lang.h"

class PvPManager : public Singleton<PvPManager>, public Logger, public Object {

	public:
		PvPManager() {
			setLoggingName("PvPManager");
		}

		void initializeEnemyFlags(PlayerObject* ghost) const;
		void onEnemyFlagAdded(ManagedReference<CreatureObject*> player, EnemyFlag* flag) const;
		void onEnemyFlagChanged(ManagedReference<CreatureObject*> player, EnemyFlag* flag, uint32 secondsDelta) const;
		void onEnemyFlagRemoved(ManagedReference<CreatureObject*> player, int previousSize, int currentSize) const;
		void scheduleEnemyFlagRemoval(CreatureObject* player) const;

	private:
		void checkAndCancelRemovalTask(CreatureObject* player, PlayerObject* ghost) const;
		void updateTEFPvPStatus(CreatureObject* player, PlayerObject* ghost) const;
};

#endif /* PVPMANAGER_H_ */
