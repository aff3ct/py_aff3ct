#include "Wrapper_py/Tools/Monitor_reduction/Monitor_reduction_BFER.hpp"
#include <pybind11/chrono.h>
#include <pybind11/functional.h>
#include <functional>

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Monitor_reduction_BFER
::Wrapper_Monitor_reduction_BFER(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Monitor_reduction<module::Monitor_BFER<>>, module::Monitor_BFER<>>(scope, "Monitor_reduction_BFER")
{
}

void Wrapper_Monitor_reduction_BFER
::definitions()
{
	this->def(py::init<const std::vector<module::Monitor_BFER<>*>>(), "monitors"_a);

	this->def("is_done_all",          &Monitor_reduction<module::Monitor_BFER<>>::is_done_all         );
	this->def("reduce_all",           &Monitor_reduction<module::Monitor_BFER<>>::reduce_all          );
	this->def("reset_all",            &Monitor_reduction<module::Monitor_BFER<>>::reset_all           );
	this->def("is_done",              &Monitor_reduction<module::Monitor_BFER<>>::is_done             );
	this->def("reduce",               &Monitor_reduction<module::Monitor_BFER<>>::reduce              );
	this->def("reset",                &Monitor_reduction<module::Monitor_BFER<>>::reset               );
	//this->def("get_monitors",         &Monitor_reduction<module::Monitor_BFER<>>::get_monitors        );
	//this->def("get_collecter",        &Monitor_reduction<module::Monitor_BFER<>>::get_collecter       );
	this->def("set_reduce_frequency",
	[](Monitor_reduction<module::Monitor_BFER<>>& self, const std::chrono::microseconds& us)
	{
		self.set_reduce_frequency(std::chrono::duration_cast<std::chrono::nanoseconds>(us));
	});
};

