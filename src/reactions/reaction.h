#ifndef _REACTIONS_REACTION_
#define _REACTIONS_REACTION_

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>
#include "voxel_occupant.h"
#include "reactant.h"
#include "product.h"

class ReactionResult {
public:
    ReactionResult() : success(false) {}
    ReactionResult(std::vector<Item*> consumedReactants, std::vector<Item*> outputProducts)
        : success(true), consumedReactants(consumedReactants), outputProducts(outputProducts) {}
    ~ReactionResult() {}

    bool success;
    std::vector<Item*> consumedReactants;
    std::vector<Item*> outputProducts;
    // TODO created objects
};

class Reaction {
public:
    Reaction(uint32_t rxID, std::string name, uint32_t time,
            std::unordered_set<std::string> categories,
            std::vector<Reactant*> reactants,
            std::vector<Product*> products);
    ~Reaction();

    uint32_t get_reaction_id() const { return reactionID; }
    std::string get_name() const { return name; }
    uint32_t get_reaction_time() const { return reactionTime; }
    std::unordered_set<std::string> get_categories() const { return categories; }
    std::vector<Reactant*> get_reactants() const { return reactants; }
    std::vector<Product*> get_products() const { return products; }

    bool reactantsOK(std::vector<Item*> inputItems) const {
        auto result = match(inputItems);
        return !(result.empty());
    }
    ReactionResult react(std::vector<Item*> inputItems) const;
protected:
    uint32_t reactionID;
    std::string name;
    uint32_t reactionTime;
    std::unordered_set<std::string> categories;
    std::vector<Reactant*> reactants;
    std::vector<Product*> products;
    // TODO CreatedObject vector

    std::vector< std::vector<Item*> > match(std::vector<Item*> inputItems) const;
};

#endif // _REACTIONS_REACTION_
