#ifndef BIND_MODULE_HPP_
#define BIND_MODULE_HPP_

#include <pybind11/pybind11.h>
#include <string>
#include <aff3ct.hpp>
#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;
namespace aff3ct
{
namespace module
{
class Module_Publicist : public Module
{
	public:
	using Module::Module;
	virtual ~Module_Publicist() = default;
};
}
}

namespace aff3ct
{
namespace wrapper
{
class Wrapper_Module : public Wrapper_py,
                       public py::class_<aff3ct::module::Module>

{
	public:
	Wrapper_Module(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Module() = default;
};
}
}
std::string to_string    (const aff3ct::module::Module& m, bool full = false);
std::string create_helper(const std::string& in_out, const std::string& type);
#endif //BIND_MODULE_HPP_