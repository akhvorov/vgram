//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_VGRAM_BUILDER_H
#define VGRAM_INT_VGRAM_BUILDER_H

#include "int_dict.h"

class IntVGramBuilder {
public:
    const double kExtensionFactor = 1.3;
    const double kMaxPower = 20000000;

    virtual IntDict* result() const = 0;
    virtual const IntDict* alpha() const = 0;
    virtual void accept(const IntSeq& seq) = 0;
    virtual IntSeq* result_freqs() = 0;
//  void printPairs(const std::ofstream& ps) const;
//  void print(const std::ofstream& file_writer) const;
    virtual double codeLength() const = 0;
};


#endif //VGRAM_INT_VGRAM_BUILDER_H
