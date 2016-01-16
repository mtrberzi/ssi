#ifndef _MODEL_MATERIAL_
#define _MODEL_MATERIAL_

#include <cstdint>
#include <string>
#include <vector>

class Material {
public:
    Material(std::string name, uint32_t type, double durability_modifier,
            bool can_smelt, uint32_t smelting_timesteps, uint32_t number_of_smelted_bars,
            std::vector<std::string> categories);
    ~Material(){}

    std::string get_name() const { return name; }
    uint32_t get_type() const { return type; }
    double get_durability_modifier() const { return durability_modifier; }
    bool can_be_smelted() const { return can_smelt; }
    uint32_t get_smelting_timesteps() const { return smelting_timesteps; }
    uint32_t get_number_of_smelted_bars() const { return number_of_smelted_bars; }
    std::vector<std::string> get_categories() const { return categories; }

protected:
    std::string name;
    uint32_t type;
    double durability_modifier;
    bool can_smelt;
    uint32_t smelting_timesteps;
    uint32_t number_of_smelted_bars;
    std::vector<std::string> categories;
    // TODO std::vector<MiningProduct> mining_products
};

#endif // _MODEL_MATERIAL_
