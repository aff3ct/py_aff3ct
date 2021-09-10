#ifndef WRAPPER_SPARSE_MATRIX_HPP_
#define WRAPPER_SPARSE_MATRIX_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>
#include <pybind11/numpy.h>
#include <iostream>
#include <fstream>      // std::filebuf
#include <aff3ct.hpp>

#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;

namespace aff3ct
{
namespace wrapper
{

class Wrapper_Sparse_matrix : public Wrapper_py,
                             public py::class_<aff3ct::tools::Sparse_matrix>
{
	public:
	Wrapper_Sparse_matrix(py::module_& scope);
	virtual void definitions();
	virtual ~Wrapper_Sparse_matrix() = default;

	static py::slice get_slice(const py::object& obj, const size_t len);
};
}
}
#endif //WRAPPER_SPARSE_MATRIX_HPP_