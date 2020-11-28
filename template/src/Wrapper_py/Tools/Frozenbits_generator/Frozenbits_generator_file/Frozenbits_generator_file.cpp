#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_file/Frozenbits_generator_file.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Frozenbits_generator_file
::Wrapper_Frozenbits_generator_file(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Frozenbits_generator_file,aff3ct::tools::Frozenbits_generator>(scope, "Frozenbits_generator_file")
{
}


void Wrapper_Frozenbits_generator_file
::definitions()
{
	this->def(py::init<const int, const int, const std::string &>(), "K"_a, "N"_a, "filename"_a);
};

