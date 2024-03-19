/**
 * @file Tunnel_numpy.hpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief 
 * @date 2024-03-14 14:59:01
 * @modified: 2024-03-15 15:55:05
 */

#ifndef WRAPPER_TUNNEL_NUMPY_HPP_
#define WRAPPER_TUNNEL_NUMPY_HPP_

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/iostream.h>

#include "Module/Tunnel/Tunnel_numpy.hpp"

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
class Wrapper_Tunnel_numpy : public Wrapper_py,
                             public py::class_<aff3ct::module::Tunnel_numpy<B>, aff3ct::module::Module>
{
    public:
    Wrapper_Tunnel_numpy(py::handle scope, const std::string& type);
    virtual void definitions();
    virtual ~Wrapper_Tunnel_numpy() = default;
};
}
}
#endif //WRAPPER_TUNNEL_NUMPY_HPP_
