#ifndef WRAPPER_{SHORT_NAME}_HPP_
#define WRAPPER_{SHORT_NAME}_HPP_

#include <pybind11/pybind11.h>
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
{full_template}
class Wrapper_{short_name} : public Wrapper_py,
                             public py::class_<{name}{short_template}{parent}>
{
	public:
	Wrapper_{short_name}(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_{short_name}() = default;
};
}
}
#endif //WRAPPER_{SHORT_NAME}_HPP_