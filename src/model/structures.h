#ifndef _MODEL_STRUCTURES_
#define _MODEL_STRUCTURES_

#include "voxel_occupant.h"
#include <cstdint>
#include "material_library.h"

// Contains definitions for all subclasses of Structure

class Bedrock : public Structure {
public:
    Bedrock() : Structure(MaterialLibrary::inst()->get_material("bedrock")) {}

    virtual bool impedesXYMovement() const { return true; }

    virtual bool impedesZMovement() const {
        return true;
    }

    virtual bool impedesXYFluidFlow() const {
        return true;
    }

    virtual bool impedesZFluidFlow() const {
        return true;
    }

    virtual bool supportsOthers() const {
        return true;
    }

    virtual bool needsSupport() const {
        return false;
    }

    virtual uint32_t getType() const {
        return 0;
    }

protected:

    virtual int32_t get_base_durability() const { return 999999; }

};

// TODO Block

// TODO Floor

// TODO Ramp

// TODO Up/Left/Right/DownRamp

#endif // _MODEL_STRUCTURES_
