/**
 * @file Source_user_numpy.cpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief
 * @date 2024-03-13 15:12:01
 * @modified: 2024-03-14 00:33:56
 */

#include "Module/Source/User/Source_user_numpy.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B>
Source_user_numpy<B>::Source_user_numpy(const int K, const std::vector<std::vector<B>>& input)
    : Source<B>(K),
     source(input),
     src_counter(0),
     done(false)
{
    const std::string name = "Source_user_numpy";
    this->set_name(name);

    int n_src = input.size();
    int src_size = input[0].size();

    if (n_src <= 0 || src_size <= 0) {
        std::stringstream message;
        message << "'n_src', and 'src_size' have to be greater than 0 ('n_src' = " << n_src
                << ", 'src_size' = " << src_size << ").";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }

    if (src_size == this->K) {
        this->source = input;
    } else {
        std::stringstream message;
        message << "The size is wrong (read: " << src_size << ", expected: " << this->K << ").";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }
}

template <typename B>
bool Source_user_numpy<B>::is_done() const
{
    return this->done;
}

template <typename B>
void Source_user_numpy<B>::_generate(B* U_K, const size_t frame_id)
{
    std::copy(this->source[this->src_counter].begin(), this->source[this->src_counter].end(), U_K);
    this->src_counter++;

    if (this->src_counter == this->source.size()) {
        this->src_counter = 0;
        this->done = true;
    }
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::module::Source_user_numpy<B_8>;
template class aff3ct::module::Source_user_numpy<B_16>;
template class aff3ct::module::Source_user_numpy<B_32>;
template class aff3ct::module::Source_user_numpy<B_64>;
#else
template class aff3ct::module::Source_user<B>;
#endif
// ==================================================================================== explicit template instantiation
