//
// Created by Aleksandr Khvorov on 29/09/2018.
//

#ifndef VGRAM_INTERFACES_INT_VGRAM_H
#define VGRAM_INTERFACES_INT_VGRAM_H

#include "../json.h"
#include "../tokenizers/base_tokenizer.h"
#include "int_stream_vgram.h"

using json = nlohmann::json;

namespace vgram_core {
    class Saver;

    class IntVGram : public IntStreamVGram {
        friend class Saver;
    public:
        IntVGram(int size, int iter_num);

        IntVGram(int size, int iter_num, int verbose);

        IntVGram(std::shared_ptr<SeqCoder> coder, const IntSeq &freqs, const std::vector<IntSeq> &alphabet,
                 int size, double min_probability, bool fitted, bool freqs_computed);

        virtual ~IntVGram() = default;

        IntVGram *fit(const std::vector<IntSeq> &seqs/*, const std::string &filename,*/);

        std::vector<std::string> transform_to_string(const std::vector<IntSeq> &seqs) const;

        std::vector<IntSeq> transform(const std::vector<IntSeq> &seqs) const;

    protected:
        int iter_num_;
        bool fitted_ = false;
        bool freqs_computed_ = false;

        void recompute_freqs(const std::vector<IntSeq> &seqs);

        virtual bool isTerminated() const;
    };
}

#endif //VGRAM_INTERFACES_INT_VGRAM_H
