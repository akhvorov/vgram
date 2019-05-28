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
    class IntVGram : public IntStreamVGram {
    public:
        static IntVGram *load(const std::string &filename) {
            int size;
            double min_probability;
            std::shared_ptr<SeqCoder> coder = std::make_shared<SeqCoder>();
            IntSeq freqs;
            std::vector<IntSeq> alphabet;
            bool fitted, freqs_computed;
            json dict = read_dict(filename, coder, freqs, alphabet, size, min_probability, fitted, freqs_computed);
//            return std::shared_ptr<IntVGram>(
//                    new IntVGram(coder, freqs, alphabet, size, min_probability, fitted, freqs_computed));
            return new IntVGram(coder, freqs, alphabet, size, min_probability, fitted, freqs_computed);
        }

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

        json dict_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const override;

        static json read_dict(const std::string &filename, const std::shared_ptr<SeqCoder> &coder, IntSeq &freqs,
                              std::vector<IntSeq> &alphabet, int &size, double &min_probability, bool &fitted,
                              bool &freqs_computed) {
            json dict = IntStreamVGram::read_dict(filename, coder, freqs, alphabet, size, min_probability);
            fitted = dict["fitted"].get<bool>();
            freqs_computed = dict["freqs_computed"].get<bool>();
            return dict;
        }

        virtual bool isTerminated() const;
    };
}

#endif //VGRAM_INTERFACES_INT_VGRAM_H
