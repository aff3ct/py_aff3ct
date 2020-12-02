#include "Wrapper_py/Module/Monitor/Monitor_BFER/Monitor_BFER.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B>
Wrapper_Monitor_BFER<B>
::Wrapper_Monitor_BFER(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Monitor_BFER_AR<B>,aff3ct::module::Monitor>(scope, "Monitor_BFER")
{
}

template <typename B>
void Wrapper_Monitor_BFER<B>
::definitions()
{
	this->def(py::init<const int, const unsigned int, const unsigned int, const bool>(),"K"_a, "max_fe"_a, "max_n_frames"_a = 0, "count_unknown_values"_a = false, py::return_value_policy::reference);

	this->def("get_n_analyzed_fra", &aff3ct::module::Monitor_BFER_AR<B>::get_n_analyzed_fra);
	this->def("get_n_fe"          , &aff3ct::module::Monitor_BFER_AR<B>::get_n_fe          );
	this->def("get_n_be"          , &aff3ct::module::Monitor_BFER_AR<B>::get_n_be          );
	this->def("get_fer"           , &aff3ct::module::Monitor_BFER_AR<B>::get_fer           );
	this->def("get_ber"           , &aff3ct::module::Monitor_BFER_AR<B>::get_ber           );
	this->def("reset"             , &aff3ct::module::Monitor_BFER_AR<B>::reset             );

};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_Monitor_BFER<int>;