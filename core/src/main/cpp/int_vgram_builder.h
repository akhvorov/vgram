//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#ifndef VGRAM_INT_VGRAM_BUILDER_H
#define VGRAM_INT_VGRAM_BUILDER_H

#include <memory>
#include "int_dict.h"

class IntVGramBuilder {
public:
    const double kExtensionFactor = 1.3;
    const double kMaxPower = 20000000;

    virtual std::shared_ptr<IntDict> result() const = 0;
    virtual const std::shared_ptr<IntDict> alpha() const = 0;
    virtual void accept(const IntSeq& seq) = 0;
    virtual int result_freqs(IntSeq* freqs) = 0;
    virtual double code_length() const = 0;
    virtual double get_min_probability() const = 0;
};


#endif //VGRAM_INT_VGRAM_BUILDER_H
