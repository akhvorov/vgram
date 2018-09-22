//
// Created by Aleksandr Khvorov on 04.09.18.
//

#include <gtest/gtest.h>
#include "../../main/cpp/int_vgram_builder_p.h"
#include "common.h"
#include <random>

TEST(IntVgramBuilderTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(IntVgramBuilderTests, ConstructorTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntVGramBuilder* builder1 = new IntVGramBuilderImpl(seqs[0], 10);
    IntVGramBuilder* builder2 = new IntVGramBuilderImpl(10);
    builder2->accept(seqs[0]);
    ASSERT_TRUE(true);
}

TEST(IntVgramBuilderTests, IndependenceTest) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> rand_len(1, 300);
    std::uniform_int_distribution<int> rand_char(1, 'z' - 'a' + 1);

    std::vector<int> alpha;
    for (char a = 'a'; a <= 'z'; a++)
        alpha.push_back(a);
    IntVGramBuilder* vgb = new IntVGramBuilderImpl(alpha, (int)alpha.size() + 100);
    for (int i = 0; i < 1000; i++) {
        int len = rand_len(generator);
        std::vector<int> builder;
        builder.reserve(len);
        for (int c = 0; c < len; c++)
            builder.push_back((char)('a' + rand_char(generator)));
        vgb->accept(builder);
    }
    ASSERT_TRUE('z' - 'a' + 5 > vgb->result()->size());
}