#ifndef BIND_SOCKET_HPP_
#define BIND_SOCKET_HPP_

#include <pybind11/pybind11.h>
#include <aff3ct.hpp>

#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;

namespace aff3ct
{
namespace wrapper
{
class Wrapper_Socket : public Wrapper_py,
                       public py::class_<aff3ct::module::Socket, std::shared_ptr<aff3ct::module::Socket>>
{
	public:
	Wrapper_Socket(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Socket() = default;
};
}
}
#endif //BIND_SOCKET_HPP_