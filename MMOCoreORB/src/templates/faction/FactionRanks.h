/*
 * FactionRank.h
 *
 *  Created on: 15/01/2012
 *      Author: victor
 */

#ifndef FACTIONRANKS_H_
#define FACTIONRANKS_H_

#include "FactionRank.h"

class DataTableIff;

class FactionRanks {
protected:
	Vector<FactionRank> ranks;

public:
	FactionRanks();
	FactionRanks(const FactionRanks& f);

	void readObject(DataTableIff* table);

	const FactionRank& getRank(int index) const {
		return ranks.get(index);
	}

	int getCount() const {
		return ranks.size();
	}
};


#endif /* FACTIONRANK_H_ */
