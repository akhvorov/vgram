//
// Created by Aleksandr Khvorov on 06/05/2019.
//

#include <iostream>
#include "vgram_builders/py_int_stream_vgram_builder.h"

using namespace std;

void test_save_int_stream_vgram() {
    string path = "test_int_stream_dict.json";
    shared_ptr<PyIntStreamVGramBuilder> loadedVgram = PyIntStreamVGramBuilder::load(path);
    cout << "load int stream vgram" << endl;
    cout << loadedVgram << endl;
    cout << "print loaded vgram" << endl;
    vector<int> v = {0, 1, 0, 1};
    cout << loadedVgram->parse(v) << endl;
    cout << "parsed" << endl;
}

int main() {
    cout << "START" << endl;
    test_save_int_stream_vgram();
    return 0;
}
