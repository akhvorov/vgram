//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <gtest/gtest.h>
#include "../../main/cpp/int_dict_p.h"
#include "../../main/cpp/vector_hash.h"
#include "common.h"

TEST(IntDictImplTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(IntDictImplTests, ConstructorTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntDictImpl dict1(seqs);
    IntDictImpl dict2(seqs[0]);
    ASSERT_TRUE(true);
}

TEST(IntDictImplTests, SizeTest) {
    IntDictImpl dict(simple_seqs());
    ASSERT_EQ(dict.size(), 4);
}

TEST(IntDictImplTests, GetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntDictImpl dict(seqs);
    ASSERT_EQ(seqs[1], dict.get(0));
    ASSERT_EQ(seqs[0], dict.get(1));
    ASSERT_EQ(seqs[3], dict.get(2));
    ASSERT_EQ(seqs[2], dict.get(3));
}

TEST(IntDictImplTests, AlphabetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    IntDictImpl dict(seqs);
    const std::vector<std::vector<int>>* alphabet = dict.alphabet();
    std::unordered_set<std::vector<int>, VectorHash> set1(alphabet->begin(), alphabet->end());
    std::unordered_set<std::vector<int>, VectorHash> set2(seqs.begin(), seqs.end());
    ASSERT_EQ(set1, set2);
}

TEST(IntDictImplTests, ParentTest) {
    IntDictImpl dict(simple_seqs());
    ASSERT_EQ(-1, dict.parent(0));
    ASSERT_EQ(0, dict.parent(1));
    ASSERT_EQ(1, dict.parent(2));
    ASSERT_EQ(-1, dict.parent(3));
}

TEST(IntDictImplTests, SearchTest) {
    IntDictImpl dict(simple_seqs());
    for (int i = 0; i < dict.size(); i++) {
        ASSERT_EQ(i, dict.search(dict.get(i)));
    }

    std::unordered_set<int> excludes;
    excludes.insert(1);
    excludes.insert(2);
    ASSERT_EQ(0, dict.search(dict.get(0), &excludes));
    ASSERT_EQ(0, dict.search(dict.get(1), &excludes));
    ASSERT_EQ(0, dict.search(dict.get(2), &excludes));
    ASSERT_EQ(3, dict.search(dict.get(3), &excludes));

    int arr1[] = {1, 0};
    int arr2[] = {0, 0, 0};
    int arr3[] = {0, 1, 0};
    ASSERT_EQ(3, dict.search(std::vector<int>(std::begin(arr1), std::end(arr1)), &excludes));
    ASSERT_EQ(0, dict.search(std::vector<int>(std::begin(arr2), std::end(arr2)), &excludes));
    ASSERT_EQ(0, dict.search(std::vector<int>(std::begin(arr3), std::end(arr3)), &excludes));
    ASSERT_EQ(1, dict.search(std::vector<int>(std::begin(arr3), std::end(arr3))));
}

TEST(IntDictImplTests, LinearParseTest1) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, 2, 2, 3, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> result;

    IntDictImpl dict(simple_seqs());
    dict.parse(seq, &result);
    ASSERT_EQ(result, expected);
}

TEST(IntDictImplTests, LinearParseTest2) {
    int arr1[] = {0, 1, 2, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, -1, 2, 2, 3, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> result;

    IntDictImpl dict(simple_seqs());
    dict.parse(seq, &result);
    ASSERT_EQ(result, expected);
}

TEST(IntDictImplTests, WeightedParseTest) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, 2, 2, 3, 0};
    int arr3[] = {3, 2, 1, 4};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> freqs(std::begin(arr3), std::end(arr3));
    int total_freqs = 3 + 2 + 1 + 4;
    std::vector<int> result;

    IntDictImpl dict(simple_seqs());
    dict.parse(seq, freqs, total_freqs, &result);
    ASSERT_EQ(result, expected);
}
