//
// Created by Igor E. Kuralenok on 13.07.2018.
//

#include "int_dict_p.h"
#include <ostream>

IntDict* IntDict::create() {

}

std::ostream& operator<< (std::ostream &out, const IntSeq& seq)
{
    out << "(";
    for (int i = 0; i < seq.size() - 1; i++) {
        out << seq[i] << " ";
    }
    if (seq.size() > 0) {
        out << seq[seq.size() - 1];
    }
    out << ")";
    return out;
}