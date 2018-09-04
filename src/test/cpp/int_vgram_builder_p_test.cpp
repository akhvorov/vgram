//
// Created by Aleksandr Khvorov on 04.09.18.
//

#include <gtest/gtest.h>
#include "../../main/cpp/int_vgram_builder_p.h"
#include "common.h"

TEST(IntVgramBuilderTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(IntVgramBuilderTests, ConstructorTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntVGramBuilderImpl builder(seqs[0], 10);
    ASSERT_TRUE(true);
}