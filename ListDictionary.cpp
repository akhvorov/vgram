//
// Created by akhvorov on 08.07.18.
//

#include "ListDictionary.h"


ListDictionary::ListDictionary(std::vector<std::string> const & sexs) {
    sex = std::vector<std::string>(sexs);
    parents = std::vector<std::int32_t>(sexs.size());
    std::vector<std::pair<std::string, std::int32_t>> parents_stack;
    std::sort(sex.begin(), sex.end());

    for (std::int32_t i = 0; i < sex.size(); ++i) {
        std::string current = sex[i];
        parents[i] = -1;
        while (!parents.empty()) {
            if (CharSeqTools.startsWith(current, parents_stack.back().first)) { //TODO startWith (in search too)
                parents[i] = parents_stack.back().second;
                break;
            }
            parents_stack.pop_back();
        }
        parents_stack.push_back(std::pair<std::string, std::int32_t>(current, i));
    }
}

ListDictionary::ListDictionary(std::vector<char> const & chars) {
    this(convertToSeqs(chars));
}

std::vector<std::string> ListDictionary::convertToSeqs(std::vector<char> const & chars) {
    std::vector<std::string> initalDict(chars.size());
    for (char character : chars)
        initalDict.push_back(std::string(character));
    return initalDict;
}

std::int32_t ListDictionary::search(std::string const & seq, std::unordered_set<std::int32_t> const & excludes) {
    int index = Arrays.binarySearch(sex, seq, cmp); // TODO (from algorithm)
    if (index >= 0) {
        if (excludes == nullptr || excludes.count(index) == 0)
            return index;
        index = -(parents[index] + 2);
    }
    index = -(index + 2);
    while (index >= 0) {
        if (CharSeqTools.startsWith(seq, sex[index]) && (excludes == nullptr || excludes.count(index) == 0)) // TODO change startWith
            return index;
        index = parents[index];
    }
    throw DICTIONARY_INDEX_IS_CORRUPTED_CODE;
}


std::string ListDictionary::get(std::int32_t index) {
return sex[index];
}

std::int32_t ListDictionary::size() {
    return sex.size();
}

std::vector<std::string> ListDictionary::alphabet() {
    return std::vector<std::string>(sex);
}

std::int32_t ListDictionary::parent(std::int32_t second) {
    return parents[second];
}
