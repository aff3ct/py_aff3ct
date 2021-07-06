#include "Wrapper_py/Tools/Sequence/Sequence.hpp"

#include <pybind11/functional.h>
#include <functional>
#include <chrono>

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Sequence
::Wrapper_Sequence(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Sequence>(scope, "Sequence")
{
}


void Wrapper_Sequence
::definitions()
{
	this->def(py::init<module::Task &, module::Task &, const size_t, const bool, const std::vector<size_t> &>(), "first"_a, "last"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), py::return_value_policy::take_ownership);
	this->def(py::init<module::Task &, const size_t, const bool, const std::vector<size_t> &>(), "first"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), py::return_value_policy::take_ownership);
	this->def(py::init<const std::vector<module::Task *> &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "firsts"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true,py::return_value_policy::take_ownership);
	this->def(py::init<const std::vector<module::Task *> &, const std::vector<module::Task *> &, const std::vector<module::Task *> &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "firsts"_a, "lasts"_a, "exclusions"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true,py::return_value_policy::take_ownership);
	/*this->def(py::init<const std::vector<const module::Task *> &, const std::vector<const module::Task *> &, const std::vector<const module::Task *> &, const size_t, const bool, const std::vector<size_t> &>(), "firsts"_a, "lasts"_a, "exclusions"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>());
	this->def(py::init<const std::vector<module::Task *> &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "firsts"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true);
	this->def(py::init<const std::vector<module::Task *> &, const std::vector<module::Task *> &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "firsts"_a, "lasts"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true);
	this->def(py::init<const std::vector<module::Task *> &, const std::vector<module::Task *> &, const std::vector<module::Task *> &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "firsts"_a, "lasts"_a, "exclusions"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true);
	this->def(py::init<module::Task &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "first"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true);
	this->def(py::init<module::Task &, module::Task &, const size_t, const bool, const std::vector<size_t> &, const bool>(), "first"_a, "last"_a, "n_threads"_a = 1, "thread_pinning"_a = false, "puids"_a = std::vector<size_t>(), "tasks_inplace"_a = true);*/
	//this->def("exec", [](aff3ct::tools::Sequence& self, std::function<bool(const std::vector<const int*>&)> stop_condition){self.exec(stop_condition);});
	this->def("exec", [](aff3ct::tools::Sequence& self)
	{
		py::gil_scoped_release release{};
		self.exec(); // gil aquired before exec of stop_condition
		             // see that pybind11/functionnal.h l.62
	});
	this->def("exec_step", &aff3ct::tools::Sequence::exec_step, "tid"_a = 0, "frame_id"_a = -1, py::return_value_policy::reference);


	this->def("export_dot", [](aff3ct::tools::Sequence& self, const std::string& file_name){
		std::ofstream f(file_name);
		self.export_dot(f);
	});
	this->def("set_n_frames",  &Sequence::set_n_frames);
	this->def("show_stats", [](aff3ct::tools::Sequence& self)
	{
		py::scoped_ostream_redirect stream(
			std::cout,                               // std::ostream&
			py::module_::import("sys").attr("stdout") // Python output
		);
		tools::Stats::show(self.get_modules_per_types(), true);
	});

	this->def("get_tasks_per_types", &aff3ct::tools::Sequence::get_tasks_per_types, py::return_value_policy::reference);
	this->def("exec_step", &aff3ct::tools::Sequence::exec_step, py::return_value_policy::reference);
	this->def("get_BFER_monitors", [](const aff3ct::tools::Sequence& self){
		return self.get_modules<module::Monitor_BFER<>>();
	}, py::return_value_policy::reference);
	this->def("is_done", &aff3ct::tools::Sequence::is_done);
};

