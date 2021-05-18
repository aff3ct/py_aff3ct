#ifndef WRAPPER_PIPELINE_HPP_
#define WRAPPER_PIPELINE_HPP_

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

class Wrapper_Pipeline : public Wrapper_py,
                         public py::class_<aff3ct::tools::Pipeline>
{
	public:
	Wrapper_Pipeline(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Pipeline() = default;
};
}
}
#endif //WRAPPER_PIPELINE_HPP_