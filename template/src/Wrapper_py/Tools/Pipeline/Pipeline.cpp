#include "Wrapper_py/Tools/Pipeline/Pipeline.hpp"

#include <pybind11/functional.h>
#include <functional>
#include <chrono>

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;


Wrapper_Pipeline
::Wrapper_Pipeline(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::tools::Pipeline>(scope, "Pipeline")
{
}


void Wrapper_Pipeline
::definitions()
{
	this->def(py::init<const std::vector<module::Task*> &,
	                   const std::vector<module::Task*> &,
	                   const std::vector<std::tuple<std::vector<module::Task*>, std::vector<module::Task*>, std::vector<module::Task*>>> &,
	                   const std::vector<size_t>,
	                   const std::vector<size_t>,
	                   const std::vector<bool>,
	                   const std::vector<bool>,
	                   const std::vector<std::vector<size_t>> >(),
	                   "firsts"_a,
	                   "lasts"_a,
	                   "sep_stages"_a             = std::vector<std::tuple<std::vector<module::Task*>, std::vector<module::Task*>, std::vector<module::Task*>>>(),
	                   "n_threads"_a              = std::vector<size_t>(),
	                   "synchro_buffer_sizes"_a   = std::vector<size_t>(),
	                   "synchro_active_waiting"_a = std::vector<bool>(),
	                   "thread_pinning"_a         = std::vector<bool>(),
	                   "puids"_a                  = std::vector<std::vector<size_t>>(),
					   py::return_value_policy::take_ownership
			);

	this->def("exec", [](aff3ct::tools::Pipeline& self)
	{
		py::gil_scoped_release release{};
		self.exec(); // gil aquired before exec of stop_condition
		             // see that pybind11/functionnal.h l.62
	});
	/*
	this->def("exec_auto", [](aff3ct::tools::Sequence& self)
	{
		py::gil_scoped_release release;
		auto mnt_red = tools::Monitor_reduction<module::Monitor_BFER<>>(self.get_modules<module::Monitor_BFER<>>());
		mnt_red.set_reduce_frequency(std::chrono::nanoseconds(1000000));
		self.exec([&mnt_red]() { return mnt_red.is_done();}); // gil aquired before exec of stop_condition
		                              // see that pybind11/functionnal.h l.62
	});*/


	this->def("export_dot", [](aff3ct::tools::Pipeline& self, const std::string& file_name){
		std::ofstream f(file_name);
		self.export_dot(f);
	});
	this->def("set_n_frames",  &Pipeline::set_n_frames);
	this->def("get_tasks_per_types", &Pipeline::get_tasks_per_types, py::return_value_policy::reference);
	this->def("show_stats", [](aff3ct::tools::Pipeline& self)
	{
		py::scoped_ostream_redirect stream(
			std::cout,                               // std::ostream&
			py::module_::import("sys").attr("stdout") // Python output
		);
	auto stages = self.get_stages();
	for (size_t s = 0; s < stages.size(); s++)
	{
		const int n_threads = stages[s]->get_n_threads();
		std::cout << "#" << std::endl << "# Pipeline stage " << s << " (" << n_threads << " thread(s)): " << std::endl;
		tools::Stats::show(stages[s]->get_tasks_per_types(), true);
	}
	});
};

