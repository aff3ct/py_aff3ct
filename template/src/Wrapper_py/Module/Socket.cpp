#include <sstream>
#include <string>
#include <memory>
#include <map>
#include <pybind11/numpy.h>
#include "Wrapper_py/Module/Socket.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct::wrapper;
using namespace aff3ct::module;

std::map<std::type_index, std::string> type_map = {{typeid(int8_t ), py::format_descriptor<int8_t >::format()},
                                                   {typeid(int16_t), py::format_descriptor<int16_t>::format()},
                                                   {typeid(int32_t), py::format_descriptor<int32_t>::format()},
                                                   {typeid(int64_t), py::format_descriptor<int64_t>::format()},
                                                   {typeid(float  ), py::format_descriptor<float  >::format()},
                                                   {typeid(double ), py::format_descriptor<double >::format()}};

Wrapper_Socket
::Wrapper_Socket(py::handle scope)
: Wrapper_py(),
  py::class_<Socket, std::shared_ptr<Socket>>(scope, "Socket", py::buffer_protocol())
{
}

void Wrapper_Socket
::definitions()
{
	this->def_buffer([](Socket &s) -> py::buffer_info{
	if (s.get_name() == "status")
	{
		size_t n_w = (size_t)s.get_task().get_module().get_n_waves();
		return py::buffer_info(
			s.get_dataptr(),            /* Pointer to buffer */
			s.get_datatype_size(),      /* Size of one scalar */
			type_map[s.get_datatype()], /* Python struct-style format descriptor */
			1,                          /* Number of dimensions */
			{n_w},                        /* Buffer dimensions */
			{(size_t)s.get_datatype_size()}
		);
	}
	else
	{
		size_t n_frames = s.get_task().get_module().get_n_frames();
		size_t n_row    = n_frames;
		size_t n_col    = s.get_n_elmts()/n_frames;
		return py::buffer_info(
			s.get_dataptr(),            /* Pointer to buffer */
			s.get_datatype_size(),      /* Size of one scalar */
			type_map[s.get_datatype()], /* Python struct-style format descriptor */
			2,                          /* Number of dimensions */
			{n_row, n_col},             /* Buffer dimensions */
			{(size_t)s.get_datatype_size()*n_col, (size_t)s.get_datatype_size()}
		);
	}
	});

	this->def("__getitem__", [](aff3ct::module::Socket& sckt, py::handle& index) {
		py::array array = py::cast(sckt);
		return array.attr("__getitem__")(index);
		},py::return_value_policy::reference);

	this->def("__setitem__", [](aff3ct::module::Socket& sckt, py::handle& index, py::handle& value) {
		py::array arr = py::cast(sckt);
		arr.attr("__setitem__")(index, value);
		},py::return_value_policy::reference);

	this->def("bind", [](aff3ct::module::Socket& self, aff3ct::module::Socket& s_out, const int priority)
	{
		self.bind(s_out, priority);
	}, "Binds the socket to socket 's_out' with priority 'priority'.", "s_out"_a, "priority"_a=1);

	this->def("bind", [](aff3ct::module::Socket& self, py::array& arr)
	{
		size_t n_row = (size_t)self.get_task().get_module().get_n_frames();
		size_t n_col = (size_t)self.get_n_elmts()/n_row;

		py::buffer_info buffer = arr.request();
		if ((size_t)buffer.shape[0] != n_row || (size_t)buffer.shape[1] != n_col)
		{
			std::stringstream message;
			message << "The shape of the array must match the socket one.";
			message << "Socket shape: " << n_row << " x " << n_col << ".";
			message << "Array shape: " << buffer.shape[0] << " x " << buffer.shape[1] << ".\n";
			throw std::runtime_error(message.str());
		}

		py::array py_self = py::cast(self);
		if (!arr.dtype().is(py_self.dtype()))
		{
			std::stringstream message;
			message << "The dtype of the array must match the socket one.";
			message << "Socket dtype: " << py_self.dtype().attr("name").cast<std::string>() << ".";
			message << "Array dtype: " << arr.dtype().attr("name").cast<std::string>() << ".\n";
			throw std::runtime_error(message.str());
		}

		self.bind(buffer.ptr);
	}, "Binds the socket to the numpy array 'array' with priority 'priority'.", "array"_a);
	this->def_property_readonly("name", &aff3ct::module::Socket::get_name);
	this->def("__deepcopy__", [](const aff3ct::module::Socket &self, py::dict) {return aff3ct::module::Socket(self);}, "memo"_a);
	this->def("info", [](const aff3ct::module::Socket& s) {py::print(Wrapper_Socket::to_string(s).c_str());}, "Print module information.");
	this->def("full_info", [](const aff3ct::module::Socket& s) {py::print(Wrapper_Socket::to_string(s, true).c_str());}, "Print module information with additionnal information.");
	this->def_property_readonly("direction", [](const aff3ct::module::Socket& self)
	{
		aff3ct::module::Task&   t = self.get_task();
		if (t.get_socket_type(self) == socket_t::SIN )
			return("in");
		else if (t.get_socket_type(self) == socket_t::SOUT )
			return("out");
		else // This should not happen
		{
			std::stringstream message;
			message << "Unknown socket direction.";
			throw std::runtime_error(message.str());
		}
	});
};

std::string Wrapper_Socket
::to_string(const aff3ct::module::Socket& s, int idx, bool full, const std::string prefix)
{
	std::stringstream message;
	message << prefix.c_str() << "- Socket ";
	if (idx >= 0)
		message << idx;
	message << "\n";

	aff3ct::module::Task&   t = s.get_task();
	aff3ct::module::Module& m = t.get_module();

	std::string type;
	if (t.get_socket_type(s) == socket_t::SIN )
		type = "in";
	else if (t.get_socket_type(s) == socket_t::SOUT )
		type = "out";

	message  << prefix.c_str() << rang::style::bold << rang::fg::blue << "\t- Name              : " << s.get_name() << rang::style::reset << "\n";
	message  << prefix.c_str() << "\t- Type              : " << type.c_str() << "\n";
	message  << prefix.c_str() << "\t- Elements per frame: " << s.get_n_elmts()/m.get_n_frames() << "\n";
	message  << prefix.c_str() << "\t- Data type         : " << s.get_datatype_string() << "\n";
	if (full)
	{
		message  << prefix.c_str() << "\t- Data bytes        : " << s.get_databytes() << "\n";
		message  << prefix.c_str() << "\t- Data ptr          : " << s.get_dataptr() << "\n";
		message  << prefix.c_str() << "\t- Address           : " << std::hex << static_cast<const void*>(&s) << "\n\n";
	}

	return message.str();
}