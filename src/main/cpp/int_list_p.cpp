//
// Created by akhvorov on 24.07.18.
//

#include "int_dict_p.h"



int IntDictImpl::linearParse(const std::vector<int>& seq, std::vector<int>& builder,
                std::unordered_set<int> const & excludes = nullptr) {

}

double IntDictImpl::weightedParse(const std::vector<int>& seq, const std::vector<int>& freqs, double totalFreq,
                                  std::vector<int>& builder, conststd::unordered_set<int>& excludes = nullptr) {

}

int IntDictImpl::search(const std::vector<int>& seq, const std::vector<int>& excludes = nullptr) const {

}

std::vector<int> IntDictImpl::parse(const std::vector<int>& seq, std::vector<int>& freqs, double totalFreq) {

}
std::vector<int> IntDictImpl::parse(const std::vector<int>& seq, std::unordered_set<int>& excludes = nullptr) {

}

std::vector<int> IntDictImpl::get(int index) const {

}

int IntDictImpl::size() const {

}

std::vector<std::vector<int>> IntDictImpl::alphabet() const {

}

int IntDictImpl::parent(int second) const {

}

IntDictImpl::~IDictionary() {}