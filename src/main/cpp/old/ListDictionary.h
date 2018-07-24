//
// Created by akhvorov on 08.07.18.
//

#ifndef VGRAM_LISTDICTIONARY_H
#define VGRAM_LISTDICTIONARY_H

class ListDictionary : public DictionaryBase {
public:
    const std::string DICTIONARY_INDEX_IS_CORRUPTED = "Dictionary index is corrupted!";
    const std::int32_t DICTIONARY_INDEX_IS_CORRUPTED_CODE = 1234;

    ListDictionary(std::vector<std::string> const & sexs);
    ListDictionary(std::vector<char> const & chars);

    std::vector<std::string> convertToSeqs(std::vector<char> const & chars); // TODO move to DictionaryBase?

    std::int32_t search(std::string const & seq, std::unordered_set<std::int32_t> const & excludes);
    std::string get(std::int32_t index);
    std::int32_t size();
    std::vector<std::string> alphabet();
    std::int32_t parent(std::int32_t second);

private:
    std::vector<std::string> sex;
    std::vector<std::int32_t> parents;
};


#endif //VGRAM_LISTDICTIONARY_H
