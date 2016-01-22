#ifndef _MODEL_MATERIAL_BUILDER_
#define _MODEL_MATERIAL_BUILDER_

#include "material.h"
#include <string>
#include <cstdint>

class MaterialBuilder {
public:
    MaterialBuilder();
    ~MaterialBuilder();

    bool can_build() const;
    Material *build() const;

    void set_name(const char *nm);
    void set_type(const char *tp);
    void set_durability_modifier(const char *mod);
    void add_category(const char *category);

    void set_can_be_smelted(const char *b);
    void set_smelting_timesteps(const char *steps);
    void set_number_of_smelted_bars(const char *bars);

    // TODO add mining products

protected:
    std::string name;
    uint32_t type;
    double durability_modifier;
    bool can_smelt;
    uint32_t smelting_timesteps;
    uint32_t number_of_smelted_bars;
    std::vector<std::string> categories;
};

#endif // _MODEL_MATERIAL_BUILDER
