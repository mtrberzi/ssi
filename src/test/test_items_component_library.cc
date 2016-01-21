#include "gtest/gtest.h"
#include <cstdint>
#include <iostream>
#include <string>
#include "material.h"
#include "component_builder.h"
#include "component_library.h"

std::string path;

class TestComponentLibrary : public ::testing::Test {
public:

    void SetUp() {
        ComponentLibrary::inst()->clear();
    }

    void TearDown() {

    }

};

TEST_F (TestComponentLibrary, Loads) {
    ASSERT_TRUE(ComponentLibrary::inst()->get_all_components().size() == 0); // initially empty
    ASSERT_TRUE(ComponentLibrary::inst()->load(path));
    // assert that something was loaded
    ASSERT_TRUE(ComponentLibrary::inst()->get_all_components().size() > 0);
}

TEST_F (TestComponentLibrary, NoDuplicateIDs) {
    ASSERT_TRUE(ComponentLibrary::inst()->load(path));
    // for each pair of components M and N, if M != N then M.getType() != N.getType()
    std::unordered_map<std::string, ComponentBuilder*> components = ComponentLibrary::inst()->get_all_components();
    for (auto entry1 : components) {
        for (auto entry2 : components) {
            ComponentBuilder *c1 = entry1.second;
            ComponentBuilder *c2 = entry2.second;
            if (c1 != c2) {
                ASSERT_FALSE(c1->get_type() == c2->get_type())
                		<< "components " << entry1.first << " and " << entry2.first
                        << " have duplicate ID";
            }
        }
    }
}

int main (int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    assert (argc == 2);
    path = argv[1];
    return RUN_ALL_TESTS();
}
