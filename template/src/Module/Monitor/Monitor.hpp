#ifndef WRAPPER_MONITOR_HPP_
#define WRAPPER_MONITOR_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

#include <aff3ct.hpp>

#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;

namespace aff3ct
{
namespace wrapper
{

class Wrapper_Monitor : public Wrapper_py,
                        public py::class_<aff3ct::module::Monitor, aff3ct::module::Module>
{
	public:
	Wrapper_Monitor(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Monitor() = default;
};
}
}
#endif //WRAPPER_MONITOR_HPP_