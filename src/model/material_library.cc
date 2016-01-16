#include "material_library.h"

MaterialLibrary::MaterialLibrary() {
    init();
}

MaterialLibrary::~MaterialLibrary() {
    // TODO delete all materials
}

void MaterialLibrary::init() {
    // the material library always contains a material named "bedrock"
    // TODO make bedrock
    /*
    MaterialBuilder bedrockBuilder = new MaterialBuilder();
    bedrockBuilder.setMaterialName("bedrock");
    bedrockBuilder.setType(0);
    bedrockBuilder.setDurabilityModifier(9999.0);
    addMaterial("bedrock", bedrockBuilder.build());
    */
}

void MaterialLibrary::add_material(std::string name, Material *material) {
    // TODO add_material
}

Material *MaterialLibrary::get_material(std::string name) const {
    // TODO get_material
}

void MaterialLibrary::clear() {
    // TODO clear
}

bool MaterialLibrary::load(std::string filepath) {
    // TODO load
}
