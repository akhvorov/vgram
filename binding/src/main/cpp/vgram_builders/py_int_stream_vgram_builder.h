//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#ifndef DICT_EXPANSION_PY_INT_STREAM_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_INT_STREAM_VGRAM_BUILDER_H

#include <fstream>
#include <pybind11/pybind11.h>
#include <src/main/cpp/int_vgram_builder.h>
#include "../seq_coder.h"
#include <pybind11/pytypes.h>
#include "../json.h"
#include "../tokenizers/base_tokenizer.h"
//#include "../coders/seq_coder.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyIntStreamVGramBuilder {
public:
    static std::shared_ptr<PyIntStreamVGramBuilder> load(const std::string &filename) {
        int size;
        double min_probability;
        SeqCoder coder;
        IntSeq freqs;
        std::vector<IntSeq> seqs, alphabet;
        read_dict(filename, coder, freqs, seqs, alphabet, size, min_probability);
        return std::shared_ptr<PyIntStreamVGramBuilder>(
                new PyIntStreamVGramBuilder(coder, freqs, seqs, alphabet, size, min_probability));
    }

    explicit PyIntStreamVGramBuilder(int size);

    PyIntStreamVGramBuilder(int size, int verbose);

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

    json alphabet_to_json(BaseTokenizer *tokenizer) const;

    static json read_dict(const std::string &filename, SeqCoder &coder, IntSeq &freqs, std::vector<IntSeq> &seqs,
                          std::vector<IntSeq> &alphabet, int &size, double &min_probability) {
        std::ifstream file(filename);
        json dict;
        file >> dict;
        file.close();

        size = dict["size"];
        min_probability = dict["min_prob"];
        for (int n : dict["coder"]) {
            alphabet.emplace_back(1, n);
            coder.encode(std::vector<int>(1, n));
        }
        for (const auto &word_obj : dict["alphabet"]) {
            freqs.push_back(word_obj["freq"].get<int>());
            seqs.push_back(word_obj["vec"].get<IntSeq>());
        }
        return dict;
    }

private:
//    friend class PyStreamVGramBuilder;

    PyIntStreamVGramBuilder(const SeqCoder &coder, const IntSeq &freqs, const std::vector<IntSeq> &seqs,
                         const std::vector<IntSeq> &alphabet, int size, double min_probability);
};

#endif //DICT_EXPANSION_PY_INT_STREAM_VGRAM_BUILDER_H
