#ifndef ENEMYFLAGS_H_
#define ENEMYFLAGS_H_

#include "system/util/VectorMap.h"
#include "system/util/VectorSet.h"
#include "system/lang.h"
#include "EnemyFlag.h"

class EnemyFlags : public Serializable, public ReadWriteLock {
	public:
		EnemyFlags(): Serializable(), ReadWriteLock(), flags() {
			addSerializableVariables();
		}

		friend void to_json(nlohmann::json& j, const EnemyFlags& enemyFlags) {
			auto array = nlohmann::json::array();

			for (int i = 0; i < enemyFlags.flags.size(); i++) {
				nlohmann::json obj = nlohmann::json::object();
				const EnemyFlag flag = enemyFlags.flags.get(i);
				EnemyFlag:to_json(obj, flag);
				array.push_back(obj);
			}

			j["flags"] = array;
		}

		inline bool hasEnemy(uint64 enemyID, uint32 faction) {
			return getEnemyIndex(enemyID, faction) != flags.npos;
		}

		inline EnemyFlag* getEnemy(uint64 enemyID, uint32 faction) {
			int index = getEnemyIndex(enemyID, faction);
			if (index == flags.npos) {
				return nullptr;
			}

			return &flags.get(index);
		}

		inline void addEnemy(uint64 enemyID, uint32 faction) {
			addEnemy(enemyID, faction, 0);
		}

		inline void addEnemy(uint64 enemyID, uint32 faction, uint64 duration) {
			removeEnemy(enemyID, faction); // Remove previous flag if it exists
			flags.add(EnemyFlag(enemyID, faction, duration));
		}

		inline void removeEnemy(uint64 enemyID) {
			Locker locker(this);
			for (int i = flags.size(); i >= 0; i--) {
				EnemyFlag* flag = &flags.get(i);
				if (flag->getEnemyID() == enemyID) {
					flags.remove(i);
				}
			}

			locker.release();
		}

		inline void removeEnemy(uint64 enemyID, uint32 faction) {
			int index = getEnemyIndex(enemyID, faction);
			if (index == flags.npos) {
				return;
			}

			flags.remove(index);
		}

		inline void removeAll() {
			flags.removeAll();
		}

	private:
		VectorSet<EnemyFlag> flags;

		inline void addSerializableVariables() {
			addSerializableVariable("flags", &flags);
		}

		inline int getEnemyIndex(uint64 enemyID, uint32 faction) {
			Locker locker(this);
			for (int i = 0; i < flags.size(); i++) {
				const EnemyFlag* flag = &flags.get(i);
				if (flag->getEnemyID() == enemyID && flag->getFaction() == faction) {
					return i;
				}
			}

			locker.release();
			return flags.npos;
		}
};

#endif /*ENEMYFLAGS_H_*/
