//
// Created by akhvorov on 27.06.18.
//

#ifndef VGRAM_LIBRARY_H
#define VGRAM_LIBRARY_H

char const* greet();
int sum(int a, int b);

BOOST_PYTHON_MODULE(vgram)
{
    using namespace boost::python;
    def("greet", greet);
    def("sum", sum);
}

#endif //VGRAM_LIBRARY_H
