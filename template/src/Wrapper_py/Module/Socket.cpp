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
		if (buffer.shape[0] !=n_row || buffer.shape[1] !=n_col)
		{
			std::stringstream message;
			message << "The shape of the array must match the socket one.";
			message << "Socket shape: " << n_row << " x " << n_col << ".";
			message << "Array shape: " << buffer.shape[0] << " x " << buffer.shape[1] << ".\n";
			throw std::runtime_error(message.str());
		}



		py::array py_self = py::cast(self);
		py::print(arr.dtype());
		py::print(py_self.dtype());
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
};