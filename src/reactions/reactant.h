#ifndef _REACTIONS_REACTANT_
#define _REACTIONS_REACTANT_

#include <cstdint>
#include <vector>
#include "voxel_occupant.h"
#include "reactant_constraint.h"

class Reactant {
public:
    Reactant(uint32_t quantity, std::vector<ReactantConstraint*> constraints);
    ~Reactant();

    uint32_t get_quantity() const { return quantity; }
    std::vector<ReactantConstraint*> get_constraints() const { return constraints; }

    std::vector<Item*> match(std::vector<Item*> inputItems) const;
protected:
    uint32_t quantity;
    std::vector<ReactantConstraint*> constraints;
};

#endif // _REACTIONS_REACTANT_
