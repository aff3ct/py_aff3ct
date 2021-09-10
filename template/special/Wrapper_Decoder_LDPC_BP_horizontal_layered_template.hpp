#ifndef WRAPPER_{SHORT_NAME}_{TYPE}_HPP_
#define WRAPPER_{SHORT_NAME}_{TYPE}_HPP_

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
template <typename B = int,typename R = float>
class Wrapper_{short_name}_{type} : public Wrapper_py,
                             public py::class_<aff3ct::module::{short_name}<B,R,tools::Update_rule_{type}{simd}<R>>, aff3ct::module::Decoder_SISO<B,R>>
{
	public:
	Wrapper_{short_name}_{type}(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_{short_name}_{type}() = default;
};
}
}
#endif //WRAPPER_{short_name}_{TYPE}_HPP_