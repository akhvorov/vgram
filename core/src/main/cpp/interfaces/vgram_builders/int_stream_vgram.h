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
    class IntStreamVGram {
    public:
        static std::shared_ptr<IntStreamVGram> load(const std::string &filename) {
            int size;
            double min_probability;
            std::shared_ptr<SeqCoder> coder = std::make_shared<SeqCoder>();
            IntSeq freqs;
            std::vector<IntSeq> alphabet;
            read_dict(filename, coder, freqs, alphabet, size, min_probability);
            return std::make_shared<IntStreamVGram>(coder, freqs, alphabet, size, min_probability);
        }

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

        std::shared_ptr<SeqCoder> get_coder();

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

        virtual json dict_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const;

        json coder_to_json() const;

        json alphabet_to_json(std::shared_ptr<BaseTokenizer> tokenizer) const;

        static json read_dict(const std::string &filename, const std::shared_ptr<SeqCoder> &coder, IntSeq &freqs,
                              std::vector<IntSeq> &alphabet, int &size, double &min_probability) {
//        std::cout << "in read_dict" << std::endl;
            std::ifstream file(filename);
            json dict;
            file >> dict;
            file.close();

            size = dict["size"];
            min_probability = dict["min_prob"];
//        std::cout << "first loop" << std::endl;
            for (int n : dict["coder"]) {
                coder->encode(std::vector<int>(1, n));
            }
//        std::cout << "between" << std::endl;
            for (const auto &word_obj : dict["alphabet"]) {
                freqs.push_back(word_obj["freq"].get<int>());
                alphabet.push_back(word_obj["vec"].get<IntSeq>());
            }
//        std::cout << "out read_dict" << std::endl;
            return dict;
        }
    };
}

#endif //VGRAM_INTERFACES_INT_STREAM_VGRAM_H
