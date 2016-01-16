#include "material_library.h"
#include <libxml/SAX.h>

MaterialLibrary *MaterialLibrary::instance = NULL;

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
    return NULL;
}

void MaterialLibrary::clear() {
    // TODO clear
}

std::unordered_map<std::string, Material*> MaterialLibrary::get_all_materials() const {
    return materials;
}

struct ParserState {
    bool success;
};

static void start_document(void *ud) {
    ParserState *state = (ParserState*)ud;
    // TODO
}

static void end_document(void *ud) {
    ParserState *state = (ParserState*)ud;
    // TODO
}

static xmlSAXHandler init_sax_handler() {
    xmlSAXHandler h = {};
    h.startDocument = start_document;
    h.endDocument = end_document;
    return h;
}
static xmlSAXHandler handler = init_sax_handler();

bool MaterialLibrary::load(std::string filepath) {
    struct ParserState state;
    if (xmlSAXUserParseFile(&handler, &state, filepath.c_str()) < 0) {
        return false;
    } else {
        // TODO do something with the result
        return false;
    }
}
