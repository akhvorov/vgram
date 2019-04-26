//
// Created by Aleksandr Khvorov on 15/02/2019.
//

#ifndef DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H

#include "../tokenizers/char_tokenizer.h"
#include "py_int_stream_vgram_builder.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyStreamVGramBuilder {
public:
    static std::shared_ptr<PyStreamVGramBuilder> load(const std::string &filename) {
        std::shared_ptr<PyIntStreamVGramBuilder> stream_builder = PyIntStreamVGramBuilder::load(filename);
        std::shared_ptr<BaseTokenizer> tokenizer = loadTokenizer(filename, stream_builder->get_coder());
        return std::make_shared<PyStreamVGramBuilder>(stream_builder, tokenizer);
    }

    explicit PyStreamVGramBuilder(int size);

    PyStreamVGramBuilder(int size, int verbose);

    PyStreamVGramBuilder(std::shared_ptr<PyIntStreamVGramBuilder> stream_builder,
                         std::shared_ptr<BaseTokenizer> tokenizer);

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
    std::shared_ptr<PyIntStreamVGramBuilder> stream_builder_;
    std::shared_ptr<BaseTokenizer> tokenizer_;
};


#endif //DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H
