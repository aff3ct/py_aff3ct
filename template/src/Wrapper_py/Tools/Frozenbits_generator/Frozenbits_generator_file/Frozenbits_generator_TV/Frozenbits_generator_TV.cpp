#include "Wrapper_py/Tools/Frozenbits_generator/Frozenbits_generator_file/Frozenbits_generator_TV/Frozenbits_generator_TV.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Frozenbits_generator_TV
::Wrapper_Frozenbits_generator_TV(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Frozenbits_generator_TV,aff3ct::tools::Frozenbits_generator_file>(scope, "Frozenbits_generator_TV")
{
}


void Wrapper_Frozenbits_generator_TV
::definitions()
{
	this->def(py::init<const int, const int, const std::string &, const std::string &>(), "K"_a, "N"_a, "awgn_codes_dir"_a, "bin_pb_path"_a);
};

