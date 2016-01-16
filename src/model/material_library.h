#ifndef _MODEL_MATERIAL_LIBRARY_
#define _MODEL_MATERIAL_LIBRARY_

#include "material.h"
#include <string>
#include <unordered_map>

class MaterialLibrary {
protected:
    std::unordered_map<std::string, Material*> materials;

    static MaterialLibrary *instance;
    MaterialLibrary();

    void init();
public:
    static MaterialLibrary *inst() {
        if (!instance) {
            instance = new MaterialLibrary();
        }
        return instance;
    }

    ~MaterialLibrary();

    bool load(std::string filepath);

    void add_material(std::string name, Material *material);
    Material *get_material(std::string material_name) const;
    void clear();

    std::unordered_map<std::string, Material*> get_all_materials() const;
};

#endif // _MODEL_MATERIAL_LIBRARY_
