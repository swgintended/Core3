#ifndef ENEMYFLAGS_H_
#define ENEMYFLAGS_H_

#include "system/util/VectorSet.h"
#include "system/lang.h"
#include "EnemyFlag.h"

class EnemyFlags : public Serializable, public ReadWriteLock {
	public:
		EnemyFlags(): Serializable(), ReadWriteLock() {
			addSerializableVariables();
			flags.setAllowOverwriteInsertPlan();
		}

		EnemyFlags(const EnemyFlags& copy): EnemyFlags() {
			copyFrom(copy);
		}

	#ifdef CXX11_COMPILER
		EnemyFlags(const EnemyFlags&& copy): EnemyFlags() {
			copyFrom(copy);
		}
	#endif

		EnemyFlags& operator=(const EnemyFlags& copy) {
			if (this == &copy) {
				return *this;
			}

			copyFrom(copy);
			return *this;
		}

	#ifdef CXX11_COMPILER
		EnemyFlags& operator=(const EnemyFlags&& copy) {
			if (this == &copy) {
				return *this;
			}

			copyFrom(copy);
			return *this;
		}
	#endif

		friend void to_json(nlohmann::json& j, const EnemyFlags& flags) {
			j["flags"] = flags.flags;
		}

		inline bool hasEnemy(uint64 enemyID, uint32 faction) const {
			return getEnemyIndex(enemyID, faction) != flags.npos;
		}

		int size() const {
			return flags.size();
		}

		inline EnemyFlag* get(int index) {
			return &flags.get(index);
		}
		inline const EnemyFlag* get(int index) const {
			return get(index);
		}

		inline EnemyFlag* getEnemy(uint64 enemyID, uint32 faction) {
			int index = getEnemyIndex(enemyID, faction);
			if (index == flags.npos) {
				return nullptr;
			}

			return get(index);
		}
		inline EnemyFlag* getEnemy(uint64 enemyID, uint32 faction) const {
			return getEnemy(enemyID, faction);
		}

		inline void addEnemy(uint64 enemyID, uint32 faction) {
			addEnemy(enemyID, faction, 0);
		}

		inline void addEnemy(uint64 enemyID, uint32 faction, uint64 duration) {
			// TODO: try deleting remove line with allowoverwriteinsertplan active
			removeEnemy(enemyID, faction); // Remove previous flag if it exists
			EnemyFlag flag(enemyID, faction, duration);
			flag.setCallbacks(notifyChanged);
			flags.add(flag);
			notifyAdded(&flag);
		}

		inline void remove(int index) {
			int previousSize = size();
			flags.remove(index);
			notifyRemoved(previousSize, size());
		}

		inline void removeEnemy(uint64 enemyID) {
			Locker locker(this);
			int previousSize = size();
			for (int i = size(); i >= 0; i--) {
				EnemyFlag* flag = get(i);
				if (flag->getEnemyID() == enemyID) {
					remove(i);
				}
			}

			int currentSize = size();
			if (previousSize != currentSize) {
				notifyRemoved(previousSize, currentSize);
			}

			locker.release();
		}

		inline void removeEnemy(uint64 enemyID, uint32 faction) {
			int index = getEnemyIndex(enemyID, faction);
			if (index == flags.npos) {
				return;
			}

			int previousSize = size();
			remove(index);
			notifyRemoved(previousSize, size());
		}

		inline void removeAll() {
			int previousSize = size();
			if (previousSize == 0) {
				return;
			}

			flags.removeAll();
			notifyRemoved(previousSize, size());
		}

		inline void setCallbacks(Function<void(EnemyFlag*)> notifyAdded, Function<void(EnemyFlag*, uint32)> notifyChanged, Function<void(int, int)> notifyRemoved) {
			this->notifyAdded = notifyAdded;
			this->notifyChanged = notifyChanged;
			this->notifyRemoved = notifyRemoved;
			Locker locker(this);
			for (int i = 0; i < size(); i++) {
				EnemyFlag* flag = get(i);
				flag->setCallbacks(notifyChanged);
			}

			locker.release();
		}

		inline bool hasTEF() {
			bool hasTEF = false;
			Locker locker(this);
			for (int i = 0; i < size(); i++) {
				const EnemyFlag* flag = get(i);
				if (flag->isTemporary() && !flag->isExpired()) {
					hasTEF = true;
					break;
				}
			}

			locker.release();
			return hasTEF;
		}

		inline bool removeExpiredFlags() {
			Locker locker(this);
			int previousSize = size();
			for (int i = size() - 1; i >= 0; i--) {
				const EnemyFlag* flag = get(i);
				if (flag->isExpired()) {
					remove(i);
				}
			}

			locker.release();
			int currentSize = size();
			if (previousSize != currentSize) {
				notifyRemoved(previousSize, currentSize);
				return true;
			} else {
				return false;
			}
		}

	private:
		VectorSet<EnemyFlag> flags;
		Function<void(EnemyFlag*)> notifyAdded = [](EnemyFlag* added){};
		Function<void(EnemyFlag*, uint32)> notifyChanged = [](EnemyFlag* changed, uint32 secondsDelta){};
		Function<void(int, int)> notifyRemoved = [](int previousSize, int currentSize){};

		inline void addSerializableVariables() {
			addSerializableVariable("flags", &flags);
		}

		inline void copyFrom(const EnemyFlags& copy) {
			flags = copy.flags;
			notifyAdded = copy.notifyAdded;
			notifyChanged = copy.notifyChanged;
			notifyRemoved = copy.notifyRemoved;
		}

		inline int getEnemyIndex(uint64 enemyID, uint32 faction) {
			Locker locker(this);
			for (int i = 0; i < size(); i++) {
				const EnemyFlag* flag = get(i);
				if (flag->getEnemyID() == enemyID && flag->getFaction() == faction) {
					return i;
				}
			}

			locker.release();
			return flags.npos;
		}

		inline int getEnemyIndex(uint64 enemyID, uint32 faction) const {
			return getEnemyIndex(enemyID, faction);
		}
};

#endif /*ENEMYFLAGS_H_*/
