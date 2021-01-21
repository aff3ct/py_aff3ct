#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_GA/Frozenbits_generator_GA.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Frozenbits_generator_GA
::Wrapper_Frozenbits_generator_GA(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Frozenbits_generator_GA,aff3ct::tools::Frozenbits_generator>(scope, "Frozenbits_generator_GA")
{
}


void Wrapper_Frozenbits_generator_GA
::definitions()
{

};

