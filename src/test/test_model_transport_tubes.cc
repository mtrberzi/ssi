#include "gtest/gtest.h"
#include "vector.h"
#include "world.h"
#include "world_updates.h"
#include "transport_endpoint.h"
#include "transport_tube.h"
#include "material_library.h"

#include <cstdint>
#include <queue>
#include <vector>

class TestItem : public Item {
public:
    TestItem() : Item(MaterialLibrary::inst()->get_material("bedrock")) {}
    virtual uint16_t get_kind() const { return 0; }
    virtual uint32_t get_type() const { return 0; }
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
    TestEndpoint *ept1 = new TestEndpoint();
    ASSERT_TRUE(world->add_occupant(Vector(0,0,1), Vector(0,0,0), ept1));
    world->timestep();
    ASSERT_TRUE(ept1->itemsReceived.empty());
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
    create_transport_tube(Vector(0,0,1), 1);
    create_transport_tube(Vector(1,0,1), 1);
    connect_transport_tubes(1, Vector(0,0,1), Vector(1,0,1));

    // collect a list of all transport tubes at each position
    std::vector<TransportTube*> tubes1;
    std::vector<TransportTube*> tubes2;

    for (VoxelOccupant *entry : world->get_occupants(Vector(0,0,1))) {
        if (entry->is_transport_tube()) {
            tubes1.push_back((TransportTube*)entry);
        }
    }
    for (VoxelOccupant *entry : world->get_occupants(Vector(1,0,1))) {
        if (entry->is_transport_tube()) {
            tubes2.push_back((TransportTube*)entry);
        }
    }
    ASSERT_EQ(1, tubes1.size()) << "could not find transport tube at location 1";
    ASSERT_EQ(1, tubes2.size()) << "could not find transport tube at location 2";

    TransportTube *t1 = tubes1.at(0);
    TransportTube *t2 = tubes2.at(0);
    ASSERT_EQ(1, t1->get_number_of_connected_devices()) << "wrong number of devices connected to t1";
    ASSERT_EQ(1, t2->get_number_of_connected_devices()) << "wrong number of devices connected to t2";

    ASSERT_TRUE(t1->get_connectionA() == t2 || t1->get_connectionB() == t2) << "t1 not connected to t2";
    ASSERT_TRUE(t2->get_connectionA() == t1 || t2->get_connectionB() == t1) << "t2 not connected to t1";
}

TEST_F (TestTransportTubes, ConnectEndpoint) {
    /*
    World w = new World(5, 10);
    TestEndpoint ept1 = new TestEndpoint();
    assertTrue(w.addOccupant(new Vector(0, 0, 1), new Vector(0, 0, 0), ept1));
    createTransportTube(w, new Vector(0, 0, 1), "a");
    connectEndpoint(w, "a", new Vector(0, 0, 1), ept1, "output");

    List<TransportTube> tubes = w.getOccupants(new Vector(0, 0, 1)).stream()
        .filter((o -> o instanceof TransportTube)).map((o -> (TransportTube)o)).collect(Collectors.toList());
    assertEquals(1, tubes.size());
    TransportTube t1 = tubes.get(0);
    assertEquals(1, t1.getNumberOfConnectedDevices());
    assertTrue(t1.getConnectionA() == ept1 || t1.getConnectionB() == ept1);

    assertEquals(t1, ept1.getConnectedTransport("output"));
    */

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
