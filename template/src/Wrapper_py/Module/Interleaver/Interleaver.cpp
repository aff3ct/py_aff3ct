#include "Wrapper_py/Module/Interleaver/Interleaver.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename D,typename T>
Wrapper_Interleaver<D,T>
::Wrapper_Interleaver(py::handle scope, const std::string& type)
: Wrapper_py(),
  py::class_<aff3ct::module::Interleaver<D,T>,aff3ct::module::Module>(scope, std::string("Interleaver_"+ type).c_str())
{
}

template <typename D,typename T>
void Wrapper_Interleaver<D,T>
::definitions()
{
	this->def(py::init<const tools::Interleaver_core<T>&>(),"core"_a, py::return_value_policy::reference);
};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_Interleaver<int8_t, uint32_t>;
template class aff3ct::wrapper::Wrapper_Interleaver<int16_t,uint32_t>;
template class aff3ct::wrapper::Wrapper_Interleaver<int32_t,uint32_t>;
template class aff3ct::wrapper::Wrapper_Interleaver<int64_t,uint32_t>;
template class aff3ct::wrapper::Wrapper_Interleaver<float,  uint32_t>;
template class aff3ct::wrapper::Wrapper_Interleaver<double, uint32_t>;