#ifndef WRAPPER_SEQUENCE_HPP_
#define WRAPPER_SEQUENCE_HPP_

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

class Wrapper_Sequence : public Wrapper_py,
                             public py::class_<aff3ct::tools::Sequence>
{
	public:
	Wrapper_Sequence(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Sequence() = default;
};
}
}
#endif //WRAPPER_SEQUENCE_HPP_