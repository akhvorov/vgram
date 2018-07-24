//
// Created by akhvorov on 24.07.18.
//

#include "int_dict_p.h"


int IntDictImpl::linearParse(std::vector<int> const & seq, std::vector<int> & builder,
                std::unordered_set<int> const & excludes) {

}

double IntDictImpl::weightedParse(std::vector<int> const & seq, std::vector<int> const & freqs, double totalFreq,
                                  std::vector<int> & builder, std::unordered_set<int> const & excludes) {

}

int IntDictImpl::search(vector<int> seq, vector<int> excludes = nullptr) {

}

vector<int> IntDictImpl::parse(vector<int> seq, vector<int> freqs, double totalFreq) {

}

vector<int> IntDictImpl::parse(vector<int> seq, unordered_set <int> excludes = nullptr) {

}

vector<int> IntDictImpl::get(int index) {

}

int IntDictImpl::size() {

}

vector<vector<int>> IntDictImpl::alphabet() {

}

int IntDictImpl::parent(int second) {

}

IntDictImpl::~IDictionary() {}