#include "gtest/gtest.h"
#include <cstdint>
#include <iostream>
#include <string>
#include "material.h"
#include "material_library.h"

// TODO material library tests

std::string path;

class TestMaterialLibrary : public ::testing::Test {
public:

    void SetUp() {
        MaterialLibrary::inst()->clear();
    }

    void TearDown() {

    }

};

TEST_F (TestMaterialLibrary, Loads) {
    ASSERT_TRUE(MaterialLibrary::inst()->get_all_materials().size() == 1); // only bedrock
    ASSERT_TRUE(MaterialLibrary::inst()->load(path));
    // assert that something was loaded
    ASSERT_TRUE(MaterialLibrary::inst()->get_all_materials().size() > 1);
}

TEST_F (TestMaterialLibrary, NoDuplicateIDs) {
    ASSERT_TRUE(MaterialLibrary::inst()->load(path));
    // for each pair of materials M and N, if M != N then M.getType() != N.getType()
    std::unordered_map<std::string, Material*> materials = MaterialLibrary::inst()->get_all_materials();
    for (auto entry1 : materials) {
        for (auto entry2 : materials) {
            Material *m1 = entry1.second;
            Material *m2 = entry2.second;
            if (m1 != m2) {
                ASSERT_FALSE(m1->get_type() == m2->get_type()) << "materials " << entry1.first << " and " << entry2.first
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
