//
// Created by Aleksandr Khvorov on 04.09.18.
//

#include <gtest/gtest.h>
#include <random>
#include "cpp/int_vgram_builder_p.h"
#include "cpp/int_dict_p.h"
#include "common.h"
#include "cpp/vector_hash.h"

TEST(IntVgramBuilderTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(IntVgramBuilderTests, ConstructorTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    std::shared_ptr<IntVGramBuilder> builder1(new IntVGramBuilderImpl(seqs[0], 10));
    std::shared_ptr<IntVGramBuilder> builder2(new IntVGramBuilderImpl(10));
    builder2->accept(seqs[0]);
    ASSERT_TRUE(true);
}

TEST(IntVgramBuilderTests, IndependenceTest) {
    std::default_random_engine generator;
    std::uniform_int_distribution<int> rand_len(1, 300);
    std::uniform_int_distribution<int> rand_char(0, 'z' - 'a');

    std::vector<int> alpha;
    for (char a = 'a'; a <= 'z'; a++)
        alpha.push_back(a);
    std::shared_ptr<IntVGramBuilder> vgb(new IntVGramBuilderImpl(alpha, (int)alpha.size() + 100));
    for (int i = 0; i < 10000; i++) {
        int len = rand_len(generator);
        std::vector<int> builder;
        builder.reserve(len);
        for (int c = 0; c < len; c++)
            builder.push_back((char)('a' + rand_char(generator)));
        vgb->accept(builder);
    }
    ASSERT_TRUE('z' - 'a' + 5 > vgb->result()->size());
}

TEST(IntVGramBuilderTests, RestoreTest) {
    std::vector<std::vector<int>> seqs;
    seqs.push_back(std::vector<int>{0});
    seqs.push_back(std::vector<int>{1});
    seqs.push_back(std::vector<int>{2});
    seqs.push_back(std::vector<int>{2, 2});
    seqs.push_back(std::vector<int>{0, 0});
    seqs.push_back(std::vector<int>{1, 1});

    std::default_random_engine generator;
    std::uniform_int_distribution<int> rand_len(1, 30);
    std::uniform_int_distribution<int> sample(0, static_cast<int>(seqs.size()));
    std::shared_ptr<IntDict> dict(new IntDictImpl(seqs));

    bool equalsAtLeastOnce = false;
    for (int i = 0; i < 10 && !equalsAtLeastOnce; i++) {
        IntSeq alpha;
        for (int a = 0; a <= 2; a++) {
            alpha.push_back(a);
        }
        std::shared_ptr<IntVGramBuilder> de(new IntVGramBuilderImpl(alpha, dict->size()));
        for (int j = 0; j < 1000; j++) {
            int len = rand_len(generator);
            IntSeq seq;
            for (int c = 0; c < len; c++) {
                int ind = sample(generator);
                seq.insert(seq.end(), seqs[ind].begin(), seqs[ind].end());
            }
            de->accept(seq);
        }
        equalsAtLeastOnce = de->result()->alphabet() == dict->alphabet();
    }
    ASSERT_TRUE(equalsAtLeastOnce);
}

TEST(IntVGramBuilderTests, RestoreTestAsym) {
    std::vector<std::vector<int>> seqs;
    seqs.push_back(std::vector<int>{0});
    seqs.push_back(std::vector<int>{1});
    seqs.push_back(std::vector<int>{2});
    seqs.push_back(std::vector<int>{2, 2});
    seqs.push_back(std::vector<int>{0, 1});
    seqs.push_back(std::vector<int>{1, 1});

    std::default_random_engine generator;
    std::uniform_int_distribution<int> rand_len(1, 30);
    std::uniform_int_distribution<int> sample(0, static_cast<int>(seqs.size()) - 1);
    std::shared_ptr<IntDict> dict(new IntDictImpl(seqs));

    bool equalsAtLeastOnce = false;
    std::cout << std::endl;
    for (int i = 0; i < 10 && !equalsAtLeastOnce; i++) {
        IntSeq alpha;
        for (int a = 0; a <= 2; a++) {
            alpha.push_back(a);
        }
        std::shared_ptr<IntVGramBuilder> de(new IntVGramBuilderImpl(alpha, dict->size()));
        for (int j = 0; j < 5000; j++) {
            int len = rand_len(generator);
            IntSeq seq;
            for (int c = 0; c < len; c++) {
                int ind = sample(generator);
                seq.insert(seq.end(), seqs[ind].begin(), seqs[ind].end());
            }
            de->accept(seq);
        }
        equalsAtLeastOnce = de->result()->alphabet() == dict->alphabet();
    }
    ASSERT_TRUE(equalsAtLeastOnce);
}

TEST(IntVGramBuilderTests, RestoreTestLong) {
    std::vector<IntSeq> seqs;
    seqs.push_back(std::vector<int>{4, 0, 1, 0});
    seqs.push_back(std::vector<int>{2, 0, 3, 0, 2});
    seqs.push_back(std::vector<int>{0, 1, 0, 0});
    seqs.push_back(std::vector<int>{1, 0, 1});
    seqs.push_back(std::vector<int>{2, 0, 1, 3, 0, 2, 0, 4, 0, 1, 3, 0});

    std::default_random_engine generator;
    std::uniform_int_distribution<int> rand_len(1, 100);
    std::uniform_int_distribution<int> sample(0, static_cast<int>(seqs.size()) - 1);
    std::shared_ptr<IntDict> dict(new IntDictImpl(seqs));

    bool equalsAtLeastOnce = false;
    std::cout << std::endl;
    for (int i = 0; i < 1 && !equalsAtLeastOnce; i++) {
        IntDict* alpha = new IntDictImpl(std::vector<IntSeq>{{0}, {1}, {2}, {3}, {4}});
        std::shared_ptr<IntVGramBuilder> de(new IntVGramBuilderImpl(*alpha, dict->size() + alpha->size()));
        for (int j = 0; j < 1000; j++) {
            int len = rand_len(generator);
            IntSeq seq;
            for (int c = 0; c < len; c++) {
                int ind = sample(generator);
                seq.insert(seq.end(), seqs[ind].begin(), seqs[ind].end());
            }
            de->accept(seq);
        }
        for (const IntSeq& alpha_seq : alpha->alphabet())
            seqs.push_back(alpha_seq);
        std::sort(seqs.begin(), seqs.end());
        equalsAtLeastOnce = de->result()->alphabet() == seqs;
    }
    ASSERT_TRUE(equalsAtLeastOnce);
}
