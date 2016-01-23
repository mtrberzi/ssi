#ifndef _REACTIONS_REACTANT_CONSTRAINT_
#define _REACTIONS_REACTANT_CONSTRAINT_

#include <cstdint>
#include "voxel_occupant.h"

class ReactantConstraint {
public:
	ReactantConstraint();
	virtual ~ReactantConstraint();

	virtual bool matches(Item *i) { return false; }
};

#endif // _REACTIONS_REACTANT_CONSTRAINT_
