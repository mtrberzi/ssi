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
