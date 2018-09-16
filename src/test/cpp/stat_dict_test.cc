//
// Created by Aleksandr Khvorov on 02.09.18.
//

#include <gtest/gtest.h>
#include "../../main/cpp/stat_dict.h"
#include "../../main/cpp/vector_hash.h"
#include "common.h"

TEST(StatDictTests, EmptyTest) {
    ASSERT_TRUE(true);
}

TEST(StatDictTests, ConstructorTest) {
    int arr1[] = {3, 2, 1, 4};
    std::vector<int> init_freqs(std::begin(arr1), std::end(arr1));
    StatDict dict(simple_seqs(), 0.002, &init_freqs);
    ASSERT_TRUE(true);
}

TEST(StatDictTests, FreqTest) {
    int arr1[] = {3, 2, 1, 4};
    std::vector<int> init_freqs(std::begin(arr1), std::end(arr1));
    StatDict dict1(simple_seqs(), 0.002, &init_freqs);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(0, dict1.freq(i));
    }

    StatDict dict2(simple_seqs(), 0.002);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(0, dict2.freq(i));
    }
}

TEST(StatDictTests, SizeTest) {
    StatDict dict(simple_seqs(), 0.002);
    ASSERT_EQ(dict.size(), 4);
}

TEST(StatDictTests, GetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    StatDict dict(simple_seqs(), 0.002);
    ASSERT_EQ(seqs[1], dict.get(0));
    ASSERT_EQ(seqs[0], dict.get(1));
    ASSERT_EQ(seqs[3], dict.get(2));
    ASSERT_EQ(seqs[2], dict.get(3));
}

TEST(StatDictTests, AlphabetTest) {
    std::vector<std::vector<int>> seqs = simple_seqs();
    StatDict dict(seqs, 0.002);
    const std::vector<std::vector<int>>& alphabet = dict.alphabet();
    std::unordered_set<std::vector<int>, VectorHash> set1(alphabet.begin(), alphabet.end());
    std::unordered_set<std::vector<int>, VectorHash> set2(seqs.begin(), seqs.end());
    ASSERT_EQ(set1, set2);
}

TEST(StatDictTests, ParentTest) {
    StatDict dict(simple_seqs(), 0.002);
    ASSERT_EQ(-1, dict.parent(0));
    ASSERT_EQ(0, dict.parent(1));
    ASSERT_EQ(1, dict.parent(2));
    ASSERT_EQ(-1, dict.parent(3));
}

TEST(StatDictTests, SearchTest) {
    StatDict dict(simple_seqs(), 0.002);
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

TEST(StatDictTests, ParseTest) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    int arr2[] = {1, 2, 2, 3, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> expected(std::begin(arr2), std::end(arr2));
    std::vector<int> result;

    StatDict dict(simple_seqs(), 0.002);
    dict.parse(seq, &result);
    ASSERT_EQ(result, expected);
}

TEST(StatDictTests, UpdateSymbolTest) {
    StatDict dict(simple_seqs(), 0.002);
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(0, dict.freq(i));
    }

    int arr[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) {
        dict.update_symbol(i, arr[i]);
    }
    for (int i = 0; i < 4; i++) {
        ASSERT_EQ(arr[i], dict.freq(i));
    }

    dict.update_symbol(0, 5);
    ASSERT_EQ(arr[0] + 5, dict.freq(0));
}

TEST(StatDictTests, CodeLengthPerCharTest) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> result;
    StatDict dict(simple_seqs(), 0.002);
    dict.parse(seq, &result);
    //int arr2[] = {1, 2, 2, 3, 0};
    ASSERT_EQ(dict.code_length_per_char(), (5 * log(5) - 2 * log(2) - 3 * 1 * log(1)) / 10);
}

TEST(StatDictTests, EnoughTest) {
    int arr1[] = {0, 1, 0, 1, 1, 0, 1, 1, 1, 0};
    std::vector<int> seq(std::begin(arr1), std::end(arr1));
    std::vector<int> result;
    StatDict dict(simple_seqs(), 0.2);
    dict.parse(seq, &result);
    ASSERT_FALSE(dict.enough(0.367));
    ASSERT_TRUE(dict.enough(0.368));
}