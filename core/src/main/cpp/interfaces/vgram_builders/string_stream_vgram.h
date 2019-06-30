//
// Created by Aleksandr Khvorov on 15/02/2019.
//

#ifndef VGRAM_INTERFACES_STRING_STREAM_VGRAM_H
#define VGRAM_INTERFACES_STRING_STREAM_VGRAM_H

#include "../tokenizers/char_tokenizer.h"
#include "int_stream_vgram.h"

using json = nlohmann::json;

namespace vgram_core {
    class StringStreamVGram {
    public:
        static std::shared_ptr<StringStreamVGram> load(const std::string &filename) {
            std::shared_ptr<IntStreamVGram> stream_builder = IntStreamVGram::load(filename);
            std::shared_ptr<BaseTokenizer> tokenizer = loadTokenizer(filename, stream_builder->get_coder());
            return std::make_shared<StringStreamVGram>(stream_builder, tokenizer);
        }


        StringStreamVGram(int size, int verbose
                //                          bool inner_words_split, bool words_are_tokens
        );

        StringStreamVGram(std::shared_ptr<IntStreamVGram> stream_builder,
                          std::shared_ptr<BaseTokenizer> tokenizer
//                          bool inner_words_split, bool words_are_tokens
        );

        void accept(const std::string &seq);

        std::vector<std::string> parse(const std::string &seq) const;

        void save(const std::string &filename) const;

        std::vector<std::string> alphabet() const;

        IntSeq freqs() const;

        void update_dict();

        static std::shared_ptr<BaseTokenizer>
        loadTokenizer(const std::string &filename, const std::shared_ptr<SeqCoder> &coder) {
            std::ifstream file(filename);
            json dict;
            file >> dict;
            file.close();
            std::vector<IntSeq> seqs;
            std::vector<std::string> texts;
            std::unordered_map<std::string, int> forward_map;
            for (const auto &word_obj : dict["alphabet"]) {
                const std::vector<int> vec = coder->decode(word_obj["vec"].get<IntSeq>());
                if (vec.size() == 1) {
                    std::string key = word_obj["text"].get<std::string>();
                    forward_map[key] = vec[0];
                }
            }
            return std::make_shared<CharTokenizer>(forward_map);
        }

    protected:
        std::shared_ptr<IntStreamVGram> stream_builder_;
        std::shared_ptr<BaseTokenizer> tokenizer_;
//        bool words_are_tokens = false;
//        bool inner_words_split = false;
    };
}

#endif //VGRAM_INTERFACES_STRING_STREAM_VGRAM_H
