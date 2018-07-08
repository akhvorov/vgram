//
// Created by akhvorov on 08.07.18.
//

#ifndef VGRAM_LISTDICTIONARY_H
#define VGRAM_LISTDICTIONARY_H

class ListDictionary : DictionaryBase {
public:
    const String DICTIONARY_INDEX_IS_CORRUPTED = "Dictionary index is corrupted!";


private:
    std::vector<std::string> sex;
    std::vector<std::int32_t> parents;


}


#endif //VGRAM_LISTDICTIONARY_H
