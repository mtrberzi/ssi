#include "gtest/gtest.h"
#include <cstdint>
#include "world.h"
#include "material_builder.h"
#include "component_library.h"
#include "component_builder.h"
#include "smelter.h"
#include "transport_endpoint.h"
#include "world_updates.h"
#include "testutil_endpoints.h"
#include "ore.h"

static const uint32_t NUMBER_OF_TIMESTEPS_TO_SMELT = 10;
static const uint32_t NUMBER_OF_BARS_TO_PRODUCE = 4;

class TestSmelter : public ::testing::Test {
public:
	World *world;
	Material *testMaterial;
	Smelter *smelter;
	TestUtilSourceEndpoint *sourceEpt;

	Vector smelter_position;

	TestSmelter()
	: world(NULL), testMaterial(NULL), smelter(NULL), sourceEpt(NULL),
	  smelter_position(1,0,1)
	{}

	void SetUp() {
		world = new World(5,5);
		ComponentLibrary::inst()->clear();
		{ // create material 'bogusite'
			MaterialBuilder *builder = new MaterialBuilder();
			builder->set_name("bogusite");
			builder->set_type("9001");
			builder->set_durability_modifier("1.0");
			builder->add_category("metal");
			builder->set_can_be_smelted("true");
			builder->set_smelting_timesteps(std::to_string(NUMBER_OF_TIMESTEPS_TO_SMELT).c_str());
			builder->set_number_of_smelted_bars(std::to_string(NUMBER_OF_BARS_TO_PRODUCE).c_str());
			ASSERT_TRUE(builder->can_build());
			testMaterial = builder->build();
			delete builder;
		}
		{ // create component 'bar'
			ComponentBuilder *builder = new ComponentBuilder();
			builder->set_name("bar");
			builder->set_type("1");
			ASSERT_TRUE(builder->can_build());
			ComponentLibrary::inst()->add_component(builder);
		}
		{ // create component 'foo'
			ComponentBuilder *builder = new ComponentBuilder();
			builder->set_name("foo");
			builder->set_type("2");
			ASSERT_TRUE(builder->can_build());
			ComponentLibrary::inst()->add_component(builder);
		}
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

    void place_smelter() {
    	smelter = new Smelter();
    	ASSERT_TRUE(world->add_occupant(smelter_position, Vector(0,0,0), smelter));
    }

    void create_and_attach_source() {
    	sourceEpt = new TestUtilSourceEndpoint();
    	Vector source_position = smelter_position - Vector(1,0,0);
    	ASSERT_TRUE(world->add_occupant(source_position, Vector(0,0,0), sourceEpt));
    	create_transport_tube(smelter_position, 1);
    	create_transport_tube(source_position, 1);
    	connect_transport_tubes(1, source_position, smelter_position);
    	connect_endpoint(1, smelter_position, smelter, 0);
    	connect_endpoint(1, source_position, sourceEpt, 0);
    }
};

TEST_F(TestSmelter, InitialConditions) {
	place_smelter();
	ASSERT_EQ(Smelter::STATE_LOAD, smelter->get_state());
	world->timestep();
	ASSERT_EQ(Smelter::STATE_LOAD, smelter->get_state());
}

TEST_F(TestSmelter, ConnectInput) {
	place_smelter();
	create_and_attach_source();
}

TEST_F(TestSmelter, InvalidItem_Rejected) {
	place_smelter();
	create_and_attach_source();

	Item *i = ComponentLibrary::inst()->create_component("foo", testMaterial);
	sourceEpt->queue_send(i);
	world->timestep();
	world->timestep();
	// the item should be in the tube right before the smelter
	auto occupants = world->get_occupants(smelter_position);
	TransportTube *transport = NULL;
	for (VoxelOccupant *occ : occupants) {
		if (occ->is_transport_tube()) {
			transport = (TransportTube*)occ;
			break;
		}
	}
	ASSERT_TRUE(transport != NULL);
	{
		auto contents = transport->get_contents();
		ASSERT_TRUE(find(contents.begin(), contents.end(), i) != contents.end());
	}
	// step again; the item should still be there
	world->timestep();
	{
		auto contents = transport->get_contents();
		ASSERT_TRUE(find(contents.begin(), contents.end(), i) != contents.end());
	}
	// smelter should still be trying to load
	ASSERT_EQ(Smelter::STATE_LOAD, smelter->get_state());
}

TEST_F(TestSmelter, ValidItem_Accepted) {
	place_smelter();
	create_and_attach_source();
	Item *i = new Ore(testMaterial);
	sourceEpt->queue_send(i);
	world->timestep();
	world->timestep();
	// the item should be in the tube right before the smelter
	auto occupants = world->get_occupants(smelter_position);
	TransportTube *transport = NULL;
	for (VoxelOccupant *occ : occupants) {
		if (occ->is_transport_tube()) {
			transport = (TransportTube*)occ;
			break;
		}
	}
	ASSERT_TRUE(transport != NULL);
	{
		auto contents = transport->get_contents();
		ASSERT_TRUE(find(contents.begin(), contents.end(), i) != contents.end())
			<< "item not found in expected transport tube";
	}
	// step again; the item should be gone
	world->timestep();
	{
		auto contents = transport->get_contents();
		ASSERT_TRUE(find(contents.begin(), contents.end(), i) == contents.end())
			<< "item not accepted by smelter";
	}
	// smelter should now be smelting
	ASSERT_EQ(Smelter::STATE_SMELT, smelter->get_state());
	ASSERT_EQ(i, smelter->get_current_ore());
}

TEST_F(TestSmelter, ValidItem_Smelted) {
	place_smelter();
	create_and_attach_source();
	FAIL() << "not implemented yet";
}

TEST_F(TestSmelter, Output_Correct) {
	place_smelter();
	create_and_attach_source();
	FAIL() << "not implemented yet";
}

int main (int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
