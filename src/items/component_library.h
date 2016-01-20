#ifndef _ITEMS_COMPONENT_LIBRARY_
#define _ITEMS_COMPONENT_LIBRARY_

#include <cstdint>
#include <string>
#include <unordered_map>

#include "component_builder.h"
#include "component.h"
#include "material.h"

class ComponentLibrary {
protected:
	static ComponentLibrary *instance;
	ComponentLibrary();

	std::unordered_map<std::string, ComponentBuilder*> components;
public:
	static ComponentLibrary *inst() {
		if (!instance) {
			instance = new ComponentLibrary();
		}
		return instance;
	}
	~ComponentLibrary();

	bool load(std::string filepath);

	void add_component(ComponentBuilder *builder);
	bool contains_component(std::string name) const;
	Component *create_component(std::string name, Material *material);

	std::unordered_map<std::string, ComponentBuilder*> get_all_components() const;

};

#endif // _ITEMS_COMPONENT_LIBRARY_
