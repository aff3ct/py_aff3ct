#ifndef BIND_TASK_HPP_
#define BIND_TASK_HPP_

#include <pybind11/pybind11.h>
#include <memory>
#include <aff3ct.hpp>
#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;

namespace aff3ct
{
namespace module
{
class Task_Publicist : public Task
{
	public:
	using Task::Task;
	using Task::codelet;

	virtual ~Task_Publicist() = default;
};
}
}

namespace aff3ct
{
namespace wrapper
{
class Wrapper_Task : public Wrapper_py,
                     public py::class_<aff3ct::module::Task,
                                       std::shared_ptr<aff3ct::module::Task>,
                                       aff3ct::module::Task_Publicist>
{
	public:
	Wrapper_Task(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Task() = default;
};
}
}

#endif //BIND_TASK_HPP_