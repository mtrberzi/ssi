#include "material_builder.h"
#include <cstdlib>
#include <cstring>

MaterialBuilder::MaterialBuilder()
: name(""), type(0), durability_modifier(0.0),
  can_smelt(false), smelting_timesteps(0), number_of_smelted_bars(0) {
}

MaterialBuilder::~MaterialBuilder() {}

bool MaterialBuilder::can_build() const {
    if (name.empty()) return false;
    if (type == 0) return false;
    if (durability_modifier <= 0.0) return false;
    if (can_smelt) {
        if (smelting_timesteps == 0) return false;
        if (number_of_smelted_bars == 0) return false;
    }
    return true;
}

Material *MaterialBuilder::build() const {
    if (!can_build()) return NULL;
    return new Material(name, type, durability_modifier, can_smelt, smelting_timesteps, number_of_smelted_bars, categories);
}

void MaterialBuilder::set_name(const char *nm) {
    name.assign(nm);
}

void MaterialBuilder::set_type(const char *tp) {
    unsigned long ul = strtoul(tp, NULL, 10);
    type = (uint32_t)ul;
}

void MaterialBuilder::set_durability_modifier(const char *mod) {
    double d = strtod(mod, NULL);
    durability_modifier = d;
}

void MaterialBuilder::add_category(const char *cat) {
    categories.push_back(std::string(cat));
}

void MaterialBuilder::set_can_be_smelted(const char *b) {
	if (strcmp(b, "true") == 0) {
		can_smelt = true;
	} else {
		can_smelt = false;
	}
}

void MaterialBuilder::set_smelting_timesteps(const char *steps) {
	unsigned long ul = strtoul(steps, NULL, 10);
	smelting_timesteps = (uint32_t)ul;
}

void MaterialBuilder::set_number_of_smelted_bars(const char *bars) {
	unsigned long ul = strtoul(bars, NULL, 10);
	number_of_smelted_bars = (uint32_t)ul;
}
