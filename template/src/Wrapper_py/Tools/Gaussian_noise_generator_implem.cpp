#include "Gaussian_noise_generator_implem.hpp"

namespace py = pybind11;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

Wrapper_Gaussian_noise_generator_implem
::Wrapper_Gaussian_noise_generator_implem(py::handle scope)
: Wrapper_py(),
  py::enum_<Gaussian_noise_generator_implem>(scope, "Gaussian_noise_generator_implem")
{
}

void Wrapper_Gaussian_noise_generator_implem
::definitions()
{
	this->value("STD" , Gaussian_noise_generator_implem::STD );
	this->value("FAST", Gaussian_noise_generator_implem::FAST);
	this->export_values();
};