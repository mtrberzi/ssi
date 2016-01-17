#include "gtest/gtest.h"
#include "vector.h"
#include "world.h"
#include "world_updates.h"
#include "transport_tube.h"
#include <cstdint>

class TestTransportTubes : public ::testing::Test {
public:
    void SetUp() {
        world = new World(5, 5);
    }

    void TearDown() {
        if (world != NULL) {
            delete world;
        }
    }

    void create_transport_tube(Vector position, uint32_t txID) {
        CreateTransportTubeUpdate u(position, txID);
        WorldUpdateResult result = u.apply(world);
        ASSERT_TRUE(result.was_successful());
    }

    void connect_transport_tubes(uint32_t txID, Vector pos1, Vector pos2) {
        ConnectTransportTubeUpdate u(txID, pos1, pos2);
        WorldUpdateResult result = u.apply(world);
        ASSERT_TRUE(result.was_successful());
    }

    void connect_endpoint(uint32_t txID, Vector pos, TransportEndpoint *ept, uint32_t eptID) {
        ConnectTransportEndpointUpdate u(txID, pos, ept, eptID);
        WorldUpdateResult result = u.apply(world);
        ASSERT_TRUE(result.was_successful());
    }

    World *world;
};

// TODO port the rest of IntTestTransportTubes here

TEST_F (TestTransportTubes, UnconnectedEndpoint_CannotSend) {
    FAIL() << "not implemented yet";
}

TEST_F (TestTransportTubes, UnconnectedEndpoint_CannotReceive) {
    FAIL() << "not implemented yet";
}

TEST_F (TestTransportTubes, CreateTransport) {
    create_transport_tube(Vector(0, 0, 1), 1);
}

TEST_F (TestTransportTubes, ConnectTransport) {
    FAIL() << "not implemented yet";
}

TEST_F (TestTransportTubes, ConnectEndpoint) {
    FAIL() << "not implemented yet";
}

TEST_F (TestTransportTubes, Send_HalfDuplex) {
    FAIL() << "not implemented yet";
}

TEST_F (TestTransportTubes, Send_FullDuplex) {
    FAIL() << "not implemented yet";
}

int main (int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
