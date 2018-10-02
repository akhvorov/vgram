//
// Created by Aleksandr Khvorov on 01/10/2018.
//

#ifndef DICT_EXPANSION_TOKENIZER_H
#define DICT_EXPANSION_TOKENIZER_H


#include <string>
#import <vector>
#include <unordered_map>

class Tokenizer {
public:
    Tokenizer();
    ~Tokenizer() = default;
    void fit(const std::vector<std::string>& seqs);//, const std::vector<int>& target);
    std::vector<std::vector<int>> transform(const std::vector<std::string>& seqs);//, const std::vector<int>& target);

protected:
    char sep_ = ' ';

    std::string normalize(const std::string& str);
    std::vector<int> tokenize(const std::string& str);

private:
    std::unordered_map<char, int> dict_;
};

#endif //DICT_EXPANSION_TOKENIZER_H
