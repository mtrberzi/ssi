#include "material.h"

Material::Material(std::string name, uint32_t type, double durability_modifier,
        bool can_smelt, uint32_t smelting_timesteps, uint32_t number_of_smelted_bars,
        std::vector<std::string> categories)
: name(name), type(type), durability_modifier(durability_modifier),
  can_smelt(can_smelt), smelting_timesteps(smelting_timesteps), number_of_smelted_bars(number_of_smelted_bars),
  categories(categories) {}
