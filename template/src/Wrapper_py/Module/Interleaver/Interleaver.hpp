#ifndef WRAPPER_INTERLEAVER_HPP_
#define WRAPPER_INTERLEAVER_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <string>
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
template <typename D = int32_t,typename T = uint32_t>
class Wrapper_Interleaver : public Wrapper_py,
                            public py::class_<aff3ct::module::Interleaver<D,T>, aff3ct::module::Module>
{
	public:
	Wrapper_Interleaver(py::handle scope, const std::string& type);
	virtual void definitions();
	virtual ~Wrapper_Interleaver() = default;
};
}
}
#endif //WRAPPER_INTERLEAVER_HPP_