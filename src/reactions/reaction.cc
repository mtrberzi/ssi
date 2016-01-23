#include "reaction.h"
#include <algorithm>

Reaction::Reaction(uint32_t rxID, std::string name, uint32_t time,
            std::unordered_set<std::string> categories,
            std::vector<Reactant*> reactants,
            std::vector<Product*> products)
    : reactionID(rxID), name(name), reactionTime(time),
      categories(categories), reactants(reactants), products(products) {

}

Reaction::~Reaction() {
    // TODO delete unused objects
}

struct exists_in_vector {
    exists_in_vector(const std::vector<Item*>& vec) : m_vec(vec) {}
    bool operator() (Item *i) {
        return (std::find(m_vec.begin(), m_vec.end(), i) != m_vec.end());
    }
private:
    const std::vector<Item*>& m_vec;
};

std::vector< std::vector<Item*> > Reaction::match(std::vector<Item*> inputItems) const {
    std::vector< std::vector<Item*> > matchedItems;
    std::vector<Item*> itemWorklist(inputItems);
    bool result = true;

    for (int i = 0; i < reactants.size(); ++i) {
        Reactant *reactant = reactants.at(i);
        std::vector<Item*> matchedReactants = reactant->match(itemWorklist);
        if (matchedReactants.empty()) {
            result = false;
            break;
        } else {
            matchedItems.push_back(std::vector<Item*>(matchedReactants));
            // erase all items in the worklist that are the same as a matched item
            itemWorklist.erase( std::remove_if(itemWorklist.begin(), itemWorklist.end(), exists_in_vector(matchedReactants)),
                    itemWorklist.end());
        }
    }

    if (result) {
        // return a COPY of matchedItems
        return std::vector< std::vector<Item*> >(matchedItems);
    } else {
        // return an empty vector
        return std::vector< std::vector<Item*> >();
    }
}

ReactionResult Reaction::react(std::vector<Item*> inputItems) const {
    std::vector< std::vector<Item*> > reactantItems(match(inputItems));
    if (reactantItems.empty()) {
        // fail
        return ReactionResult();
    }

    std::vector<Item*> outputProducts;
    for (Product *p : products) {
        std::vector<Item*> newItems(p->produce(reactantItems));
        outputProducts.insert(outputProducts.end(), newItems.begin(), newItems.end());
    }
    // TODO newObjs

    // build the list of reagents
    std::vector<Item*> consumedReactants;
    for (std::vector<Item*> rx : reactantItems) {
        consumedReactants.insert(consumedReactants.end(), rx.begin(), rx.end());
    }
    // success
    return ReactionResult(consumedReactants, outputProducts);
}
