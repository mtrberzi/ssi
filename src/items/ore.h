#ifndef _ITEMS_ORE_
#define _ITEMS_ORE_

#include "voxel_occupant.h"
#include "material.h"
#include <cstdint>

class Ore : public Item {
public:
	Ore(Material *mat);
	virtual ~Ore();

	virtual uint16_t get_kind() const { return 1; }
	virtual uint32_t get_type() const { return get_material()->get_type(); }
	virtual bool is_ore() const { return true; }
};

#endif // _ITEMS_ORE_
