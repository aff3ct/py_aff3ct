#ifndef WRAPPER_FROZENBITS_GENERATOR_5G_HPP_
#define WRAPPER_FROZENBITS_GENERATOR_5G_HPP_

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

class Wrapper_Frozenbits_generator_5G : public Wrapper_py,
                             public py::class_<aff3ct::tools::Frozenbits_generator_5G, aff3ct::tools::Frozenbits_generator_file>
{
	public:
	Wrapper_Frozenbits_generator_5G(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Frozenbits_generator_5G() = default;
};
}
}
#endif //WRAPPER_FROZENBITS_GENERATOR_5G_HPP_