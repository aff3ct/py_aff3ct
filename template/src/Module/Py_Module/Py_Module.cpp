#include <sstream>
#include <algorithm>
#include <exception>
#include <memory>

#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

#include "Py_Module.hpp"

using namespace aff3ct;
using namespace aff3ct::module;
namespace py = pybind11;

Py_Module
::Py_Module()
: Module(), done_flag(false)
{
	const std::string name = "Py_Module";
	this->set_name(name);
	this->set_short_name(name);

	this->set_single_wave(true);
}

Py_Module
::Py_Module(const Py_Module& ref)
: Module(ref), done_flag(false)
{
	this->set_name(ref.get_name());
	this->set_short_name(ref.get_name());
	if (ref.has_child())
		this->child.reset(new py::object(ref.get_child()));
	else
		this->child.reset(new py::object(py::cast(ref)));
}

void Py_Module
::create_codelet(Task& task, const py::function& codelet)
{
	Module::create_codelet(task,[codelet](Module &m, Task &t, const size_t f)->int
	{
		py::gil_scoped_acquire acquire{};
		auto py_m = py::cast(static_cast<Py_Module&>(m));
		py::list l;
		for (size_t i = 0; i < t.sockets.size()-1; i++) // I don't pass the STATUS here
			l.append(py::array(py::cast(t.sockets[i])));

		return codelet(py_m,l,f).cast<int>();
	});
}

void Py_Module
::create_fake_codelet(Task& task)
{
	Module::create_codelet(task,[](Module &m, Task &t, const size_t f)->int
	{
		return 0;
	});

}

void Py_Module
::set_n_frames_per_wave(const size_t n_frames_per_wave)
{
	if(this->child.get() != nullptr)
	{
		auto cpp_child = py::cast<Py_Module&>(*this->child);
		cpp_child.set_n_frames_per_wave(n_frames_per_wave);
		this->n_frames_per_wave = n_frames_per_wave;
	}
	else
	{
		Module::set_n_frames_per_wave(n_frames_per_wave);
	}

}

bool Py_Module
::has_child() const
{
	return this->child.get() != nullptr;
}

py::object Py_Module
::get_child() const
{
	return *this->child;
}

void Py_Module
::set_n_frames(const size_t n_frames)
{
	if(this->has_child())
	{
		auto& cpp_child = py::cast<Py_Module&>(*this->child);
		cpp_child.set_n_frames(n_frames);
		this->n_frames = n_frames;
	}
	else
	{
		this->Module::set_n_frames(n_frames);
	}
}

py::object Py_Module
::__copy__() const
{
	// Cast it to py::object, this should work if the Module is a Py_Module
	py::object py_module = py::cast(*this);
	// Recover the python type
	py::type      type = py_module.attr("__class__");
	// create a new object without initializing it
	py::object     copy_ = py_module.attr("__new__")(type);
	// clone C++ state using copy constructor of Py_Module (copy Tasks, Sockets ...)
	py::module::import("py_aff3ct").attr("module").attr("py_module").attr("Py_Module").attr("__init__")(copy_, py_module);
	// clone Python state
	py::object py_deepcopy  = py::module::import("copy").attr("deepcopy");
	try
	{
		py::object cpy_dict = py_deepcopy(py_module.attr("__dict__"));
		copy_.attr("__dict__").attr("update")(cpy_dict);
	}
	catch(const std::exception& e)
	{
	}
	Py_Module& copy = py::cast<Py_Module&>(copy_);
	if (copy.has_child())
		copy.child.reset();

	return copy_;
}

py::object Py_Module
::__deepcopy__() const
{
	py::object py_copy = this->__copy__();
	Py_Module& copy = py::cast<Py_Module&>(py_copy);
	copy.deep_copy(*this); // Rebuild C++ state with new Tasks / Sockets
	return py_copy;
}

Py_Module* Py_Module
::clone() const
{
	try
	{
		// https://github.com/pybind/pybind11/issues/1049
		py::object* py_clone = new py::object(this->__deepcopy__());
		// Create new Py_Module from a casted version of py_clone
		// py_clone will be the child of cpp_clone
		auto cpp_clone = new  Py_Module(py::cast<Py_Module&>(*py_clone));
		return cpp_clone;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		throw(e);
	}
}

std::string Py_Module
::to_string() const
{
	py::object self = py::cast(*this);
	std::stringstream message;
	message << "----------------------------- Module -----------------------------" << std::endl;
	message << "- Name         : " << this->get_name()         << "\n";
	message << "- Address      : " <<  std::hex << static_cast<const void*>(this) << "\n";
	message << "- Class        : " << self.attr("__class__").attr("__name__").cast<std::string>() << "\n";
	message << "- Frames number: " << this->get_n_frames()     << "\n";
	message << "- Task number  : " << this->tasks.size()       << "\n";

	if (this->tasks.size() > 0)
		message << "- Tasks        :" << "\n";
	for (size_t i = 0 ; i < this->tasks.size() ; i++)
	{
		message << "\t- Task " << i << "\n";
		std::string name = this->tasks[i]->get_name();
		message << "\t\t- Name          : " << name << "\n";
		message << "\t\t- Address       : " <<  std::hex << static_cast<void*>(this->tasks[i].get()) << "\n";
		message << "\t\t- Module address: " <<  std::hex << static_cast<void*>(&this->tasks[i]->get_module()) << "\n";
		message << "\t\t- Socket number : " << this->tasks[i]->sockets.size() << "\n";
		if (this->tasks[i]->sockets.size() > 0)
			message << "\t\t- Sockets      : " << "\n";
		for (size_t j = 0 ; j < this->tasks[i]->sockets.size() ; j++)
		{
			message << "\t\t\t- Socket " << j << "\n";
			std::string type;
			if (this->tasks[i]->get_socket_type(*this->tasks[i]->sockets[j]) == socket_t::SIN )
				type = "in";
			else if (this->tasks[i]->get_socket_type(*this->tasks[i]->sockets[j]) == socket_t::SOUT )
				type = "out";

			message << "\t\t\t\t- Type              : " << type.c_str() << "\n";
			message << "\t\t\t\t- Name              : " << this->tasks[i]->sockets[j]->get_name() << "\n";
			message << "\t\t\t\t- Elements per frame: " << this->tasks[i]->sockets[j]->get_n_elmts()/this->get_n_frames() << "\n";
			message << "\t\t\t\t- Data type         : " << this->tasks[i]->sockets[j]->get_datatype_string() << "\n";
			message << "\t\t\t\t- Data bytes        : " << this->tasks[i]->sockets[j]->get_databytes() << "\n";
			message << "\t\t\t\t- Data ptr          : " << this->tasks[i]->sockets[j]->get_dataptr() << "\n";
			message << "\t\t\t\t- Address           : " << std::hex << static_cast<void*>(this->tasks[i]->sockets[j].get()) << "\n\n";
		}
	}
	if (this->has_child())
	{
		message << "- Child : \n";
		auto & cpp_child = py::cast<Py_Module&>(*this->child);
		message << cpp_child.to_string();
	}
	else
	{
		message << "- Child : no child\n";
	}


	return message.str();
}

bool Py_Module
::is_done() const
{
	return this->done_flag;
}

void Py_Module
::toggle_done()
{
	this->done_flag = true;
}