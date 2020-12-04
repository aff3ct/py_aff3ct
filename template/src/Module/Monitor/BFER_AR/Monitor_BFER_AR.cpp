#include <string>
#include <vector>
#include <sstream>

 #include "Tools/Perf/distance/hamming_distance.h"
#include "Module/Monitor/BFER_AR/Monitor_BFER_AR.hpp"

using namespace aff3ct;
using namespace aff3ct::module;

template <typename B>
Monitor_BFER_AR<B>
::Monitor_BFER_AR(const int K, const unsigned max_fe, const unsigned max_n_frames,
               const bool count_unknown_values)
: Monitor(), K(K), max_fe(max_fe), max_n_frames(max_n_frames),
  count_unknown_values(count_unknown_values), vals(new Attributes()), err_hist(0), err_hist_activated(false)
{
	const std::string name = "Monitor_BFER_AR";
	this->set_name(name);
	this->set_single_wave(true);

	if (K <= 0)
	{
		std::stringstream message;
		message << "'K' has to be greater than 0 ('K' = " << K << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	auto &p1 = this->create_task("check_errors", (int)mnt::tsk::check_errors);
	auto p1s_U = this->template create_socket_in<B>(p1, "U", this->get_K());
	auto p1s_V = this->template create_socket_in<B>(p1, "V", this->get_K());

	this->create_codelet(p1, [p1s_U, p1s_V](Module &m, Task &t, const size_t frame_id) -> int
	{
		auto &mnt = static_cast<Monitor_BFER_AR<B>&>(m);

		auto n_be = mnt._check_errors(static_cast<B*>(t[p1s_U].get_dataptr()),
		                              static_cast<B*>(t[p1s_V].get_dataptr()),
		                              frame_id);

		return n_be;
	});

	auto &p2 = this->create_task("check_errors2", (int)mnt::tsk::check_errors2);
	auto p2s_U   = this->template create_socket_in <B      >(p2,   "U", get_K());
	auto p2s_V   = this->template create_socket_in <B      >(p2,   "V", get_K());
	auto p2s_FRA = this->template create_socket_out<int64_t>(p2, "FRA", 1      );
	auto p2s_BE  = this->template create_socket_out<int32_t>(p2,  "BE", 1      );
	auto p2s_FE  = this->template create_socket_out<int32_t>(p2,  "FE", 1      );
	auto p2s_BER = this->template create_socket_out<float  >(p2, "BER", 1      );
	auto p2s_FER = this->template create_socket_out<float  >(p2, "FER", 1      );

	this->create_codelet(p2, [p2s_U, p2s_V, p2s_FRA, p2s_BE, p2s_FE, p2s_BER, p2s_FER](Module &m, Task &t,
	                                                                                   const size_t frame_id) -> int
	{
		auto &mnt = static_cast<Monitor_BFER_AR<B>&>(m);

		auto n_be = mnt._check_errors2(static_cast<B*      >(t[p2s_U  ].get_dataptr()),
		                               static_cast<B*      >(t[p2s_V  ].get_dataptr()),
		                               static_cast<int64_t*>(t[p2s_FRA].get_dataptr()),
		                               static_cast<int32_t*>(t[p2s_BE ].get_dataptr()),
		                               static_cast<int32_t*>(t[p2s_FE ].get_dataptr()),
		                               static_cast<float*  >(t[p2s_BER].get_dataptr()),
		                               static_cast<float*  >(t[p2s_FER].get_dataptr()),
		                               frame_id);

		return n_be;
	});

	reset();
}

template <typename B>
Monitor_BFER_AR<B>* Monitor_BFER_AR<B>
::clone() const
{
	auto m = new Monitor_BFER_AR(*this);
	m->deep_copy(*this);
	return m;
}


template <typename B>
bool Monitor_BFER_AR<B>
::equivalent(const Monitor_BFER_AR<B>& m, bool do_throw) const
{
	if (get_K() != m.get_K())
	{
		std::stringstream message;
		message << "'get_K()' is different than 'm.get_K()' ('get_K()' = " << get_K() << ", 'm.get_K()' = "
		        << m.get_K() <<").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (get_max_fe() != m.get_max_fe())
	{
		if (!do_throw)
			return false;

		std::stringstream message;
		message << "'get_max_fe()' is different than 'm.get_max_fe()' ('this->get_max_fe()' = " << get_max_fe()
		        << ", 'm.get_max_fe()' = " << m.get_max_fe() << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (get_max_n_frames() != m.get_max_n_frames())
	{
		if (!do_throw)
			return false;

		std::stringstream message;
		message << "'get_max_n_frames()' is different than 'm.get_max_n_frames()' ('this->get_max_n_frames()' = "
		        << get_max_n_frames() << ", 'm.get_max_n_frames()' = " << m.get_max_n_frames() << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	if (get_count_unknown_values() != m.get_count_unknown_values())
	{
		if (!do_throw)
			return false;

		std::stringstream message;
		message << "'get_count_unknown_values()' is different than 'm.get_count_unknown_values()' "
		        << "('get_count_unknown_values()' = " << get_count_unknown_values()
		        << ", 'm.get_count_unknown_values()' = " << m.get_count_unknown_values() << ").";
		throw tools::invalid_argument(__FILE__, __LINE__, __func__, message.str());
	}

	return true;
}

template <typename B>
int Monitor_BFER_AR<B>
::check_errors(const B *U, const B *V, const int frame_id, const bool managed_memory)
{
	(*this)[mnt::sck::check_errors::U].bind(U);
	(*this)[mnt::sck::check_errors::V].bind(V);
	const auto &status = (*this)[mnt::tsk::check_errors].exec(frame_id, managed_memory);

	return status[0];
}

template <typename B>
int Monitor_BFER_AR<B>
::_check_errors(const B *U,
                const B *V,
                const size_t frame_id)
{
	int n_be_total = 0;
	for (size_t f = 0; f < this->get_n_frames(); f++)
		n_be_total += this->__check_errors(U + f * get_K(),
		                                   V + f * get_K(),
		                                   f);

	return n_be_total;
}

template <typename B>
int Monitor_BFER_AR<B>
::check_errors2(const B *U,
                const B *V,
                int64_t *FRA,
                int32_t *BE,
                int32_t *FE,
                float   *BER,
                float   *FER,
                const int frame_id,
                const bool managed_memory)
{
	(*this)[mnt::sck::check_errors2::U  ].bind(U);
	(*this)[mnt::sck::check_errors2::V  ].bind(V);
	(*this)[mnt::sck::check_errors2::FRA].bind(FRA);
	(*this)[mnt::sck::check_errors2::BE ].bind(BE);
	(*this)[mnt::sck::check_errors2::FE ].bind(FE);
	(*this)[mnt::sck::check_errors2::BER].bind(BER);
	(*this)[mnt::sck::check_errors2::FER].bind(FER);
	const auto &status = (*this)[mnt::tsk::check_errors2].exec(frame_id, managed_memory);

	return status[0];
}

template <typename B>
int Monitor_BFER_AR<B>
::_check_errors2(const B *U,
                 const B *V,
                 int64_t *FRA,
                 int32_t *BE,
                 int32_t *FE,
                 float   *BER,
                 float   *FER,
                 const size_t frame_id)
{
	int n_be_total = 0;
	for (size_t f = 0; f < this->get_n_frames(); f++)
	{
		auto n_be = this->__check_errors(U + f * get_K(),
		                                 V + f * get_K(),
		                                 f);
		n_be_total += n_be;

		FRA[f] = (int64_t)this->get_n_analyzed_fra();
		BE [f] = n_be;
		FE [f] = n_be ? 1 : 0;
		BER[f] = this->get_ber();
		FER[f] = this->get_fer();
	}

	return n_be_total;
}

template <typename B>
int Monitor_BFER_AR<B>
::__check_errors(const B *U, const B *V, const size_t frame_id)
{
	int bit_errors_count;

	if (get_count_unknown_values())
		bit_errors_count = (int)tools::hamming_distance_unk(U, V, get_K());
	else
		bit_errors_count = (int)tools::hamming_distance(U, V, get_K());

	if (bit_errors_count)
	{
		vals->n_be += bit_errors_count;
		vals->n_fe ++;

		if (err_hist_activated)
			err_hist.add_value(bit_errors_count);
	}

	vals->n_fra++;

	return bit_errors_count;
}

template <typename B>
bool Monitor_BFER_AR<B>
::fe_limit_achieved() const
{
	return get_max_fe() != 0 && get_n_fe() >= get_max_fe();
}

template <typename B>
bool Monitor_BFER_AR<B>
::frame_limit_achieved() const
{
	return get_max_n_frames() != 0 && get_n_analyzed_fra() >= get_max_n_frames();
}

template <typename B>
bool Monitor_BFER_AR<B>
::is_done() const
{
	return fe_limit_achieved() || frame_limit_achieved();
}

template <typename B>
const typename Monitor_BFER_AR<B>::Attributes& Monitor_BFER_AR<B>
::get_attributes() const
{
	return *vals.get();
}

template <typename B>
int Monitor_BFER_AR<B>
::get_K() const
{
	return K;
}

template <typename B>
unsigned Monitor_BFER_AR<B>
::get_max_n_frames() const
{
	return max_n_frames;
}

template <typename B>
unsigned Monitor_BFER_AR<B>
::get_max_fe() const
{
	return max_fe;
}

template <typename B>
unsigned long long Monitor_BFER_AR<B>
::get_n_analyzed_fra() const
{
	return vals->n_fra.load();
}

template <typename B>
unsigned long long Monitor_BFER_AR<B>
::get_n_fe() const
{
	return vals->n_fe.load();
}

template <typename B>
unsigned long long Monitor_BFER_AR<B>
::get_n_be() const
{
	return vals->n_be.load();
}

template <typename B>
float Monitor_BFER_AR<B>
::get_fer() const
{
	auto t_fer = 0.f;
	if (this->get_n_fe() != 0)
		t_fer = (float)this->get_n_fe() / (float)this->get_n_analyzed_fra();
	else
		t_fer = (1.f) / ((float)this->get_n_analyzed_fra());

	return t_fer;
}

template <typename B>
float Monitor_BFER_AR<B>
::get_ber() const
{
	auto t_ber = 0.f;
	if (this->get_n_be() != 0)
		t_ber = (float)this->get_n_be() / (float)this->get_n_analyzed_fra() / (float)this->get_K();
	else
		t_ber = (1.f) / ((float)this->get_n_analyzed_fra()) / this->get_K();

	return t_ber;
}

template<typename B>
bool Monitor_BFER_AR<B>
::get_count_unknown_values() const
{
	return count_unknown_values;
}

template<typename B>
const tools::Histogram<int>& Monitor_BFER_AR<B>::get_err_hist() const
{
	return err_hist;
}

template<typename B>
void Monitor_BFER_AR<B>
::activate_err_histogram(bool val)
{
	err_hist_activated = val;
}

template <typename B>
void Monitor_BFER_AR<B>
::reset()
{
	Monitor::reset();
	vals->reset();

	this->err_hist.reset();
}

template <typename B>
void Monitor_BFER_AR<B>
::collect(const Monitor& m, bool fully)
{
	collect(static_cast<const Monitor_BFER_AR<B>&>(m), fully);
}

template <typename B>
void Monitor_BFER_AR<B>
::collect(const Monitor_BFER_AR<B>& m, bool fully)
{
	collect(m.get_attributes());
	if (fully)
		this->err_hist.add_values(m.err_hist);
}

template <typename B>
void Monitor_BFER_AR<B>
::collect(const Attributes& v)
{
	*vals.get() += v;
}

template <typename B>
Monitor_BFER_AR<B>& Monitor_BFER_AR<B>
::operator+=(const Monitor_BFER_AR<B>& m)
{
	collect(m, false);
	return *this;
}

template <typename B>
void Monitor_BFER_AR<B>
::copy(const Monitor& m, bool fully)
{
	copy(static_cast<const Monitor_BFER_AR<B>&>(m), fully);
}

template <typename B>
void Monitor_BFER_AR<B>
::copy(const Monitor_BFER_AR<B>& m, bool fully)
{
	copy(m.get_attributes());
	if (fully)
		this->err_hist = m.err_hist;
}

template <typename B>
void Monitor_BFER_AR<B>
::copy(const Attributes& v)
{
	vals->n_be = v.n_be.load();
	vals->n_fe = v.n_fe.load();
	vals->n_fra = v.n_fra.load();
}

template <typename B>
Monitor_BFER_AR<B>& Monitor_BFER_AR<B>
::operator=(const Monitor_BFER_AR<B>& m)
{
	copy(m, false);
	return *this;
}


template <typename B>
typename Monitor_BFER_AR<B>::Attributes& Monitor_BFER_AR<B>::Attributes
::operator+=(const Attributes& a)
{
	n_be  += a.n_be;
	n_fe  += a.n_fe;
	n_fra += a.n_fra;

	return *this;
}

template <typename B>
void Monitor_BFER_AR<B>::Attributes
::reset()
{
	n_be  = 0;
	n_fe  = 0;
	n_fra = 0;
}

template <typename B>
Monitor_BFER_AR<B>::Attributes
::Attributes()
{
	reset();
}

// ==================================================================================== explicit template instantiation
#include "Tools/types.h"
#ifdef AFF3CT_MULTI_PREC
template class aff3ct::module::Monitor_BFER_AR<B_8 >;
template class aff3ct::module::Monitor_BFER_AR<B_16>;
template class aff3ct::module::Monitor_BFER_AR<B_32>;
template class aff3ct::module::Monitor_BFER_AR<B_64>;
#else
template class aff3ct::module::Monitor_BFER_AR<B>;
#endif
// ==================================================================================== explicit template instantiation
