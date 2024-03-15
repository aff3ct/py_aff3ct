/**
 * @file Fetcher_numpy.hpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief
 * @date 2024-03-13 18:00:30
 * @modified: 2024-03-15 10:19:29
 */

#ifndef FETCHER_NUMPY_HPP_
#define FETCHER_NUMPY_HPP_

#include "Module/Module.hpp"
#include "Module/Socket.hpp"
#include "Module/Task.hpp"
#include "Tools/Interface/Interface_is_done.hpp"
#include "Tools/Interface/Interface_reset.hpp"

namespace aff3ct {
namespace module {
    namespace ftr {
        enum class tsk : size_t { append,
            SIZE };
        namespace sck {
            enum class append : size_t { data,
                SIZE };
        }
    }

    template <typename B = int>
    class Fetcher_numpy : public Module,
                          public tools::Interface_is_done,
                          public tools::Interface_reset {
    public:
        inline Task& operator[](const ftr::tsk t);
        inline Socket& operator[](const ftr::sck::append s);

    protected:
        std::vector<std::vector<B>> data;
        const int K; /*!< Number of information bits in one frame */
        const int N; /*!< Number of frames */

    public:
        Fetcher_numpy(const int K, const int N);

        ~Fetcher_numpy() = default;

        Fetcher_numpy<B>* clone() const;

        int get_K() const;

        int get_N() const;

        void append(std::vector<B>& U_K);

        std::vector<std::vector<B>> get_data() const;

        bool is_done() const;

        void reset();

    private:
        void _append(B* U_K);

        void _get_data(B* data) const;
    };
}
}

#endif /* FETCHER_NUMPY_HPP_ */
