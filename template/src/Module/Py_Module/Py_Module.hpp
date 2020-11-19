#ifndef PY_MODULE_HPP_
#define PY_MODULE_HPP_

#include <string>
#include <aff3ct.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace aff3ct
{
namespace module
{
class Py_Module : public Module
{
private:
	std::shared_ptr<py::object> child;

public:
	Py_Module();
	Py_Module(const Py_Module& );

	virtual ~Py_Module() =default;

	using Module::create_task;
	using Module::create_socket_in;
	using Module::create_socket_out;

	virtual Py_Module* clone() const;
	virtual py::object __copy__() const;
	virtual py::object __deepcopy__() const;
	virtual void set_n_frames_per_wave(const size_t n_frames_per_wave);
	virtual void set_n_frames         (const size_t n_frames         );

	void create_codelet(Task& task, const py::function& codelet);
	std::string to_string() const;
	bool has_child() const;
	py::object get_child() const;

};
}
}

#endif /* PY_MODULE_HPP_ */
