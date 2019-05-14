//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef DICT_EXPANSION_VGRAM_H
#define DICT_EXPANSION_VGRAM_H

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
#include "../json.h"
#include "../tokenizers/base_tokenizer.h"
#include "py_int_stream_vgram_builder.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyIntVGramBuilder : public PyIntStreamVGramBuilder {
public:
    static std::shared_ptr<PyIntVGramBuilder> load(const std::string &filename) {
        int size;
        double min_probability;
        std::shared_ptr<SeqCoder> coder = std::make_shared<SeqCoder>();
        IntSeq freqs;
        std::vector<IntSeq> alphabet;
        bool fitted, freqs_computed;
        json dict = read_dict(filename, coder, freqs, alphabet, size, min_probability, fitted, freqs_computed);
        return std::shared_ptr<PyIntVGramBuilder>(
                new PyIntVGramBuilder(coder, freqs, alphabet, size, min_probability, fitted, freqs_computed));
    }

    PyIntVGramBuilder(int size, int iter_num);

    PyIntVGramBuilder(int size, int iter_num, int verbose);

    PyIntVGramBuilder *fit(const std::vector<IntSeq> &seqs, /*const std::string &filename,*/ py::args &args);

    std::vector<std::string> transform_to_string(const std::vector<IntSeq> &seqs, py::args &args) const;

    std::vector<IntSeq> transform(const std::vector<IntSeq> &seqs) const;

protected:
    int iter_num_;
    bool fitted_ = false;
    bool freqs_computed_ = false;

    void recompute_freqs(const std::vector<IntSeq> &seqs);

    json dict_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const override;

    static json read_dict(const std::string &filename, const std::shared_ptr<SeqCoder> &coder, IntSeq &freqs,
                          std::vector<IntSeq> &alphabet, int &size, double &min_probability, bool &fitted,
                          bool &freqs_computed) {
        json dict = PyIntStreamVGramBuilder::read_dict(filename, coder, freqs, alphabet, size, min_probability);
        fitted = dict["fitted"].get<bool>();
        freqs_computed = dict["freqs_computed"].get<bool>();
        return dict;
    }

private:
    PyIntVGramBuilder(std::shared_ptr<SeqCoder> coder, const IntSeq &freqs, const std::vector<IntSeq> &alphabet,
                      int size, double min_probability, bool fitted, bool freqs_computed);
};

#endif //DICT_EXPANSION_VGRAM_H
