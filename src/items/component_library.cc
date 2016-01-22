#include "component_library.h"
#include <libxml/SAX.h>
#include <stack>
#include <cstring>
#include <vector>

ComponentLibrary *ComponentLibrary::instance = NULL;

ComponentLibrary::ComponentLibrary() {
}

ComponentLibrary::~ComponentLibrary() {
	clear();
}

void ComponentLibrary::clear() {
	// TODO delete all builders
	components.clear();
}

void ComponentLibrary::add_component(ComponentBuilder *builder) {
	// TODO assert no duplicates
	// TODO assert can_build
	components[builder->get_component_name()] = builder;
}

bool ComponentLibrary::contains_component(std::string name) const {
	return components.find(name) != components.end();
}

Component *ComponentLibrary::create_component(std::string name, Material *material) {
	auto iterator = components.find(name);
	if (iterator != components.end()) {
		ComponentBuilder *builder = iterator->second;
		return builder->build(material);
	} else {
		return NULL;
	}
}

std::unordered_map<std::string, ComponentBuilder*> ComponentLibrary::get_all_components() const {
	return components;
}

enum ParserState {
	S_BOTTOM,
	S_COMPONENTS,
	S_COMPONENT_DEFINITION,
	S_UNKNOWN
};

struct ParserData {
    bool success;
    std::stack<ParserState> stack;
    std::vector<ComponentBuilder*> components;

    ComponentBuilder *current_component;

    bool final_check_result;
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
    for (ComponentBuilder *builder : parser->components) {
    	if (!builder->can_build()) {
    		// TODO error here
    		parser->final_check_result = false;
    	}
    }
    if (parser->final_check_result) {
        parser->success = true;
    } else {
        parser->success = false;
        return;
    }
}

static void chars(void *ud, const xmlChar *ch, int len) {
}

static void start_element(void *ud, const xmlChar *utfname, const xmlChar **attrs) {
    ParserData *parser = (ParserData*)ud;
    const char *name = (const char*)utfname;
    if (strcmp(name, "components") == 0 && parser->stack.top() == S_BOTTOM) {
        parser->stack.push(S_COMPONENTS);
    } else if (strcmp(name, "component") == 0 && parser->stack.top() == S_COMPONENTS) {
        parser->stack.push(S_COMPONENT_DEFINITION);
        parser->current_component = new ComponentBuilder();
        // parse component attributes
        for (int i = 0; attrs[i] != NULL; i += 2) {
            const char *key = (const char*)attrs[i+0];
            const char *value = (const char*)attrs[i+1];
            if (strcmp(key, "name") == 0) {
                parser->current_component->set_name(value);
            } else if (strcmp(key, "type") == 0) {
                parser->current_component->set_type(value);
            } else {
                // TODO warn about unknown attribute
            }
        }
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
    case S_COMPONENT_DEFINITION:
        parser->components.push_back(parser->current_component);
        parser->current_component = NULL;
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

bool ComponentLibrary::load(std::string filepath) {
    struct ParserData parser;
    // TODO all failure cases should delete unused builders, etc.
    if (xmlSAXUserParseFile(&handler, &parser, filepath.c_str()) < 0) {
        return false;
    } else {
        if (parser.success) {
            for (ComponentBuilder *c : parser.components) {
            	add_component(c);
            }
            return true;
        } else {
            return false;
        }
    }
}
