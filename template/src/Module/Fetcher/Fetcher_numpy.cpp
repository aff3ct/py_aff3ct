/**
 * @file Fetcher_numpy.cpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief
 * @date 2024-03-13 18:00:38
 * @modified: 2024-03-15 10:37:58
 */

#include "Module/Fetcher/Fetcher_numpy.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B>
Task& Fetcher_numpy<B>::operator[](const ftr::tsk t)
{
    switch (t) {
    case ftr::tsk::append:
        return Module::operator[](static_cast<size_t>(ftr::tsk::append));
    default:
        break;
    }

    std::stringstream message;
    message << "Unknown task";
    throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
}

template <typename B>
Socket& Fetcher_numpy<B>::operator[](const ftr::sck::append s)
{
    return Module::operator[]((size_t)ftr::tsk::append)[(size_t)s];
    // return this->get_socket(static_cast<size_t>(s));
}

template <typename B>
Fetcher_numpy<B>::Fetcher_numpy(const int K, const int N)
    : Module()
    , K(K)
    , N(N)
{
    const std::string name = "Fetcher_numpy";
    this->set_name(name);

    if (K <= 0 || N <= 0) {
        std::stringstream message;
        message << "'K', and 'N' have to be greater than 0 ('K' = " << K
                << ", 'N' = " << N << ").";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }

    auto& p = this->create_task("append");
    auto p1s_U = this->template create_socket_in<B>(p, "U_K", this->K);

    this->create_codelet(p, [p1s_U](Module& m, Task& t, const size_t frame_id) -> int {
        auto& ftr = static_cast<Fetcher_numpy<B>&>(m);

        ftr._append(static_cast<B*>(t[p1s_U].get_dataptr()));

        return status_t::SUCCESS;
    });
}

template <typename B>
Fetcher_numpy<B>* Fetcher_numpy<B>::clone() const
{
    auto m = new Fetcher_numpy(*this);
    m->deep_copy(*this);
    return m;
}

template <typename B>
int Fetcher_numpy<B>::get_K() const
{
    return this->K;
}

template <typename B>
int Fetcher_numpy<B>::get_N() const
{
    return this->N;
}

template <typename B>
void Fetcher_numpy<B>::append(std::vector<B>& U_K)
{
    this->data.push_back(U_K);
}

template <typename B>
std::vector<std::vector<B>> Fetcher_numpy<B>::get_data() const
{
    return this->data;
}

template <typename B>
void Fetcher_numpy<B>::_append(B* U_K)
{
    std::vector<B> U_K_vec(U_K, U_K + this->K);
    this->append(U_K_vec);
}

template <typename B>
void Fetcher_numpy<B>::_get_data(B* data) const
{
    std::vector<B> data_vec;
    for (auto& U_K : this->data) {
        std::copy(U_K.begin(), U_K.end(), data_vec.end());
    }
    std::copy(data_vec.begin(), data_vec.end(), data);
}

template <typename B>
bool Fetcher_numpy<B>::is_done() const
{
    return this->data.size() == this->N;
}

template <typename B>
void Fetcher_numpy<B>::reset()
{
    this->data.clear();
}

#include "Tools/types.h"
template class aff3ct::module::Fetcher_numpy<int8_t>;
template class aff3ct::module::Fetcher_numpy<int16_t>;
template class aff3ct::module::Fetcher_numpy<int32_t>;
template class aff3ct::module::Fetcher_numpy<int64_t>;
template class aff3ct::module::Fetcher_numpy<float>;
template class aff3ct::module::Fetcher_numpy<double>;
