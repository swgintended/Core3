#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/events/EnemyFlagRemovalTask.h"
#include "PvPManager.h"

void PvPManager::initializeEnemyFlags(PlayerObject* ghost) const {
	ManagedWeakReference<CreatureObject*> weakPlayerRef = ghost->getParent().staticCastToWeak<CreatureObject*>();
	ghost->getEnemyFlags()->setCallbacks([&](EnemyFlag* flag) -> void { onEnemyFlagAdded(weakPlayerRef.get(), flag); },
										 [&](EnemyFlag* flag, uint32 secondsDelta) -> void { onEnemyFlagChanged(weakPlayerRef.get(), flag, secondsDelta); },
										 [&](int previousSize, int currentSize) -> void { onEnemyFlagRemoved(weakPlayerRef.get(), previousSize, currentSize); });
}

void PvPManager::onEnemyFlagAdded(ManagedReference<CreatureObject*> player, EnemyFlag* flag) const {
	if (player == nullptr) {
		return;
	}
	player->sendSystemMessage("Enemy flag added");

	if (flag->isTemporary()) {
		ManagedReference<PlayerObject*> ghost = player->getPlayerObject();
		if (ghost == nullptr) {
			return;
		}

		updateTEFPvPStatus(player, ghost);
		scheduleEnemyFlagRemoval(player);
	}
}

void PvPManager::onEnemyFlagChanged(ManagedReference<CreatureObject*> player, EnemyFlag* flag, uint32 secondsDelta) const {
	if (player == nullptr) {
		return;
	}
	player->sendSystemMessage("Enemy flag changed");

	if (secondsDelta == 0) {
		ManagedReference<PlayerObject*> ghost = player->getPlayerObject();
		if (ghost == nullptr) {
			return;
		}

		// Flag was changed from temporary to persistent. Cancel the removal task
		// if needed and update TEF status.
		checkAndCancelRemovalTask(player, ghost);
		updateTEFPvPStatus(player, ghost);
	}
}

void PvPManager::onEnemyFlagRemoved(ManagedReference<CreatureObject*> player, int previousSize, int currentSize) const {
	if (player == nullptr) {
		return;
	}
	player->sendSystemMessage("Enemy flag removed");

	ManagedReference<PlayerObject*> ghost = player->getPlayerObject();
	if (ghost == nullptr) {
		return;
	}

	// Cancel the flag removal task if there are no more TEF flags
	checkAndCancelRemovalTask(player, ghost);
	updateTEFPvPStatus(player, ghost);
}

void PvPManager::scheduleEnemyFlagRemoval(CreatureObject* player) const {
	ManagedReference<PlayerObject*> ghost = player->getPlayerObject();
	if (ghost == nullptr) {
		return;
	}

	EnemyFlags* flags = ghost->getEnemyFlags();
	Locker locker(flags);
	uint32 smallestExpiration = UINT32_MAX;
	for (int i = 0; i < flags->size(); i++) {
		const EnemyFlag* flag = flags->get(i);
		if (flag->isTemporary() && flag->getExpiration() < smallestExpiration) {
			smallestExpiration = flag->getExpiration();
		}
	}
	locker.release();

	if (player->containsPendingTask("enemy_flag_removal")) {
		player->removePendingTask("enemy_flag_removal");
	}

	player->sendSystemMessage("Scheduling flag removal in " + String::valueOf(smallestExpiration) + " seconds");
	Reference<EnemyFlagRemovalTask*> task = new EnemyFlagRemovalTask(player);
	// tasks are scheduled in milliseconds
	player->addPendingTask("enemy_flag_removal", task, smallestExpiration * 1000);
}

// Private methods

void PvPManager::checkAndCancelRemovalTask(CreatureObject* player, PlayerObject* ghost) const {
	if (player->containsPendingTask("enemy_flag_removal") && !ghost->getEnemyFlags()->hasTEF()) {
		player->sendSystemMessage("Cancelling flag removal task");
		player->removePendingTask("enemy_flag_removal");
	}
}

void PvPManager::updateTEFPvPStatus(CreatureObject* player, PlayerObject* ghost) const {
	bool playerHasTEF = player->getPvpStatusBitmask() & CreatureFlag::TEF;
	bool needsTEF = ghost->getEnemyFlags()->hasTEF();
	if (playerHasTEF && !needsTEF) {
		player->sendSystemMessage("Removing TEF");
		player->clearPvpStatusBit(CreatureFlag::TEF);
	} else if (!playerHasTEF && needsTEF) {
		player->sendSystemMessage("Adding TEF");
		player->setPvpStatusBit(CreatureFlag::TEF);
	}
}
