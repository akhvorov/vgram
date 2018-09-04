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

//TEST(IntVgramBuilderTests, IndependenceTest) {
//    std::vector<int> alpha;
//    for (char a = 'a'; a <= 'z'; a++)
//        alpha.push_back(a);
//    IntVGramBuilderImpl de(alpha, (int)alpha.size() + 100);
//    for (int i = 0; i < 10000; i++) {
//        int len = rand() % 300;
//        std::vector<int> builder;
//        builder.reserve(len);
//        for (int c = 0; c < len; c++)
//            builder.push_back((char)('a' + rand() % ('z' - 'a' + 1)));
//        de.accept(builder);
//    }
//    ASSERT_TRUE('z' - 'a' + 5 > de.result()->size());
//}