//
// Created by Aleksandr Khvorov on 02/07/2019.
//

#ifndef DICT_EXPANSION_LOADER_H
#define DICT_EXPANSION_LOADER_H

#include "vgram_builders/string_vgram.h"

namespace vgram_core {
    class Loader {
    public:
        static std::shared_ptr<IntStreamVGram> load_int_stream_vgram(const std::string &filename) {
            int size;
            double min_probability;
            std::shared_ptr<SeqCoder> coder = std::make_shared<SeqCoder>();
            IntSeq freqs;
            std::vector<IntSeq> alphabet;
            read_dict(filename, coder, freqs, alphabet, size, min_probability);
            return std::make_shared<IntStreamVGram>(coder, freqs, alphabet, size, min_probability);
        }

        static IntVGram *load_int_vgram(const std::string &filename) {
            int size;
            double min_probability;
            std::shared_ptr<SeqCoder> coder = std::make_shared<SeqCoder>();
            IntSeq freqs;
            std::vector<IntSeq> alphabet;
            bool fitted, freqs_computed;
            read_fitable_dict(filename, coder, freqs, alphabet, size, min_probability, fitted, freqs_computed);
//            return std::shared_ptr<IntVGram>(
//                    new IntVGram(coder, freqs, alphabet, size, min_probability, fitted, freqs_computed));
            return new IntVGram(coder, freqs, alphabet, size, min_probability, fitted, freqs_computed);
        }

        static std::shared_ptr<StringStreamVGram> load_string_stream_vgram(const std::string &filename) {
            std::shared_ptr<IntStreamVGram> stream_builder = load_int_stream_vgram(filename);
            std::shared_ptr<BaseTokenizer> tokenizer = load_tokenizer(filename, stream_builder->get_coder());
            return std::make_shared<StringStreamVGram>(stream_builder, tokenizer);
        }

        static StringVGram *load_string_vgram(const std::string &filename) {
            std::shared_ptr<IntVGram> int_builder = std::shared_ptr<IntVGram>(load_int_vgram(filename));
            std::shared_ptr<BaseTokenizer> tokenizer = load_tokenizer(filename, int_builder->get_coder());
//            return std::make_shared<StringVGram>(int_builder, tokenizer);
            return new StringVGram(int_builder, tokenizer);
        }

    private:
        static json read_fitable_dict(const std::string &filename, const std::shared_ptr<SeqCoder> &coder,
                                      IntSeq &freqs, std::vector<IntSeq> &alphabet, int &size, double &min_probability,
                                      bool &fitted, bool &freqs_computed) {
            json dict = read_dict(filename, coder, freqs, alphabet, size, min_probability);
            fitted = dict["fitted"].get<bool>();
            freqs_computed = dict["freqs_computed"].get<bool>();
            return dict;
        }

        static json read_dict(const std::string &filename, const std::shared_ptr<SeqCoder> &coder, IntSeq &freqs,
                              std::vector<IntSeq> &alphabet, int &size, double &min_probability) {
//        std::cout << "in read_dict" << std::endl;
            json dict = read_dict_json(filename);
            size = dict["size"];
            min_probability = dict["min_prob"];
//        std::cout << "first loop" << std::endl;
            read_coder(dict, coder);
//        std::cout << "between" << std::endl;
            read_alphabet(dict, freqs, alphabet);
//        std::cout << "out read_dict" << std::endl;
            return dict;
        }

        static json read_dict_json(const std::string &filename) {
            std::ifstream file(filename);
            json dict;
            file >> dict;
            file.close();
            return dict;
        }

        static void read_coder(json &dict, const std::shared_ptr<SeqCoder> &coder) {
            for (int n : dict["coder"]) {
                coder->encode(std::vector<int>(1, n));
            }
        }

        static void read_alphabet(json &dict, IntSeq &freqs, std::vector<IntSeq> &alphabet) {
            for (const auto &word_obj : dict["alphabet"]) {
                freqs.push_back(word_obj["freq"].get<int>());
                alphabet.push_back(word_obj["vec"].get<IntSeq>());
            }
        }

        static std::shared_ptr<BaseTokenizer> load_tokenizer(const std::string &filename,
                                                             const std::shared_ptr<SeqCoder> &coder) {
            json dict = read_dict_json(filename);
            std::unordered_map<std::string, int> forward_map;
            for (const auto &word_obj : dict["alphabet"]) {
                const std::vector<int> vec = coder->decode(word_obj["vec"].get<IntSeq>());
                if (vec.size() == 1) {
                    std::string key = word_obj["text"].get<std::string>();
                    forward_map[key] = vec[0];
                }
            }
            //TODO chose tokenizer
            return std::make_shared<CharTokenizer>(forward_map);
        }
    };
}

#endif //DICT_EXPANSION_LOADER_H
