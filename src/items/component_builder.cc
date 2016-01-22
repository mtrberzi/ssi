#include "component_builder.h"

ComponentBuilder::ComponentBuilder()
: componentName(""), type(0) {}

ComponentBuilder::~ComponentBuilder() {}

bool ComponentBuilder::can_build() const {
	if (componentName.compare("") == 0) {
		return false;
	}
	if (type == 0) {
		return false;
	}
	return true;
}

Component *ComponentBuilder::build(Material *material) {
	if (!can_build()) return NULL;
	return new Component(material, componentName, type);
}

void ComponentBuilder::set_name(const char *nm) {
	componentName.assign(nm);
}

void ComponentBuilder::set_type(const char *tp) {
    unsigned long ul = strtoul(tp, NULL, 10);
    type = (uint32_t)ul;
}
