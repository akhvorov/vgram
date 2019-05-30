//
// Created by Aleksandr Khvorov on 26.08.18.
//

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

namespace py = pybind11;

void init_tokenizers(py::module &);
void init_vgram_builders(py::module &);

PYBIND11_MODULE(vgram, m) {
    m.doc() = "python lib for vgram features building";
    init_tokenizers(m);
    init_vgram_builders(m);
}
