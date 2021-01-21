#ifndef BIND_GAUSSIAN_NOISE_GENERATOR_IMPLEM_HPP_
#define BIND_GAUSSIAN_NOISE_GENERATOR_IMPLEM_HPP_

#include <pybind11/pybind11.h>
#include <string>
#include <aff3ct.hpp>
#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;
using namespace aff3ct;
using namespace aff3ct::tools;

namespace aff3ct
{
namespace wrapper
{
class Wrapper_Gaussian_noise_generator_implem : public Wrapper_py,
                                                public py::enum_<aff3ct::tools::Gaussian_noise_generator_implem>

{
	public:
	Wrapper_Gaussian_noise_generator_implem(py::handle scope);
	virtual void definitions();
	virtual ~Wrapper_Gaussian_noise_generator_implem() = default;
};
}
}
#endif //BIND_GAUSSIAN_NOISE_GENERATOR_IMPLEM_HPP_