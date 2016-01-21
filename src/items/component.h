#ifndef _ITEMS_COMPONENT_
#define _ITEMS_COMPONENT_

#include <cstdint>
#include <string>
#include "material.h"
#include "voxel_occupant.h"

class Component : public Item {
public:
	Component(Material *material, std::string componentName, uint32_t type);
	~Component();

	virtual uint16_t get_kind() const { return 2; }
	virtual uint32_t get_type() const { return type; }
	std::string get_component_name() const { return name; }
protected:
	std::string name;
	uint32_t type;
};

#endif // _ITEMS_COMPONENT_
