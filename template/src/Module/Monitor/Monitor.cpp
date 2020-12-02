#include "Wrapper_py/Module/Monitor/Monitor.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Monitor
::Wrapper_Monitor(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Monitor,aff3ct::module::Module>(scope, "Monitor")
{
}


void Wrapper_Monitor
::definitions()
{
	this->def("reset", &Monitor::reset);
	this->def("is_done", &Monitor::is_done);
};

