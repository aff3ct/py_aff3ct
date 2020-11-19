#ifndef WRAPPER_PY_HPP_
#define WRAPPER_PY_HPP_
#include <aff3ct.hpp>
#include <pybind11/pybind11.h>
namespace py = pybind11;

namespace aff3ct
{
namespace wrapper
{
class Wrapper_py
{
public:
	Wrapper_py();
	virtual void definitions();
	virtual ~Wrapper_py() = default;
};
}
}

#endif