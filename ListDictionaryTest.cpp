//
// Created by akhvorov on 09.07.18.
//

#include <iostream>
#include "ListDictionaryTest.h"
#include "ListDictionary.h"

void printResult(bool passed) {
    if (passed)
        std::cout << "passed" << std::endl;
    else
        std::cout << "failed" << std::endl;
}

void ListDictionaryTest::testStringConstructor() {
    std::vector<std::string> seqs;
    seqs.push_back("a");
    seqs.push_back("b");
    seqs.push_back("cd");
    ListDictionary dict(seqs);
    printResult(true);
}

void ListDictionaryTest::testCharConstructor() {
    std::vector<char> seqs;
    seqs.push_back('a');
    seqs.push_back('b');
    seqs.push_back('c');
    ListDictionary dict(seqs);
    printResult(true);
}

void ListDictionaryTest::testSizeEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    ListDictionary dict(seqs);
    passed &= dict.size() == 0;
    printResult(passed);
}

void ListDictionaryTest::testSizeNotEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    seqs.push_back("a");
    seqs.push_back("b");
    seqs.push_back("cd");
    ListDictionary dict(seqs);
    passed &= dict.size() == 3;
    printResult(passed);
}

void ListDictionaryTest::testAlphabetEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    ListDictionary dict(seqs);
    passed &= dict.alphabet().size() == 0;
    printResult(passed);
}

void ListDictionaryTest::testAlphabetNotEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    seqs.push_back("a");
    seqs.push_back("b");
    seqs.push_back("cd");
    ListDictionary dict(seqs);
    std::vector<std::string> alphabet = dict.alphabet();
    passed &= alphabet.size() == 3;
    std::unordered_set<std::string> set(alphabet.begin(), alphabet.end());
    passed &= set.count("a") == 1;
    passed &= set.count("b") == 1;
    passed &= set.count("cd") == 1;
    printResult(passed);
}

void ListDictionaryTest::testSearchEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    ListDictionary dict(seqs);
    passed &= dict.search("a") == -1;
    printResult(passed);
}

void ListDictionaryTest::testSearchNotEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    seqs.push_back("a");
    seqs.push_back("ab");
    seqs.push_back("cd");
    seqs.push_back("cde");
    seqs.push_back("c");
    ListDictionary dict(seqs);
    passed &= dict.search("a") == 0;
    passed &= dict.search("ab") == 1;
    passed &= dict.search("c") == 2;
    passed &= dict.search("cd") == 3;
    passed &= dict.search("cde") == 4;
    printResult(passed);
}

void ListDictionaryTest::testGetNotEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    seqs.push_back("a");
    seqs.push_back("ab");
    seqs.push_back("cd");
    ListDictionary dict(seqs);
    passed &= dict.get(0) == "a";
    passed &= dict.get(1) == "ab";
    passed &= dict.get(2) == "cd";
    printResult(passed);
}

void ListDictionaryTest::testParentNotEmpty() {
    bool passed = true;
    std::vector<std::string> seqs;
    seqs.push_back("a");
    seqs.push_back("ab");
    seqs.push_back("cd");
    seqs.push_back("cde");
    seqs.push_back("c");
    seqs.push_back("cbd");
    ListDictionary dict(seqs);
    passed &= dict.parent(0) == -1;
    passed &= dict.parent(1) == 0;
    passed &= dict.parent(2) == -1;
    passed &= dict.parent(3) == 2;
    passed &= dict.parent(4) == 2;
    passed &= dict.parent(5) == 3;
    printResult(passed);
}