#ifndef WRAPPER_MONITOR_BFER_AR_HPP_
#define WRAPPER_MONITOR_BFER_AR_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

#include "Module/Monitor/BFER_AR/Monitor_BFER_AR.hpp"

#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;

namespace aff3ct
{
namespace wrapper
{
template <typename B = int>
class Wrapper_Monitor_BFER_AR : public Wrapper_py,
                             public py::class_<aff3ct::module::Monitor_BFER_AR<B>, aff3ct::module::Monitor>
{
	public:
	Wrapper_Monitor_BFER_AR(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Monitor_BFER_AR() = default;
};
}
}
#endif //WRAPPER_MONITOR_BFER_AR_HPP_