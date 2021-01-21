#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_GA_Arikan/Frozenbits_generator_GA_Arikan.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Frozenbits_generator_GA_Arikan
::Wrapper_Frozenbits_generator_GA_Arikan(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Frozenbits_generator_GA_Arikan,aff3ct::tools::Frozenbits_generator>(scope, "Frozenbits_generator_GA_Arikan")
{
}


void Wrapper_Frozenbits_generator_GA_Arikan
::definitions()
{
	this->def(py::init<const int, const int, const std::string &, const bool>(), "K"_a, "N"_a, "dump_channels_path"_a = "", "dump_channels_single_thread"_a = true);
};

