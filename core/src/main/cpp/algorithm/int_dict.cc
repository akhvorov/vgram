//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#include <ostream>
#include "int_dict.h"

std::ostream& operator<< (std::ostream &out, const IntSeq& seq)
{
    out << "(";
    for (int i = 0; i < seq.size() - 1; i++) {
        out << seq[i] << " ";
    }
    if (!seq.empty()) {
        out << seq[seq.size() - 1];
    }
    out << ")";
    return out;
}

std::string to_string(const IntSeq& seq) {
    std::string res = "(";
    for (int i = 0; i < seq.size() - 1; i++) {
        res += std::to_string(seq[i]) + " ";
    }
    if (!seq.empty()) {
        res += std::to_string(seq[seq.size() - 1]);
    }
    res += ")";
    return res;
}