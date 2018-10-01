//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <gtest/gtest.h>
#include "cpp/int_dict_p.h"
#include "cpp/vector_hash.h"
#include "common.h"

TEST(IntDictTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(IntDictTests, ConstructorTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    std::shared_ptr<IntDict> dict0(new IntDictImpl());
    std::shared_ptr<IntDict> dict1(new IntDictImpl(seqs));
    std::shared_ptr<IntDict> dict2(new IntDictImpl(seqs[0]));
    ASSERT_TRUE(true);
}

TEST(IntDictTests, SizeTest) {
    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    ASSERT_EQ(dict->size(), 4);
}

TEST(IntDictTests, GetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    std::shared_ptr<IntDict> dict(new IntDictImpl(seqs));
    ASSERT_EQ(seqs[1], dict->get(0));
    ASSERT_EQ(seqs[0], dict->get(1));
    ASSERT_EQ(seqs[3], dict->get(2));
    ASSERT_EQ(seqs[2], dict->get(3));
}

TEST(IntDictTests, AlphabetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    std::shared_ptr<IntDict> dict(new IntDictImpl(seqs));
    const std::vector<std::vector<int>>& alphabet = dict->alphabet();
    std::unordered_set<std::vector<int>, VectorHash> set1(alphabet.begin(), alphabet.end());
    std::unordered_set<std::vector<int>, VectorHash> set2(seqs.begin(), seqs.end());
    ASSERT_EQ(set1, set2);
}

TEST(IntDictTests, ParentTest) {
    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    ASSERT_EQ(-1, dict->parent(0));
    ASSERT_EQ(0, dict->parent(1));
    ASSERT_EQ(1, dict->parent(2));
    ASSERT_EQ(-1, dict->parent(3));
}

TEST(IntDictTests, SearchTest) {
    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    for (int i = 0; i < dict->size(); i++) {
        ASSERT_EQ(i, dict->search(dict->get(i), nullptr));
    }

    std::unordered_set<int> excludes;
    excludes.insert(1);
    excludes.insert(2);
    ASSERT_EQ(0, dict->search(dict->get(0), &excludes));
    ASSERT_EQ(0, dict->search(dict->get(1), &excludes));
    ASSERT_EQ(0, dict->search(dict->get(2), &excludes));
    ASSERT_EQ(3, dict->search(dict->get(3), &excludes));

    ASSERT_EQ(3, dict->search(std::vector<int>{1, 0}, &excludes));
    ASSERT_EQ(0, dict->search(std::vector<int>{0, 0, 0}, &excludes));
    ASSERT_EQ(0, dict->search(std::vector<int>{0, 1, 0}, &excludes));
    ASSERT_EQ(1, dict->search(std::vector<int>{0, 1, 0}, nullptr));

    ASSERT_EQ(4, dict->search(std::vector<int>{2}, nullptr));
    dict->set_mutable(false);
    ASSERT_EQ(-1, dict->search(std::vector<int>{3}, nullptr));
}

TEST(IntDictTests, LinearParseTestOrdinary) {
    std::vector<int> seq{0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    std::vector<int> expected{1, 2, 2, 3, 0};
    std::vector<int> result;

    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    dict->parse(seq, &result);
    ASSERT_EQ(result, expected);
}

TEST(IntDictTests, LinearParseTestNewSymbol) {
    std::vector<int> seq{0, 1, 2, 0, 1, 1, 0, 1, 1, 1, 0};
    std::vector<int> expected{1, 4, 2, 2, 3, 0};
    std::vector<int> result;

    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    dict->parse(seq, &result, nullptr);
    ASSERT_EQ(result, expected);
}

TEST(IntDictTests, LinearParseTestNewSymbolNotMutable) {
    std::vector<int> seq{0, 1, 2, 0, 1, 1, 0, 1, 1, 1, 0};
    std::vector<int> expected{1, 2, 2, 3, 0};
    std::vector<int> result;

    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    dict->set_mutable(false);
    dict->parse(seq, &result, nullptr);
    ASSERT_EQ(result, expected);
}

TEST(IntDictTests, NewSymbolInEmptyAlphabetTest) {
    IntSeq result;
    std::shared_ptr<IntDict> dict(new IntDictImpl());
    std::vector<std::vector<int>> seqs = simple_seqs();
    for (const IntSeq& seq : seqs) {
        dict->parse(seq, &result);
    }
    ASSERT_EQ(dict->size(), 2);
}

TEST(IntDictTests, WeightedParseTest) {
    std::vector<int> seq{0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    std::vector<int> expected{1, 2, 2, 3, 0};
    std::vector<int> freqs{3, 2, 1, 4};
    int total_freqs = 3 + 2 + 1 + 4;
    std::vector<int> result;

    std::shared_ptr<IntDict> dict(new IntDictImpl(simple_seqs()));
    dict->parse(seq, freqs, total_freqs, &result);
    ASSERT_EQ(result, expected);
}
