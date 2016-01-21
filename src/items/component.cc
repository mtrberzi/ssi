#include "component.h"

Component::Component(Material *material, std::string name, uint32_t type)
: name(name), type(type), Item(material) {}

Component::~Component() {}
