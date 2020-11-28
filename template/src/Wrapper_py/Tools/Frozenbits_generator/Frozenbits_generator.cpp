#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Frozenbits_generator
::Wrapper_Frozenbits_generator(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Frozenbits_generator>(scope, "Frozenbits_generator")
{
}


void Wrapper_Frozenbits_generator
::definitions()
{
	this->def("generate", &aff3ct::tools::Frozenbits_generator::generate);
	this->def("get_best_channels", &aff3ct::tools::Frozenbits_generator::get_best_channels);
	this->def("get_K", &aff3ct::tools::Frozenbits_generator::get_K);
	this->def("get_N", &aff3ct::tools::Frozenbits_generator::get_N);
	this->def("get_noise", &Frozenbits_generator::get_noise);
	this->def("set_noise", &Frozenbits_generator::set_noise);
	this->def("clone", &Frozenbits_generator::clone);
};

