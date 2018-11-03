//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#ifndef DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H

#include <pybind11/pybind11.h>
#include <cpp/int_vgram_builder.h>
#include <cpp/seq_coder.h>
#include <pybind11/pytypes.h>
#include "json.h"
#include "base_tokenizer.h"

namespace py = pybind11;
using json = nlohmann::json;

class PyStreamVGramBuilder {
public:
    explicit PyStreamVGramBuilder(int size);
    PyStreamVGramBuilder(int size, int verbose);
    PyStreamVGramBuilder(int size, const std::string& filename);
    PyStreamVGramBuilder(int size, const std::string& filename, int verbose);
    explicit PyStreamVGramBuilder(const std::string& filename);
    PyStreamVGramBuilder(const std::string& filename, int verbose);

    void accept(const IntSeq& seq);
    IntSeq parse(const IntSeq& seq) const;
    void save(const std::string& filename, BaseTokenizer* tokenizer = nullptr) const;
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
    int verbose_;
    std::string filename_;

    virtual json dict_to_json(BaseTokenizer* tokenizer) const;
    json alphabet_to_json(BaseTokenizer* tokenizer) const;
    json coder_to_json() const;
    json read_dict(const std::string& filename);
};

#endif //DICT_EXPANSION_PY_STREAM_VGRAM_BUILDER_H
