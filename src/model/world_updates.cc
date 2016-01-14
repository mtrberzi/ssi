#include "world_updates.h"

WorldUpdateResult RemoveObjectUpdate::apply(World &w) {
	w.remove_occupant(target);
	return WorldUpdateResult(true);
}

// TODO complete world updates implementation
