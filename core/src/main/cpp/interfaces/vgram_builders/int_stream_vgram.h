//
// Created by Aleksandr Khvorov on 03/11/2018.
//

#ifndef VGRAM_INTERFACES_INT_STREAM_VGRAM_H
#define VGRAM_INTERFACES_INT_STREAM_VGRAM_H

#include <fstream>
//#include <iostream>
//#include <int_vgram_builder.h>
#include "../../algorithm/int_vgram_builder.h"
#include "../json.h"
#include "../tokenizers/base_tokenizer.h"
#include "../seq_coder.h"

using json = nlohmann::json;

namespace vgram_core {
    class Saver;

    class IntStreamVGram {
        friend class Saver;
    public:
        explicit IntStreamVGram(int size);

        IntStreamVGram(int size, int verbose);

        IntStreamVGram(std::shared_ptr<SeqCoder> coder, const IntSeq &freqs, const std::vector<IntSeq> &alphabet,
                       int size, double min_probability);

        void accept(const IntSeq &seq);

        IntSeq parse(const IntSeq &seq) const;

        void save(const std::string &filename, std::shared_ptr<BaseTokenizer> tokenizer = nullptr) const;

        std::vector<IntSeq> alphabet() const;

        IntSeq freqs() const;

        void update_dict();

        const std::shared_ptr<SeqCoder> get_coder() const;

        std::shared_ptr<IntDict> get_dict();

    protected:
        int size_;
        std::shared_ptr<IntVGramBuilder> builder_;
        std::shared_ptr<IntDict> dict_;
        std::shared_ptr<SeqCoder> coder_;
        IntSeq freqs_;
        int total_freqs_;
        double min_probability_;
        int verbose_;
    };
}

#endif //VGRAM_INTERFACES_INT_STREAM_VGRAM_H
