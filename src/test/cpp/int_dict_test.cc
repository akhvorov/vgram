//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <gtest/gtest.h>
#include "../../main/cpp/int_dict_p.h"
#include "../../main/cpp/vector_hash.h"
#include "common.h"

TEST(IntDictTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(IntDictTests, ConstructorTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntDict* dict1 = new IntDictImpl(seqs);
    IntDict* dict2 = new IntDictImpl(seqs[0]);
    ASSERT_TRUE(true);
}

TEST(IntDictTests, SizeTest) {
    IntDict* dict = new IntDictImpl(simple_seqs());
    ASSERT_EQ(dict->size(), 4);
}

TEST(IntDictTests, GetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntDict* dict = new IntDictImpl(seqs);
    ASSERT_EQ(seqs[1], dict->get(0));
    ASSERT_EQ(seqs[0], dict->get(1));
    ASSERT_EQ(seqs[3], dict->get(2));
    ASSERT_EQ(seqs[2], dict->get(3));
}

TEST(IntDictTests, AlphabetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntDict* dict = new IntDictImpl(seqs);
    const std::vector<std::vector<int>>& alphabet = dict->alphabet();
    std::unordered_set<std::vector<int>, VectorHash> set1(alphabet.begin(), alphabet.end());
    std::unordered_set<std::vector<int>, VectorHash> set2(seqs.begin(), seqs.end());
    ASSERT_EQ(set1, set2);
}

TEST(IntDictTests, ParentTest) {
    IntDict* dict = new IntDictImpl(simple_seqs());
    ASSERT_EQ(-1, dict->parent(0));
    ASSERT_EQ(0, dict->parent(1));
    ASSERT_EQ(1, dict->parent(2));
    ASSERT_EQ(-1, dict->parent(3));
}

TEST(IntDictTests, SearchTest) {
    IntDict* dict = new IntDictImpl(simple_seqs());
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

    int arr1[] = {1, 0};
    int arr2[] = {0, 0, 0};
    int arr3[] = {0, 1, 0};
    ASSERT_EQ(3, dict->search(std::vector<int>(std::begin(arr1), std::end(arr1)), &excludes));
    ASSERT_EQ(0, dict->search(std::vector<int>(std::begin(arr2), std::end(arr2)), &excludes));
    ASSERT_EQ(0, dict->search(std::vector<int>(std::begin(arr3), std::end(arr3)), &excludes));
    ASSERT_EQ(1, dict->search(std::vector<int>(std::begin(arr3), std::end(arr3)), nullptr));
}

TEST(IntDictTests, LinearParseTest1) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, 2, 2, 3, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> result;

    IntDict* dict = new IntDictImpl(simple_seqs());
    dict->parse(seq, &result);
    ASSERT_EQ(result, expected);
}

TEST(IntDictTests, LinearParseTest2) {
    int arr1[] = {0, 1, 2, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, -1, 2, 2, 3, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> result;

    IntDict* dict = new IntDictImpl(simple_seqs());
    dict->parse(seq, &result, nullptr);
    ASSERT_EQ(result, expected);
}

TEST(IntDictTests, WeightedParseTest) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, 2, 2, 3, 0};
    int arr3[] = {3, 2, 1, 4};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> freqs(std::begin(arr3), std::end(arr3));
    int total_freqs = 3 + 2 + 1 + 4;
    std::vector<int> result;

    IntDict* dict = new IntDictImpl(simple_seqs());
    dict->parse(seq, freqs, total_freqs, &result);
    ASSERT_EQ(result, expected);
}
