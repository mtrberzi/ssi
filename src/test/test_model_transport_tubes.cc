#include "gtest/gtest.h"
#include "vector.h"
#include "world.h"
#include "world_updates.h"
#include "transport_endpoint.h"
#include "transport_tube.h"
#include <cstdint>
#include <queue>
#include <vector>

class TestItem : public Item {
public:
    TestItem() : Item(MaterialLibrary::inst()->get_material("bedrock")) {}
    virtual uint16_t get_kind() const { return 0; }
    virtual uint32_t get_type() const { return 0; }
    /*
    public TestItem() {
      super(MaterialLibrary.getInstance().getMaterial("bedrock"));
    }

    @Override
    public short getKind() {
      return (short)0;
    }

    @Override
    public int getType() {
      return 0;
    }
    */
};

class TestEndpoint : public TransportEndpoint {
public:
    TestEndpoint() {}
    virtual ~TestEndpoint() {}
    virtual Vector get_extents() const { return Vector(1,1,1); }
    virtual uint32_t get_type() const { return 0; }
    virtual bool has_world_updates() const { return false; }

    virtual std::unordered_set<uint32_t> get_transport_endpoints() const {
        // 0 = input, 1 = output
        return std::unordered_set<uint32_t> {0, 1};
    }

    std::vector<Item*> itemsReceived;
    virtual bool receive_to_endpoint(uint32_t endpointID, Item *item) {
        if (endpointID == 0) {
            itemsReceived.push_back(item);
            return true;
        } else {
            return false;
        }
    }

    std::queue<Item*> sendQueue;
    void queue_output(Item *i) {
        sendQueue.push(i);
    }

protected:
    virtual void pre_send_timestep() {
        if (!(sendQueue.empty())) {
            Item *i = sendQueue.front();
            set_endpoint_output(1, i);
        }
    }

    virtual void post_send_timestep(std::unordered_map<uint32_t, bool> send_results) {
        auto result = send_results.find(1);
        if (result != send_results.end()) {
            bool status = result->second;
            if (status) {
                // remove the item that was sent
                sendQueue.pop();
            }
        }
    }
};

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
    TestEndpoint *ept1 = new TestEndpoint();
    ASSERT_TRUE(world->add_occupant(Vector(0,0,1), Vector(0,0,0), ept1));
    ept1->queue_output(new TestItem());
    world->timestep();
    ASSERT_FALSE(ept1->sendQueue.empty());
}

TEST_F (TestTransportTubes, UnconnectedEndpoint_CannotReceive) {
    FAIL() << "not implemented yet";
}

TEST_F (TestTransportTubes, CreateTransport) {
    Vector position(0,0,1);
    uint32_t txID = 1;
    create_transport_tube(position, txID);
    bool found_ok = false;
    auto occupants = world->get_occupants(position);
    for (VoxelOccupant *occ : occupants) {
        if (!(occ->is_transport_tube())) continue;
        TransportTube *transport = (TransportTube*)occ;
        if (transport->get_transport_id() == txID) {
            found_ok = true;
            break;
        }
    }
    ASSERT_TRUE(found_ok) << "could not find transport tube with matching ID at target position";
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
