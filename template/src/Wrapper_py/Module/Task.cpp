#include <pybind11/stl.h>
#include "Wrapper_py/Module/Task.hpp"
#include "Wrapper_py/Module/Socket.hpp"
#include <pybind11/iostream.h>
#include <rang.hpp>

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct::module;
using namespace aff3ct::wrapper;

Wrapper_Task
::Wrapper_Task(py::handle scope)
: Wrapper_py(),
  py::class_<Task, std::shared_ptr<Task>, Task_Publicist>(scope, "Task")
{
}

void Wrapper_Task
::definitions()
{
	this->def_property_readonly("name",    &aff3ct::module::Task::get_name);
	this->def_property_readonly("sockets", [](aff3ct::module::Task& t) -> std::vector<std::shared_ptr<aff3ct::module::Socket>>
	{
		return t.sockets;
	});

	this->def("exec",
		[](Task &self, const int frame_id, const bool managed_memory)
		{
			py::scoped_ostream_redirect stream(
			std::cout,                                // std::ostream&
			py::module_::import("sys").attr("stdout") // Python output
			);
			//setControlMode(rang::control::Force);
			self.exec(frame_id, managed_memory);
		},
		"frame_id"_a = -1, "managed_memory"_a = true);
	this->def_property("debug", &Task::is_debug, &Task::set_debug);
	this->def_property("stats", &Task::is_stats, &Task::set_stats);
	this->def_property("fast" , &Task::is_fast,  &Task::set_fast );

	this->def("set_debug_hex      ", &Task::set_debug_hex      , "debug_hex"_a);
	this->def("set_debug_limit    ", &Task::set_debug_limit    , "limit"_a    );
	this->def("set_debug_precision", &Task::set_debug_precision, "prec"_a     );
	this->def("set_debug_frame_max", &Task::set_debug_frame_max, "limit"_a    );
	this->def("info", [](const aff3ct::module::Task& t) {py::print(Wrapper_Task::to_string(t).c_str());}, "Print module information.");
	this->def("full_info", [](const aff3ct::module::Task& t) {py::print(Wrapper_Task::to_string(t, true).c_str());}, "Print module information with additionnal information.");


	this->def_property_readonly("tasks", [](aff3ct::module::Task_Publicist& self) { return self.codelet; });
};

std::string Wrapper_Task
::to_string(const aff3ct::module::Task& t, int idx, bool full, const std::string prefix)
{
	std::stringstream message;
	message << prefix.c_str() << "- Task ";
	if (idx >= 0)
		message << idx;
	message << "\n";

	std::string name = t.get_name();
	message << prefix.c_str() << rang::style::bold << rang::fg::magenta << "\t- Name         : " << name << rang::style::reset << "\n";
	if (full)
	{
		message << prefix.c_str() << "\t- Address       : " <<  std::hex << static_cast<const void*>(&t) << "\n";
		message << prefix.c_str() << "\t- Module address: " <<  std::hex << static_cast<const void*>(&t.get_module()) << "\n";
	}
	if (t.sockets.size() > 0)
		message << prefix.c_str() << rang::style::bold << rang::fg::blue << "\t- Sockets      : " << rang::style::reset <<  "\n";
	else
		message << prefix.c_str() << rang::style::bold << rang::fg::blue << "\t- No Socket." << rang::style::reset << "\n";


	std::stringstream prefix_;
	prefix_ << prefix.c_str() << "\t";
	for (size_t j = 0 ; j < t.sockets.size() ; j++)
	{
		message << Wrapper_Socket::to_string(*t.sockets[j], j, full, prefix_.str()).c_str();
	}
	return message.str();
}