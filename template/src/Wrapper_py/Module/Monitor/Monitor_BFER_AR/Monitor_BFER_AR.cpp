#include "Wrapper_py/Module/Monitor/Monitor_BFER_AR/Monitor_BFER_AR.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B>
Wrapper_Monitor_BFER_AR<B>
::Wrapper_Monitor_BFER_AR(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Monitor_BFER_AR<B>,aff3ct::module::Monitor>(scope, "Monitor_BFER_AR")
{
}

template <typename B>
void Wrapper_Monitor_BFER_AR<B>
::definitions()
{
	this->def(py::init<const int, const unsigned int, const unsigned int, const bool>(),"K"_a, "max_fe"_a, "max_n_frames"_a = 0, "count_unknown_values"_a = false, py::return_value_policy::take_ownership);

	this->def("get_n_analyzed_fra", &aff3ct::module::Monitor_BFER_AR<B>::get_n_analyzed_fra);
	this->def("get_n_fe"          , &aff3ct::module::Monitor_BFER_AR<B>::get_n_fe          );
	this->def("get_n_be"          , &aff3ct::module::Monitor_BFER_AR<B>::get_n_be          );
	this->def("get_fer"           , &aff3ct::module::Monitor_BFER_AR<B>::get_fer           );
	this->def("get_ber"           , &aff3ct::module::Monitor_BFER_AR<B>::get_ber           );

};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_Monitor_BFER_AR<int>;