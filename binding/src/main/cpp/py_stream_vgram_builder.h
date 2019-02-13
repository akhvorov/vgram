//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#ifndef DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H

#include <fstream>
#include <pybind11/pybind11.h>
#include <src/main/cpp/int_vgram_builder.h>
#include <src/main/cpp/seq_coder.h>
#include <pybind11/pytypes.h>
#include "json.h"
#include "base_tokenizer.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyStreamVGramBuilder {
public:
    static std::shared_ptr<PyStreamVGramBuilder> load(const std::string &filename) {
        json dict = read_dict(filename);
        int size = dict["size"];
        double min_probability = dict["min_prob"];
        SeqCoder coder;
        IntSeq freqs;
        std::vector<IntSeq> seqs, alphabet;
        for (int n : dict["coder"]) {
            alphabet.emplace_back(1, n);
            coder.encode(std::vector<int>(1, n));
        }
        for (const auto &word_obj : dict["alphabet"]) {
            freqs.push_back(word_obj["freq"].get<int>());
            seqs.push_back(word_obj["vec"].get<IntSeq>());
        }
        return std::make_shared<PyStreamVGramBuilder>(coder, freqs, seqs, alphabet, size, min_probability);
    }

    explicit PyStreamVGramBuilder(int size);

    PyStreamVGramBuilder(int size, int verbose);

    void accept(const IntSeq &seq);

    IntSeq parse(const IntSeq &seq) const;

    void save(const std::string &filename, BaseTokenizer *tokenizer = nullptr) const;

    std::vector<IntSeq> alphabet() const;

    IntSeq freqs() const;

    void update_dict();

protected:
    int size_;
    std::shared_ptr<IntVGramBuilder> builder_;
    std::shared_ptr<IntDict> dict_;
    SeqCoder coder_;
    IntSeq freqs_;
    int total_freqs_;
    double min_probability_;
    int verbose_;

    virtual json dict_to_json(BaseTokenizer *tokenizer) const;
    json coder_to_json() const;
    json alphabet_to_json(BaseTokenizer* tokenizer) const;

    static json read_dict(const std::string& filename) {
        std::ifstream file(filename);
        json dict;
        file >> dict;
        file.close();
        return dict;
    }

private:
    PyStreamVGramBuilder(const SeqCoder &coder, const IntSeq &freqs, const std::vector<IntSeq> &seqs,
                         const std::vector<IntSeq> &alphabet, int size, double min_probability);
};

#endif //DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H
