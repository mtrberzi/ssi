#include "component.h"

Component::Component(Material *material, std::string name, uint32_t type)
: Item(material), name(name), type(type) {}

Component::~Component() {}
