#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python.hpp>
#include <vector>
//#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

std::string greet()
{
   return "hello, world";
}

std::string hello(std::string name)
{
    std::string h = "hello, ";
    return h + name;
}

int sum(int a, int b)
{
  return a + b;
}

// it's ok!
template<class T>
struct VecToList
{
    static PyObject* convert(const std::vector<T>& vec)
    {
        boost::python::list* l = new boost::python::list();
        for(size_t i = 0; i < vec.size(); i++) {
            l->append(vec[i]);
        }

        return l->ptr();
    }
};

template<class T>
std::vector<T> list_to_vec(boost::python::list& ns)
{
    std::vector<T> v;
    for (int i = 0; i < len(ns); ++i)
    {
        v.push_back(boost::python::extract<T>(ns[i]));
    }
    return v;
}

std::vector<std::string> foo(std::vector<std::string> array)
{
    for(std::size_t i = 0; i < array.size(); i++) {
        array[i] += "!";
    }
    return array;
}


std::vector<std::string> foo2(boost::python::list& ns)
{
    return foo(list_to_vec<std::string>(ns));
}

BOOST_PYTHON_MODULE(vgram)
        {
                using namespace boost::python;
                def("greet", greet);
                def("hello", hello);
                def("sum", sum);

                /*class_<std::vector<std::string> >("vector_string")
                .def(vector_indexing_suite<std::vector<std::string> >() );*/

                // https://stackoverflow.com/questions/5314319/how-to-export-stdvector
                boost::python::to_python_converter<std::vector<std::string, std::allocator<std::string> >, VecToList<std::string> >();

                def("foo2", foo2);

        }

