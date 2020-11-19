#ifndef FACTIONS_H_
#define FACTIONS_H_
namespace Factions {
	enum {
		FACTIONNEUTRAL = 0,
		FACTIONIMPERIAL = 0xDB4ACC54,
		FACTIONREBEL = 0x16148850,
		// SWGIntended: additional factions for EnemyFlag
		FACTIONBOUNTY = STRING_HASHCODE("bounty"),
		FACTIONBOSS = STRING_HASHCODE("boss"),
		FACTIONDUEL = STRING_HASHCODE("duel"),
		FACTIONGROUP = STRING_HASHCODE("group"),
		FACTIONSERVER = STRING_HASHCODE("server")
	};
}
#endif
