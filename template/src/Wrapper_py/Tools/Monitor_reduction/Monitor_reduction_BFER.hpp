#ifndef WRAPPER_MONITOR_REDUCTION_HPP_
#define WRAPPER_MONITOR_REDUCTION_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <fstream>
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

class Wrapper_Monitor_reduction_BFER : public Wrapper_py,
                                       public py::class_<aff3ct::tools::Monitor_reduction<module::Monitor_BFER<>>,
                                                         aff3ct::module::Monitor_BFER<>>
{
public:
	Wrapper_Monitor_reduction_BFER(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Monitor_reduction_BFER() = default;
};
}
}
#endif //WRAPPER_MONITOR_REDUCTION_HPP_