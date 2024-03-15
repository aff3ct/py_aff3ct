/**
 * @file Tunnel_numpy.cpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief
 * @date 2024-03-13 18:00:38
 * @modified: 2024-03-15 20:03:58
 */

#include "Module/Tunnel/Tunnel_numpy.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B>
Task& Tunnel_numpy<B>::operator[](const ftr::tsk t)
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
Socket& Tunnel_numpy<B>::operator[](const ftr::sck::append s)
{
    return Module::operator[]((size_t)ftr::tsk::append)[(size_t)s];
}

template <typename B>
Socket& Tunnel_numpy<B>::operator[](const ftr::sck::get s)
{
    return Module::operator[]((size_t)ftr::tsk::get)[(size_t)s];
}

template <typename B>
Tunnel_numpy<B>::Tunnel_numpy(const int K, const int N, const bool is_out)
    : Module()
    , K(K)
    , N(N)
    , is_out(is_out)
{
    const std::string name = "Tunnel_numpy";
    this->set_name(name);

    this->append_done = false;
    this->get_done = false;
    this->counter = 0;

    if (K <= 0 || N <= 0) {
        std::stringstream message;
        message << "'K', and 'N' have to be greater than 0 ('K' = " << K
                << ", 'N' = " << N << ").";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }

    auto& p1 = this->create_task("append");
    auto p1s_U = this->template create_socket_in<B>(p1, "U_K", this->K);
    // auto p1s_data = this->template create_socket_out<B>(p1, "data", this->K * this->N);
    this->create_codelet(p1, [p1s_U](Module& m, Task& t, const size_t frame_id) -> int {
        auto& ftr = static_cast<Tunnel_numpy<B>&>(m);

        ftr._append(static_cast<B*>(t[p1s_U].get_dataptr()));

        return status_t::SUCCESS;
    });

    auto& p2 = this->create_task("get");
    auto p2s_U = this->template create_socket_out<B>(p2, "U_K", this->K);
    this->create_codelet(p2, [p2s_U](Module& m, Task& t, const size_t frame_id) -> int {
        auto& ftr = static_cast<Tunnel_numpy<B>&>(m);

        ftr._get(static_cast<B*>(t[p2s_U].get_dataptr()));

        return status_t::SUCCESS;
    });
}

template <typename B>
Tunnel_numpy<B>* Tunnel_numpy<B>::clone() const
{
    auto m = new Tunnel_numpy(*this);
    m->deep_copy(*this);
    return m;
}

template <typename B>
int Tunnel_numpy<B>::get_K() const
{
    return this->K;
}

template <typename B>
int Tunnel_numpy<B>::get_N() const
{
    return this->N;
}

template <typename B>
void Tunnel_numpy<B>::append(std::vector<B>& U_K)
{
    if (this->is_out) {
        std::stringstream message;
        message << "The tunnel is an output tunnel, you can't append data to it.";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }
    (*this)[ftr::sck::append::U_K].bind(U_K.data());
    (*this)[ftr::tsk::append].exec();
}

template <typename B>
void Tunnel_numpy<B>::get(std::vector<B>& U_K)
{
    if (!this->is_out) {
        std::stringstream message;
        message << "The tunnel is an input tunnel, you can't get data from it.";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }
    (*this)[ftr::sck::get::U_K].bind(U_K.data());
    (*this)[ftr::tsk::get].exec();
}

template <typename B>
void Tunnel_numpy<B>::set_data(const std::vector<std::vector<B>>& data)
{
    this->data = data;
}

template <typename B>
std::vector<std::vector<B>> Tunnel_numpy<B>::get_data() const
{
    return this->data;
}

template <typename B>
void Tunnel_numpy<B>::_append(B* U_K)
{
    std::vector<B> U_K_vec(U_K, U_K + this->K);
    this->data.push_back(U_K_vec);
    if (this->data.size() == this->N) {
        this->append_done = true;
    }
}

template <typename B>
void Tunnel_numpy<B>::_get(B* U_K)
{
    std::copy(this->data[this->counter].begin(), this->data[this->counter].end(), U_K);
    this->counter++;

    if (this->counter == this->data.size()) {
        this->counter = 0;
        this->get_done = true;
    }
}

template <typename B>
bool Tunnel_numpy<B>::is_done() const
{
    if (this->is_out) {
        return this->get_done;
    } else {
        return this->append_done;
    }
}

template <typename B>
void Tunnel_numpy<B>::reset()
{
    this->data.clear();
    this->get_done = false;
    this->append_done = false;
    this->counter = 0;
}

#include "Tools/types.h"
template class aff3ct::module::Tunnel_numpy<int8_t>;
template class aff3ct::module::Tunnel_numpy<int16_t>;
template class aff3ct::module::Tunnel_numpy<int32_t>;
template class aff3ct::module::Tunnel_numpy<int64_t>;
template class aff3ct::module::Tunnel_numpy<float>;
template class aff3ct::module::Tunnel_numpy<double>;
