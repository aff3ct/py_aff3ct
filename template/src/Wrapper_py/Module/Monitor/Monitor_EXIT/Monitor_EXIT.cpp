#include "Wrapper_py/Module/Monitor/Monitor_EXIT/Monitor_EXIT.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B, typename R>
Wrapper_Monitor_EXIT<B, R>
::Wrapper_Monitor_EXIT(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Monitor_EXIT<B, R>,aff3ct::module::Monitor>(scope, "Monitor_EXIT")
{
}

template <typename B, typename R>
void Wrapper_Monitor_EXIT<B, R>
::definitions()
{
	this->def(py::init<const int, const unsigned int>(),"size"_a, "max_n_trials"_a, py::return_value_policy::take_ownership);
    this->def("get_I_A", &Monitor_EXIT<B,R>::get_I_A);
    this->def("get_I_E", &Monitor_EXIT<B,R>::get_I_E);
};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_Monitor_EXIT<int, float>;