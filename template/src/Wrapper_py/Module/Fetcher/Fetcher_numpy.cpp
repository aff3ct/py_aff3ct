/**
 * @file Fetcher_numpy.cpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief 
 * @date 2024-03-14 14:58:53
 * @modified: 2024-03-15 12:11:34
 */

#include "Wrapper_py/Module/Fetcher/Fetcher_numpy.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B>
Wrapper_Fetcher_numpy<B>
::Wrapper_Fetcher_numpy(py::handle scope, const std::string& type)
: Wrapper_py(),
  py::class_<aff3ct::module::Fetcher_numpy<B>,aff3ct::module::Module>(scope, std::string("Fetcher_numpy_" + type).c_str())
{
}

template <typename B>
void Wrapper_Fetcher_numpy<B>
::definitions()
{
    this->def(py::init<const int, const int>(),"K"_a, "N"_a, R"pbdoc()pbdoc", py::return_value_policy::take_ownership);
    this->def("get_K", &aff3ct::module::Fetcher_numpy<B>::get_K, R"pbdoc()pbdoc");
    this->def("get_N", &aff3ct::module::Fetcher_numpy<B>::get_N, R"pbdoc()pbdoc");
    this->def("append", &aff3ct::module::Fetcher_numpy<B>::append, R"pbdoc()pbdoc");
    this->def("get_data", &aff3ct::module::Fetcher_numpy<B>::get_data, R"pbdoc()pbdoc");
    this->def("is_done", &aff3ct::module::Fetcher_numpy<B>::is_done, R"pbdoc()pbdoc");
    this->def("reset", &aff3ct::module::Fetcher_numpy<B>::reset, R"pbdoc()pbdoc");
    this->def_property_readonly("tasks", [](Fetcher_numpy<B>& self)-> std::vector<std::shared_ptr<Task>> { return self.tasks; },R"pbdoc(List of tasks:
* **append**: Task method method appends a vector to the buffer.

    * U_K: a vector of bits to append.
)pbdoc");
    this->doc() = R"pbdoc(Fetches a message.

Parameters:

    * B: type of the bits in the Fetcher.
)pbdoc";
};

#include "Tools/types.h"

template class aff3ct::wrapper::Wrapper_Fetcher_numpy<int8_t>;
template class aff3ct::wrapper::Wrapper_Fetcher_numpy<int16_t>;
template class aff3ct::wrapper::Wrapper_Fetcher_numpy<int32_t>;
template class aff3ct::wrapper::Wrapper_Fetcher_numpy<int64_t>;
template class aff3ct::wrapper::Wrapper_Fetcher_numpy<float>;
template class aff3ct::wrapper::Wrapper_Fetcher_numpy<double>;
