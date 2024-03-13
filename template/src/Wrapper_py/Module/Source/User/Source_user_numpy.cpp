/**
 * @file Source_user_numpy.cpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief 
 * @date 2024-03-13 20:04:08
 * @modified: 2024-03-13 21:13:55
 */
#include "Wrapper_py/Module/Source/User/Source_user_numpy.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B>
Wrapper_Source_user_numpy<B>
::Wrapper_Source_user_numpy(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::Source_user_numpy<B>,aff3ct::module::Source<B>>(scope, "Source_user_numpy")
{
}

template <typename B>
void Wrapper_Source_user_numpy<B>
::definitions()
{
    this->def(py::init<const int, const std::vector<std::vector<B>> &>(),"K"_a, "input"_a, R"pbdoc()pbdoc", py::return_value_policy::take_ownership);
};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_Source_user_numpy<int32_t>;