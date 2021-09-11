#include "{path}/{short_name}_{type}.hpp"

namespace py = pybind11;
using namespace py::literals;
using namespace aff3ct;
using namespace aff3ct::module;
using namespace aff3ct::tools;
using namespace aff3ct::wrapper;

template <typename B,typename R>
Wrapper_{short_name}_{type}<B,R>
::Wrapper_{short_name}_{type}(py::handle scope)
: Wrapper_py(),
  py::class_<aff3ct::module::{short_name}<B,R,tools::Update_rule_{type}{simd}<R>>,aff3ct::module::Decoder_SISO<B,R>>(scope, "{short_name}_{type}")
{
}

template <typename B,typename R>
void Wrapper_{short_name}_{type}<B,R>
::definitions()
{
    this->def(py::init([](const int K, const int N, const int n_ite, const tools::Sparse_matrix & H, const std::vector< unsigned > &info_bits_pos, {norm_factor_decl}{offset_decl}const bool enable_syndrome, const int syndrome_depth){
        {max_CN_degree_decl}
        return new module::{short_name}<B,R,tools::Update_rule_{type}{simd}<R>>(K, N, n_ite, H, info_bits_pos, tools::Update_rule_{type}{simd}<R>({args}), enable_syndrome, syndrome_depth);
    }),"K"_a, "N"_a, "n_ite"_a, "H"_a, "info_bits_pos"_a, {offset_arg}{norm_factor_arg}"enable_syndrome"_a = true, "syndrome_depth"_a = 1, R"pbdoc()pbdoc", py::return_value_policy::take_ownership);
};

#include "Tools/types.h"
template class aff3ct::wrapper::Wrapper_{short_name}_{type}<int,float>;