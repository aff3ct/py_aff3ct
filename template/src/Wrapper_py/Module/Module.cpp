#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/functional.h>
#include <functional>
#include <iostream>
#include <sstream>

#include "Wrapper_py/Module/Module.hpp"
#include "Wrapper_py/Module/Task.hpp"

std::string create_helper(const std::string& in_out, const std::string& type)
{
	std::stringstream message;
	message << "Create a new " << in_out << " socket for 'n_elt' elements of type '";
	message << type << "'.";
	return message.str();
};

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct::module;
using namespace aff3ct::wrapper;

Wrapper_Module
::Wrapper_Module(py::handle scope)
: Wrapper_py(),
  py::class_<Module>(scope, "Module")
{
}

void Wrapper_Module
::definitions()
{
	this->def(py::init<>(), py::return_value_policy::reference);
	this->def_property_readonly("tasks", [](Module& self) -> std::vector<std::shared_ptr<Task>> { return self.tasks; });
	this->def_property("n_frames", &Module::get_n_frames   , &Module::set_n_frames);
	this->def_property("name"    , &Module::get_custom_name, &Module::set_custom_name);
	this->def("__getitem__",  [](Module& m, const std::string& s) { return &m[s];}, py::return_value_policy::reference);
	this->def("__call__",     [](Module& m, const std::string& s) { return &m(s);}, py::return_value_policy::reference);
	this->def("info", [](Module& m) {py::print(to_string(m).c_str());}, "Print module information.");
	this->def("full_info", [](Module& m) {py::print(to_string(m, true).c_str());}, "Print module information with additionnal information.");

};

std::string to_string(const aff3ct::module::Module& m, bool full)
{
	py::object self = py::cast(m);
	std::stringstream message;
	message << "-----------------------------" << m.get_name() <<  "-----------------------------" << std::endl;
	message << rang::style::bold << rang::fg::green << "- Name         : " << m.get_custom_name()  << rang::style::reset << "\n";
	if (full)
		message << "- Address      : " <<  std::hex << static_cast<const void*>(&m) << "\n";
	message << "- Class        : " << self.attr("__class__").attr("__name__").cast<std::string>() << "\n";
	message << "- Frames number: " << m.get_n_frames()     << "\n";
	if (m.tasks.size() > 0)
		message << rang::style::bold << rang::fg::magenta << "- Tasks        :" << rang::style::reset << "\n";
	else
		message << "- No Task." << "\n";

	for (size_t i = 0 ; i < m.tasks.size() ; i++)
	{
		message << Wrapper_Task::to_string(*m.tasks[i], i, full, "\t").c_str();
	}
	return message.str();
}