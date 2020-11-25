#ifndef ENEMYFLAGS_H_
#define ENEMYFLAGS_H_

#include "system/util/VectorSet.h"
#include "system/lang.h"
#include "EnemyFlag.h"

class EnemyFlags : public VectorSet<EnemyFlag>, public ReadWriteLock {
	public:
		EnemyFlags(): ReadWriteLock() {
			setAllowOverwriteInsertPlan();
		}

		inline bool hasEnemy(uint64 enemyID, uint32 faction) {
			return getEnemyIndex(enemyID, faction) != this->npos;
		}

		inline EnemyFlag* getEnemy(uint64 enemyID, uint32 faction) {
			int index = getEnemyIndex(enemyID, faction);
			if (index == this->npos) {
				return nullptr;
			}

			return &get(index);
		}

		inline void addEnemy(uint64 enemyID, uint32 faction) {
			addEnemy(enemyID, faction, 0);
		}

		inline void addEnemy(uint64 enemyID, uint32 faction, uint64 duration) {
			removeEnemy(enemyID, faction); // Remove previous flag if it exists
			add(EnemyFlag(enemyID, faction, duration));
		}

		inline void removeEnemy(uint64 enemyID) {
			Locker locker(this);
			for (int i = size(); i >= 0; i--) {
				EnemyFlag* flag = &get(i);
				if (flag->getEnemyID() == enemyID) {
					remove(i);
				}
			}

			locker.release();
		}

		inline void removeEnemy(uint64 enemyID, uint32 faction) {
			int index = getEnemyIndex(enemyID, faction);
			if (index == this->npos) {
				return;
			}

			remove(index);
		}

	private:

		inline int getEnemyIndex(uint64 enemyID, uint32 faction) {
			Locker locker(this);
			for (int i = 0; i < size(); i++) {
				const EnemyFlag* flag = &get(i);
				if (flag->getEnemyID() == enemyID && flag->getFaction() == faction) {
					return i;
				}
			}

			locker.release();
			return this->npos;
		}
};

#endif /*ENEMYFLAGS_H_*/
