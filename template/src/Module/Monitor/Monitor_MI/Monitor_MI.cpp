#include "Wrapper_py/Module/Monitor/Monitor_MI/Monitor_MI.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B, typename R>
Wrapper_Monitor_MI<B, R>
::Wrapper_Monitor_MI(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Monitor_MI<B, R>,aff3ct::module::Monitor>(scope, "Monitor_MI")
{
}

template <typename B, typename R>
void Wrapper_Monitor_MI<B, R>
::definitions()
{
	this->def(py::init<const int, const unsigned int>(),"N"_a, "max_n_trials"_a, py::return_value_policy::reference);
};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_Monitor_MI<int, float>;