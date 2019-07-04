//
// Created by Aleksandr Khvorov on 01/07/2019.
//

#ifndef DICT_EXPANSION_SAVER_H
#define DICT_EXPANSION_SAVER_H

#include <fstream>
#include <iomanip>
#include "vgram_builders/string_vgram.h"

namespace vgram_core {
    class Saver {
    public:
        static void save(const vgram_core::IntStreamVGram *int_stream_vgram, const std::string &filename) {
            json dict;
            add_base_info(dict, int_stream_vgram, nullptr);
            write_to_file(dict, filename);
        }

        static void save(const vgram_core::IntVGram *int_vgram, const std::string &filename) {
            json dict;
            add_base_info(dict, int_vgram, nullptr);
            add_fitted_info(dict, int_vgram);
            write_to_file(dict, filename);
        }

        static void save(const vgram_core::StringStreamVGram *string_stream_vgram, const std::string &filename) {
            json dict;
            add_base_info(dict, string_stream_vgram->stream_builder_.get(), string_stream_vgram->tokenizer_);
            write_to_file(dict, filename);
        }

        static void save(const vgram_core::StringVGram *string_vgram, const std::string &filename) {
            json dict;
            add_base_info(dict, string_vgram->int_builder_.get(), string_vgram->tokenizer_);
            add_fitted_info(dict, string_vgram->int_builder_.get());
            write_to_file(dict, filename);
        }

    private:
        static void write_to_file(json& dict, const std::string &filename) {
            std::ofstream file(filename);
            file << std::setw(2) << dict << std::endl;
            file.close();

        }

        static void add_base_info(json &dict, const vgram_core::IntStreamVGram *int_stream_vgram,
                                  const std::shared_ptr<vgram_core::BaseTokenizer> &tokenizer) {
            dict["size"] = int_stream_vgram->size_;
            dict["min_prob"] = int_stream_vgram->min_probability_;
            dict["coder"] = coder_to_json(int_stream_vgram->get_coder());
            dict["alphabet"] = alphabet_to_json(int_stream_vgram, tokenizer);
        }

        static void add_fitted_info(json &dict, const vgram_core::IntVGram *int_vgram) {
            dict["fitted"] = int_vgram->fitted_;
            dict["freqs_computed"] = int_vgram->freqs_computed_;
        }

        static json coder_to_json(const std::shared_ptr<vgram_core::SeqCoder> &coder) {
            std::vector<std::pair<int, int>> pairs(coder->code_map().begin(), coder->code_map().end());
            std::sort(pairs.begin(), pairs.end(),
                      [](const std::pair<int, int> &a, const std::pair<int, int> &b) { return a.second < b.second; });
            json coder_obj;
            for (const auto &p : pairs) {
                coder_obj.push_back(p.first);
            }
            return coder_obj;
        }

        static json alphabet_to_json(const vgram_core::IntStreamVGram *int_stream_vgram,
                                     const std::shared_ptr<vgram_core::BaseTokenizer> &tokenizer) {
            json alpha;
            const IntSeq &freqs = int_stream_vgram->freqs();
            auto &dict = int_stream_vgram->dict_;
            for (int i = 0; i < dict->size(); i++) {
                json word_obj;
                word_obj["vec"] = json(dict->get(i));
                word_obj["freq"] = freqs[i];
                if (tokenizer != nullptr) {
                    word_obj["text"] = tokenizer->decode(int_stream_vgram->get_coder()->decode(dict->get(i)));
                }
                alpha.push_back(word_obj);
            }
            return alpha;
        }
    };
}

#endif //DICT_EXPANSION_SAVER_H
