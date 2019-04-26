//
// Created by Aleksandr Khvorov on 25/02/2019.
//

#ifndef DICT_EXPANSION_PY_VGRAM_BUILDER_H
#define DICT_EXPANSION_PY_VGRAM_BUILDER_H

#include "../tokenizers/char_tokenizer.h"
#include "py_int_vgram_builder.h"
#include "py_stream_vgram_builder.h"

class PyVGramBuilder {
public:
    static std::shared_ptr<PyVGramBuilder> load(const std::string &filename) {
        std::shared_ptr<PyIntVGramBuilder> int_builder = PyIntVGramBuilder::load(filename);
        std::shared_ptr<BaseTokenizer> tokenizer = PyStreamVGramBuilder::loadTokenizer(filename, int_builder->get_coder());
        return std::make_shared<PyVGramBuilder>(int_builder, tokenizer);
    }

    PyVGramBuilder(int size, int iter_num);

    PyVGramBuilder(int size, int iter_num, int verbose);

    PyVGramBuilder(std::shared_ptr<PyIntVGramBuilder> int_builder, std::shared_ptr<BaseTokenizer> tokenizer);

    PyVGramBuilder *fit(const std::vector<std::string> &seqs, /*const std::string &filename,*/ py::args &args);

    std::vector<std::string> transform(const std::vector<std::string> &seqs, py::args &args) const;

    void save(const std::string &filename) const;

    IntSeq freqs() const;

    std::vector<std::string> alphabet() const;

private:
    std::shared_ptr<PyIntVGramBuilder> int_builder_;
    std::shared_ptr<BaseTokenizer> tokenizer_;
};

#endif //DICT_EXPANSION_PY_VGRAM_BUILDER_H
