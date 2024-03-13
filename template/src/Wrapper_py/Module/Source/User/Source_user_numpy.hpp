/**
 * @file Source_user_numpy.hpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief 
 * @date 2024-03-13 20:04:16
 * @modified: 2024-03-13 21:32:45
 */

#ifndef WRAPPER_SOURCE_USER_NUMPY_HPP_
#define WRAPPER_SOURCE_USER_NUMPY_HPP_


#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

#include "Module/Source/User/Source_user_numpy.hpp"

#include "Wrapper_py/Wrapper_py.hpp"

namespace py = pybind11;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;

namespace aff3ct
{
namespace wrapper
{
template <typename B = int32_t>
class Wrapper_Source_user_numpy : public Wrapper_py,
                             public py::class_<aff3ct::module::Source_user_numpy<B>, aff3ct::module::Source<B>>
{
    public:
    Wrapper_Source_user_numpy(py::handle scope);
    virtual void definitions();
    virtual ~Wrapper_Source_user_numpy() = default;
};
}
}
#endif //WRAPPER_SOURCE_USER_NUMPY_HPP_