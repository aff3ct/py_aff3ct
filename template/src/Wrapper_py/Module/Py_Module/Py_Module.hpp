#ifndef BIND_PY_MODULE_HPP_
#define BIND_PY_MODULE_HPP_
#include <pybind11/pybind11.h>

#include "Module/Py_Module/Py_Module.hpp"
#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;

namespace aff3ct
{
namespace module
{
class Py_Module_Trampoline : public Py_Module
{
	public:
		using Py_Module::Py_Module;
		py::object __copy__()     const override {PYBIND11_OVERLOAD(py::object, Py_Module, __copy__,     ); };
		py::object __deepcopy__() const override {PYBIND11_OVERLOAD(py::object, Py_Module, __deepcopy__, ); };
};
}
}

namespace aff3ct
{
namespace wrapper
{
class Wrapper_Py_Module : public Wrapper_py,
                          public py::class_<aff3ct::module::Py_Module,
                                 aff3ct::module::Module,
                                 aff3ct::module::Py_Module_Trampoline >
{
	public:
	Wrapper_Py_Module(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Py_Module() = default;
};
}
}

#endif //BIND_PY_MODULE_HPP_