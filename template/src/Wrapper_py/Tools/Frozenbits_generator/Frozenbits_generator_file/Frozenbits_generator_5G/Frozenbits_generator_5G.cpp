#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_file/Frozenbits_generator_5G/Frozenbits_generator_5G.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Frozenbits_generator_5G
::Wrapper_Frozenbits_generator_5G(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Frozenbits_generator_5G,aff3ct::tools::Frozenbits_generator_file>(scope, "Frozenbits_generator_5G")
{
}


void Wrapper_Frozenbits_generator_5G
::definitions()
{
	this->def(py::init<const int, const int>(), "K"_a, "N"_a);
};

