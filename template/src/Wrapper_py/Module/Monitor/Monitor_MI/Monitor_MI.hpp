#ifndef WRAPPER_MONITOR_MI_HPP_
#define WRAPPER_MONITOR_MI_HPP_

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
template <typename B = int, typename R = float>
class Wrapper_Monitor_MI : public Wrapper_py,
                             public py::class_<aff3ct::module::Monitor_MI<B, R>, aff3ct::module::Monitor>
{
	public:
	Wrapper_Monitor_MI(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Monitor_MI() = default;
};
}
}
#endif //WRAPPER_MONITOR_MI_HPP_