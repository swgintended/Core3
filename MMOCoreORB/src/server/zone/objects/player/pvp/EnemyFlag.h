#ifndef ENEMYFLAG_H_
#define ENEMYFLAG_H_

#include "system/lang.h"

class EnemyFlag : public Serializable {
public:
	static const uint32 TEFTIMER = 300;

	EnemyFlag() : EnemyFlag(0, 0, 0) {}
	EnemyFlag(const uint64 enemyID, const uint32 faction) : EnemyFlag(enemyID, faction, 0) {}
	EnemyFlag(const uint64 enemyID, const uint32 faction, const uint32 durationInSeconds) : Serializable(), enemyID(enemyID), faction(faction) {
		if (durationInSeconds == 0) {
			// Persistent flag
			setPersistent();
		} else {
			// Temporary flag
			updateExpiration(durationInSeconds);
		}

		addSerializableVariables();
	}

	EnemyFlag(const EnemyFlag& copy): Object(), Serializable() {
		copyFrom(copy);
		addSerializableVariables();
	}

#ifdef CXX11_COMPILER
	EnemyFlag(const EnemyFlag&& copy): Serializable() {
		copyFrom(copy);
		addSerializableVariables();
	}
#endif

	EnemyFlag& operator=(const EnemyFlag& copy) {
		if (this == &copy) {
			return *this;
		}

		copyFrom(copy);
		return *this;
	}

#ifdef CXX11_COMPILER
	EnemyFlag& operator=(const EnemyFlag&& copy) {
		if (this == &copy) {
			return *this;
		}

		copyFrom(copy);
		return *this;
	}
#endif

	friend void to_json(nlohmann::json& j, const EnemyFlag& flag) {
		j["enemyID"] = flag.enemyID;
		j["faction"] = flag.faction;
		j["expiration"] = flag.expiration;
		j["temporary"] = flag.temporary;
	}

	inline uint64 getEnemyID() const {
		return enemyID;
	}

	inline uint32 getFaction() const {
		return faction;
	}

	inline bool isTemporary() const {
		return temporary;
	}

	inline bool isExpired() const {
		if (!isTemporary()) {
			return false;
		}

		return expiration.isPast();
	}

	inline uint32 getExpiration() const {
		if (!isTemporary()) {
			return 0;
		}

		return expiration.getTime();
	}

	inline void updateExpiration(const uint32 expiresInSecondsFromNow = TEFTIMER) {
		uint32 prevExpiration = getExpiration();
		temporary = true;
		expiration.updateToCurrentTime();
		expiration.addMiliTime(expiresInSecondsFromNow * 1000);
		notifyChanged(this, getExpiration() - prevExpiration);
	}

	inline void setPersistent() {
		temporary = false;
		notifyChanged(this, 0);
	}

	int compareTo(const EnemyFlag& other) const {
		if (enemyID != other.enemyID) {
			return enemyID - other.enemyID;
		}

		if (faction != other.faction) {
			// Arbitrarily sort faction by hash (could compare by name in the future)
			return faction - other.faction;
		}

		if (!temporary && !other.temporary) {
			return 0;
		} else if (!temporary) {
			// Arbitrarily sort persistent flags before TEFs
			return -1;
		} else {
			return 1;
		}

		// Sort TEFs by expiration
		return expiration.compareTo(other.expiration);
	}

	inline void setCallbacks(Function<void(EnemyFlag*, uint32)> notifyChanged) {
		this->notifyChanged = notifyChanged;
	}

private:
	uint64 enemyID;
	uint32 faction;
	SerializableTime expiration;
	bool temporary;
	Function<void(EnemyFlag*, uint32)> notifyChanged = [](EnemyFlag* flag, uint32 secondsDelta){};

	inline void addSerializableVariables() {
		addSerializableVariable("enemyID", &enemyID);
		addSerializableVariable("faction", &faction);
		addSerializableVariable("expiration", &expiration);
		addSerializableVariable("temporary", &temporary);
	}

	inline void copyFrom(const EnemyFlag& copy) {
		enemyID = copy.enemyID;
		faction = copy.faction;
		expiration = copy.expiration;
		temporary = copy.temporary;
		notifyChanged = copy.notifyChanged;
	}

	// GCW Scenario
	// ==========
	// Ariana = Covert Rebel
	// Joanna = Overt Rebel
	// Kreen = Overt Imperial
	//
	// Ariana and Joanna are grouped together.
	//
	// 1. Initial flags:
	//   Ariana (Covert Rebel):
	//   Joanna (Overt Rebel):
	//     EnemyFlag(0, IMPERIAL, 0);
	//   Kreen (Overt Imperial):
	//     EnemyFlag(0, REBEL, 0);
	//
	// Ariana sees Joanna (blue) and Kreen (blue).
	// Joanna sees Ariana (blue) and Kreen (red).
	// Kreen sees Ariana (blue) and Joanna (red).
	//
	// 2. Kreen attacks Joanna:
	//   Ariana (Covert Rebel):
	//   Joanna (Overt Rebel):
	//     EnemyFlag(0, IMPERIAL, 0);
	//   Kreen (Overt Imperial):
	//     EnemyFlag(0, REBEL, 0);
	//     EnemyFlag(Joanna->getGroupID(), REBEL, TEFTIMER);
	//
	// Ariana sees Joanna (blue) and Kreen (blue + attackable).
	// Joanna sees Ariana (blue) and Kreen (red).
	// Kreen sees Ariana (blue) and Joanna (red).
	//
	// 3. Ariana attacks Kreen using the group TEF and gains a GCW TEF:
	//   Ariana (Covert Rebel):
	//     EnemyFlag(0, IMPERIAL, TEFTIMER);
	//   Joanna (Overt Rebel):
	//     EnemyFlag(0, IMPERIAL, 0);
	//   Kreen (Overt Imperial):
	//     EnemyFlag(0, REBEL, 0);
	//     EnemyFlag(Joanna->getGroupID(), REBEL, TEFTIMER);
	//
	// Ariana sees Joanna (blue) and Kreen (red).
	// Joanna sees Ariana (blue) and Kreen (red).
	// Kreen sees Ariana (red) and Joanna (red).

	// Bounty Scenario
	// ==========
	// Ariana = Jedi
	// Joanna = Jedi group member
	// Kreen = Bounty Hunter
	// Deda = Bounty Hunter group member
	//
	// Ariana and Joanna are grouped together.
	// Kreen and Deda are grouped together.
	//
	// 1. Initial flags:
	//   Ariana (Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//   Joanna (Jedi group):
	//   Kreen (BH):
	//   Deda (BH group):
	//
	// Ariana sees Joanna (blue), Kreen (blue), and Deda (blue).
	// Joanna sees Ariana (blue), Kreen (blue), and Deda (blue).
	// Kreen sees Ariana (blue + attackable), Joanna (blue), and Deda (blue).
	// Deda sees Ariana (blue), Joanna (blue), and Kreen (blue).
	//
	// 2. Kreen engages Ariana for her Jedi bounty. Ariana has *not* attacked back:
	//   Ariana (Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, TEFTIMER);
	//   Joanna (Jedi group):
	//   Kreen (BH):
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Ariana->getGroupID(), BOUNTY, TEFTIMER);
	//   Deda (BH group):
	//
	// Ariana sees Joanna (blue), Kreen (red), and Deda (blue).
	// Joanna sees Ariana (blue), Kreen (blue + attackable), and Deda (blue).
	// Kreen sees Ariana (red), Joanna (blue), and Deda (blue).
	// Deda sees Ariana (blue), Joanna (blue), and Kreen (blue).
	//
	// 3. Joanna attacks Kreen using the bounty group TEF. Ariana has still *not* attacked back (she's slow):
	//   Ariana (Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, TEFTIMER);
	//   Joanna (Jedi group):
	//     EnemyFlag(Kreen->getGroupID(), BOUNTY, TEFTIMER);
	//   Kreen (BH):
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Ariana->getGroupID(), BOUNTY, TEFTIMER);
	//   Deda (BH group):
	//
	// Ariana sees Joanna (blue), Kreen (red), and Deda (blue).
	// Joanna sees Ariana (blue), Kreen (red), and Deda (blue).
	// Kreen sees Ariana (red), Joanna (red), and Deda (blue).
	// Deda sees Ariana (blue), Joanna (blue + attackable), and Kreen (blue). *
	//
	// * Note how with timing it's possible for BH group members to attack Jedi
	//   group members before being able to attack the Jedi themselves.
	//
	// 4. Ariana finally attacks Kreen:
	//   Ariana (Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Kreen->getGroupID(), BOUNTY, TEFTIMER);
	//   Joanna (Jedi group):
	//     EnemyFlag(Kreen->getGroupID(), BOUNTY, TEFTIMER);
	//   Kreen (BH):
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Ariana->getGroupID(), BOUNTY, TEFTIMER);
	//   Deda (BH group):
	//
	// Ariana sees Joanna (blue), Kreen (red), and Deda (blue).
	// Joanna sees Ariana (blue), Kreen (red), and Deda (blue).
	// Kreen sees Ariana (red), Joanna (red), and Deda (blue).
	// Deda sees Ariana (blue + attackable), Joanna (blue + attackable), and Kreen (blue).
	//
	// 5. Deda attacks Joanna (not going after the scary Jedi), and now both groups are red to each other:
	//   Ariana (Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Kreen->getGroupID(), BOUNTY, TEFTIMER);
	//   Joanna (Jedi group):
	//     EnemyFlag(Kreen->getGroupID(), BOUNTY, TEFTIMER);
	//   Kreen (BH):
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Ariana->getGroupID(), BOUNTY, TEFTIMER);
	//   Deda (BH group):
	//     EnemyFlag(Joanna->getGroupID(), BOUNTY, TEFTIMER);
	//
	// Ariana sees Joanna (blue), Kreen (red), and Deda (red).
	// Joanna sees Ariana (blue), Kreen (red), and Deda (red).
	// Kreen sees Ariana (red), Joanna (red), and Deda (blue).
	// Deda sees Ariana (red), Joanna (red), and Kreen (blue).

	// Bounty + GCW Scenario
	// ==========
	// Ariana = Covert Rebel Jedi
	// Kreen = Bounty Hunter
	// Deda = Overt Imperial BH group member
	//
	// Kreen and Deda are grouped together.
	//
	// 1. Initial flags:
	//   Ariana (Covert Rebel Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//   Kreen (BH):
	//   Deda (Overt Imperial BH group):
	//
	// Ariana Kreen (blue) and Deda (blue).
	// Kreen sees Ariana (blue + attackable) and Deda (blue).
	// Deda sees Ariana (blue) and Kreen (blue).
	//
	// 2. Ariana kills a stormtrooper and gains a GCW TEF:
	//   Ariana (Covert Rebel Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(0, IMPERIAL, TEFTIMER);
	//   Kreen (BH):
	//   Deda (Overt Imperial BH group):
	//     EnemyFlag(0, REBEL, 0);
	//
	// Ariana Kreen (blue) and Deda (blue).
	// Kreen sees Ariana (blue + attackable) and Deda (blue).
	// Deda sees Ariana (blue + attackable) and Kreen (blue).
	//
	// 3. Deda jumps the gun and attacks Ariana too soon with the GCW TEF:
	//   Ariana (Covert Rebel Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(0, IMPERIAL, TEFTIMER); *
	//   Kreen (BH):
	//   Deda (Overt Imperial BH group):
	//     EnemyFlag(0, REBEL, 0);
	//
	// Ariana Kreen (blue) and Deda (red).
	// Kreen sees Ariana (blue + attackable) and Deda (blue).
	// Deda sees Ariana (red) and Kreen (red).
	//
	// * The GCW TEF will be refreshed every time Ariana attacks Deda, since
	//   this EnemyFlag is what currently links the two players
	//
	// 4. Kreen engages Ariana and starts the bounty mission:
	//   Ariana (Covert Rebel Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(0, IMPERIAL, TEFTIMER);
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, TEFTIMER);
	//   Kreen (BH):
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER);
	//   Deda (Overt Imperial BH group):
	//     EnemyFlag(0, REBEL, 0);
	//
	// Ariana Kreen (red) and Deda (red).
	// Kreen sees Ariana (red) and Deda (blue).
	// Deda sees Ariana (red) and Kreen (blue).
	//
	// 5. Ariana and Deda each lands another strike:
	//   Ariana (Covert Rebel Jedi):
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, 0);
	//     EnemyFlag(0, IMPERIAL, TEFTIMER); **
	//     EnemyFlag(Kreen->getObjectID(), BOUNTY, TEFTIMER);
	//     EnemyFlag(Deda->getGroupID(), BOUNTY, TEFTIMER); *
	//   Kreen (BH):
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER);
	//   Deda (Overt Imperial BH group):
	//     EnemyFlag(0, REBEL, 0);
	//     EnemyFlag(Ariana->getObjectID(), BOUNTY, TEFTIMER); *
	//
	// Ariana Kreen (red) and Deda (red).
	// Kreen sees Ariana (red) and Deda (blue).
	// Deda sees Ariana (red) and Kreen (blue).
	//
	// * These EnemyFlags (of the BOUNTY faction type) now take priority for
	//   refreshing the TEF between Ariana and Deda.
	// ** The GCW TEF will fall off as the bounty fight continues. If another
	//    outside Overt Imperial attacks, (or if an NPC is attacked by Ariana),
	//    it will be refreshed. Otherwise, when it falls off Ariana will no
	//    longer be attackable by Imperials, even though she is attacking an
	//    Overt Imperial (Deda).

	// Duel Scenario
	// ==========
	// Ariana = Player 1
	// Kreen = Player 2
	//
	// 1. Initial flags:
	//   Ariana:
	//   Kreen:
	//
	// Ariana sees Kreen (blue).
	// Kreen sees Ariana (blue).
	//
	// 2. Ariana sends /duel to Kreen:
	//   Ariana:
	//   Kreen:
	//     EnemyFlag(Ariana->getObjectID(), DUEL, 0);
	//
	// Ariana sees Kreen (blue).
	// Kreen sees Ariana (blue).
	//
	// 3. Kreen sends /duel to Ariana:
	//   Ariana:
	//     EnemyFlag(Kreen->getObjectID(), DUEL, 0);
	//   Kreen:
	//     EnemyFlag(Ariana->getObjectID(), DUEL, 0);
	//
	// Ariana sees Kreen (red).
	// Kreen sees Ariana (red).

	// World Boss Scenario
	// ==========
	// Ariana = Group A
	// Deda = Group B
	//
	// 1. Initial flags:
	//   Ariana (Group A):
	//   Deda (Group B):
	//
	// 2. Ariana and her group attack a world boss:
	//   Ariana (Group A):
	//     EnemyFlag(0, SERVER, TEFTIMER);
	//   Deda (Group B):
	//
	// Ariana sees Deda (blue).
	// Deda sees Ariana (blue + attackable).
	//
	// 3. Deda attacks Ariana, who attacks back:
	//   Ariana (Group A):
	//     EnemyFlag(0, SERVER, TEFTIMER);
	//     EnemyFlag(Deda->getObjectID(), 0, TEFTIMER);
	//     EnemyFlag(Deda->getGroupID(), 0, TEFTIMER);
	//   Deda (Group B):
	//     EnemyFlag(Ariana->getObjectID(), 0, TEFTIMER);
	//     EnemyFlag(Ariana->getGroupID(), 0, TEFTIMER);
	//
	// Ariana sees Deda (red).
	// Deda sees Ariana (red).
	//
	// 4. Deda hits the world boss while attacking:
	//   Ariana (Group A):
	//     EnemyFlag(0, SERVER, TEFTIMER);
	//     EnemyFlag(Deda->getObjectID(), 0, TEFTIMER);
	//     EnemyFlag(Deda->getGroupID(), 0, TEFTIMER);
	//   Deda (Group B):
	//     EnemyFlag(Ariana->getObjectID(), 0, TEFTIMER);
	//     EnemyFlag(Ariana->getGroupID(), 0, TEFTIMER);
	//     EnemyFlag(0, SERVER, TEFTIMER);
	//
	// Ariana sees Deda (red).
	// Deda sees Ariana (red).
};

#endif /*ENEMYFLAG_H_*/
