#include <boost/python/module.hpp>
#include <boost/python/def.hpp>

char const* greet()
{
   return "hello, world";
}

int sum(int a, int b)
{
  return a + b;
}

BOOST_PYTHON_MODULE(vgram)
        {
                using namespace boost::python;
                def("greet", greet);
                def("sum", sum);
        }

