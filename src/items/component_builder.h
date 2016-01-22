#ifndef _ITEMS_COMPONENT_BUILDER_
#define _ITEMS_COMPONENT_BUILDER_

#include <cstdint>
#include <string>
#include "component.h"

class ComponentBuilder {
public:
	ComponentBuilder();
	~ComponentBuilder();

	bool can_build() const;
	Component *build(Material *mat);

	void set_name(const char *nm);
	void set_type(const char *tp);

	std::string get_component_name() const {
		return componentName;
	}
	uint32_t get_type() const {
		return type;
	}
protected:
	std::string componentName;
	uint32_t type;
};

#endif // _ITEMS_COMPONENT_BUILDER_
