#include <pybind11/stl.h>
#include <aff3ct.hpp>
#include <string>
#include <sstream>

#include "Wrapper_py/Module/Py_Module/Py_Module.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct::module;
using namespace aff3ct::wrapper;

Wrapper_Py_Module
::Wrapper_Py_Module(py::handle scope)
: Wrapper_py(),
  py::class_<Py_Module, Module, Py_Module_Trampoline>(scope, "Py_Module")
{
}

void Wrapper_Py_Module
::definitions()
{
	this->def(py::init<>());
	this->def(py::init<const Py_Module_Trampoline&>());
	this->def("clone",          &Py_Module::clone);
	this->def("__copy__",       &Py_Module::__copy__);
	this->def("__deepcopy__",   &Py_Module::__deepcopy__);
	this->def("__str__",        &Py_Module::to_string);
	this->def_property("n_frames", &Py_Module::get_n_frames, &Py_Module::set_n_frames);
	this->def("create_codelet", &Py_Module::create_codelet, "task"_a, "codelet"_a);

	this->def("create_task", [](Py_Module& self, const std::string &name)->Task&
	{
		return self.create_task(name, -1);
	}, "Create a new Task.", "name"_a, py::return_value_policy::reference);

	this->def("create_socket_in",    [](Py_Module& self, Task& task, const std::string& name, const size_t n_elts, py::object type)-> int
	{
		const std::string type_str (type.attr("__name__").cast<std::string>());
		int s = -1;
		if      (type_str == "int8"   ) s = self.create_socket_in<int8_t >(task, name, n_elts);
		else if (type_str == "int16"  ) s = self.create_socket_in<int16_t>(task, name, n_elts);
		else if (type_str == "int32"  ) s = self.create_socket_in<int32_t>(task, name, n_elts);
		else if (type_str == "int64"  ) s = self.create_socket_in<int64_t>(task, name, n_elts);
		else if (type_str == "float32") s = self.create_socket_in<float  >(task, name, n_elts);
		else if (type_str == "float64") s = self.create_socket_in<double >(task, name, n_elts);
		return s;
	}, "task"_a, "name"_a, "n_elts"_a, "type"_a);

	this->def("create_socket_out",    [](Py_Module& self, Task& task, const std::string& name, const size_t n_elts, py::object type)-> int
	{
		const std::string type_str (type.attr("__name__").cast<std::string>());
		int s = -1;
		if      (type_str == "int8"   ) s = self.create_socket_out<int8_t >(task, name, n_elts);
		else if (type_str == "int16"  ) s = self.create_socket_out<int16_t>(task, name, n_elts);
		else if (type_str == "int32"  ) s = self.create_socket_out<int32_t>(task, name, n_elts);
		else if (type_str == "int64"  ) s = self.create_socket_out<int64_t>(task, name, n_elts);
		else if (type_str == "float32") s = self.create_socket_out<float  >(task, name, n_elts);
		else if (type_str == "float64") s = self.create_socket_out<double >(task, name, n_elts);
		return s;
	}, "task"_a, "name"_a, "n_elts"_a, "type"_a);
};