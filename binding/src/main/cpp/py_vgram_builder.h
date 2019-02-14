//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include "json.h"
#include "base_tokenizer.h"
#include "py_stream_vgram_builder.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyVGramBuilder : public PyStreamVGramBuilder {
public:
    static std::shared_ptr<PyVGramBuilder> load(const std::string &filename) {
        int size;
        double min_probability;
        SeqCoder coder;
        IntSeq freqs;
        std::vector<IntSeq> seqs, alphabet;
        bool fitted, freqs_computed;
        json dict = read_dict(filename, coder, freqs, seqs, alphabet, size, min_probability, fitted, freqs_computed);
        return std::shared_ptr<PyVGramBuilder>(new PyVGramBuilder(coder, freqs, seqs, alphabet, size, min_probability, fitted,
                                                freqs_computed));
    }

    PyVGramBuilder(int size, int iter_num);

    PyVGramBuilder(int size, int iter_num, int verbose);

    PyVGramBuilder *fit(const std::vector<IntSeq> &seqs, const std::string &filename, py::args args);

    std::vector<std::string> transform(const std::vector<IntSeq> &seqs, py::args args) const;

protected:
    int iter_num_;
    bool fitted_ = false;
    bool freqs_computed_ = false;

    void recompute_freqs(const std::vector<IntSeq> &seqs);

    json dict_to_json(BaseTokenizer *tokenizer) const override;

    static json read_dict(const std::string &filename, SeqCoder &coder, IntSeq &freqs, std::vector<IntSeq> &seqs,
                          std::vector<IntSeq> &alphabet, int &size, double &min_probability, bool &fitted,
                          bool &freqs_computed) {
        json dict = PyStreamVGramBuilder::read_dict(filename, coder, freqs, seqs, alphabet, size, min_probability);
        fitted = dict["fitted"].get<bool>();
        freqs_computed = dict["freqs_computed"].get<bool>();
        return dict;
    }

private:
    PyVGramBuilder(const SeqCoder &coder, const IntSeq &freqs, const std::vector<IntSeq> &seqs,
                   const std::vector<IntSeq> &alphabet, int size, double min_probability, bool fitted,
                   bool freqs_computed);
};

#endif //DICT_EXPANSION_VGRAM_H
