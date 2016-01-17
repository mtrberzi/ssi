#include "material_library.h"
#include "material_builder.h"
#include <libxml/SAX.h>
#include <stack>
#include <cstring>
#include <vector>

MaterialLibrary *MaterialLibrary::instance = NULL;

MaterialLibrary::MaterialLibrary() {
    init();
}

MaterialLibrary::~MaterialLibrary() {
    for (auto entry : materials) {
        delete entry.second;
    }
}

void MaterialLibrary::init() {
    // the material library always contains a material named "bedrock"
    MaterialBuilder bedrockBuilder;
    bedrockBuilder.set_name("bedrock");
    bedrockBuilder.set_type("1");
    bedrockBuilder.set_durability_modifier("9999.0");
    // TODO assert bedrockBuilder.can_build()
    add_material("bedrock", bedrockBuilder.build());
}

void MaterialLibrary::add_material(std::string name, Material *material) {
    // TODO assert no duplicates
    materials[name] = material;
}

Material *MaterialLibrary::get_material(std::string name) const {
    std::unordered_map<std::string, Material*>::const_iterator it = materials.find(name);
    if (it == materials.end()) {
        return NULL;
    } else {
        return it->second;
    }
}

void MaterialLibrary::clear() {
    for (auto entry : materials) {
        delete entry.second;
    }
    init();
}

std::unordered_map<std::string, Material*> MaterialLibrary::get_all_materials() const {
    return materials;
}

enum ParserState {
    S_BOTTOM,
    S_MATERIALS,
    S_MATERIAL_DEFINITION,
    S_MATERIAL_CATEGORIES,
    S_CATEGORY,
    S_UNKNOWN,
};

struct ParserData {
    bool success;
    std::stack<ParserState> stack;

    std::vector<MaterialBuilder*> materials;
    MaterialBuilder *current_material;

    bool final_check_result;
    std::vector<Material*> final_materials;
};

static void start_document(void *ud) {
    ParserData *parser = (ParserData*)ud;
    parser->stack.push(S_BOTTOM);
    parser->success = false;
    parser->final_check_result = false;
}

static void end_document(void *ud) {
    ParserData *parser = (ParserData*)ud;
    parser->final_check_result = true;
    for (MaterialBuilder *builder : parser->materials) {
        if (!builder->can_build()) {
            // TODO error here
            parser->final_check_result = false;
        }
    }
    if (parser->final_check_result) {
        // if no errors were encountered, build each material and delete the builders
        for (MaterialBuilder *builder : parser->materials) {
            Material *mat = builder->build();
            // TODO assert not null
            parser->final_materials.push_back(mat);
            delete builder;
        }
        parser->success = true;
    } else {
        parser->success = false;
        return;
    }
}

static void chars(void *ud, const xmlChar *ch, int len) {
    ParserData *parser = (ParserData*)ud;
    // TODO characters
}

static void start_element(void *ud, const xmlChar *utfname, const xmlChar **attrs) {
    ParserData *parser = (ParserData*)ud;
    const char *name = (const char*)utfname;
    if (strcmp(name, "materials") == 0 && parser->stack.top() == S_BOTTOM) {
        parser->stack.push(S_MATERIALS);
    } else if (strcmp(name, "material") == 0 && parser->stack.top() == S_MATERIALS) {
        parser->stack.push(S_MATERIAL_DEFINITION);
        parser->current_material = new MaterialBuilder();
        // parse material attributes
        for (int i = 0; attrs[i] != NULL; i += 2) {
            const char *key = (const char*)attrs[i+0];
            const char *value = (const char*)attrs[i+1];
            if (strcmp(key, "name") == 0) {
                parser->current_material->set_name(value);
            } else if (strcmp(key, "type") == 0) {
                parser->current_material->set_type(value);
            } else if (strcmp(key, "durabilityModifier") == 0) {
                parser->current_material->set_durability_modifier(value);
            } else {
                // TODO warn about unknown attribute
            }
            // TODO attributes: canBeSmelted, smeltingTimesteps, numberOfSmeltedBars
        }
    } else if (strcmp(name, "categories") == 0 && parser->stack.top() == S_MATERIAL_DEFINITION) {
        parser->stack.push(S_MATERIAL_CATEGORIES);
    } else if (strcmp(name, "category") == 0 && parser->stack.top() == S_MATERIAL_CATEGORIES) {
        parser->stack.push(S_CATEGORY);
        for (int i = 0; attrs[i] != NULL; i += 2) {
            const char *key = (const char*)attrs[i+0];
            const char *value = (const char*)attrs[i+1];
            if (strcmp(key, "name") == 0) {
                parser->current_material->add_category(value);
            } else {
                // TODO warn about unknown attribute
            }
        }
    } else if (strcmp(name, "miningProducts") == 0 && parser->stack.top() == S_MATERIAL_DEFINITION) {
        // TODO mining products parsing
        parser->stack.push(S_UNKNOWN);
    } else {
        // don't know what this element is, or it appeared in the wrong context
        // TODO this should at least be a warning
        parser->stack.push(S_UNKNOWN);
    }
}

static void end_element(void *ud, const xmlChar *name) {
    ParserData *parser = (ParserData*)ud;
    ParserState state = parser->stack.top();
    parser->stack.pop();
    switch (state) {
    case S_MATERIAL_DEFINITION:
        parser->materials.push_back(parser->current_material);
        parser->current_material = NULL;
        break;
    case S_UNKNOWN:
    default:
        break;
    }
}

static xmlSAXHandler init_sax_handler() {
    xmlSAXHandler h = {};
    h.startDocument = start_document;
    h.endDocument = end_document;
    h.characters = chars;
    h.startElement = start_element;
    h.endElement = end_element;
    return h;
}
static xmlSAXHandler handler = init_sax_handler();

bool MaterialLibrary::load(std::string filepath) {
    struct ParserData parser;
    // TODO all failure cases should delete unused builders, etc.
    if (xmlSAXUserParseFile(&handler, &parser, filepath.c_str()) < 0) {
        return false;
    } else {
        if (parser.success) {
            for (Material *m : parser.final_materials) {
                add_material(m->get_name(), m);
            }
            return true;
        } else {
            return false;
        }
    }
}
