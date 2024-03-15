/**
 * @file Tunnel_numpy.hpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief
 * @date 2024-03-13 18:00:30
 * @modified: 2024-03-15 15:55:06
 */

#ifndef TUNNEL_NUMPY_HPP_
#define TUNNEL_NUMPY_HPP_

#include "Module/Module.hpp"
#include "Module/Socket.hpp"
#include "Module/Task.hpp"
#include "Tools/Interface/Interface_is_done.hpp"
#include "Tools/Interface/Interface_reset.hpp"

namespace aff3ct {
namespace module {
    namespace ftr {
        enum class tsk : size_t {
            append,
            get,
            SIZE
        };
        namespace sck {
            enum class append : size_t {
                U_K,
                data,
                SIZE
            };
            enum class get : size_t {
                U_K,
                data,
                SIZE
            };
        }
    }

    template <typename B = int>
    class Tunnel_numpy : public Module,
                        //   public tools::Interface_is_done,
                          public tools::Interface_reset {
    public:
        inline Task& operator[](const ftr::tsk t);
        inline Socket& operator[](const ftr::sck::append s);
        inline Socket& operator[](const ftr::sck::get s);

    protected:
        std::vector<std::vector<B>> data;
        const size_t K; /*!< Number of information bits in one frame */
        const size_t N; /*!< Number of frames */
        size_t counter;
        bool append_done;
        bool get_done;

    public:
        Tunnel_numpy(const int K, const int N);

        ~Tunnel_numpy() = default;

        Tunnel_numpy<B>* clone() const;

        int get_K() const;

        int get_N() const;

        void append(std::vector<B>& U_K);

        void get(std::vector<B>& U_K);

        void set_data(const std::vector<std::vector<B>>& data);

        std::vector<std::vector<B>> get_data() const;

        bool is_append_done() const;

        bool is_get_done() const;

        void reset();

    private:
        void _append(B* U_K);

        void _get(B* U_K);

        // void _get_data(B* data) const;
    };
}
}

#endif /* TUNNEL_NUMPY_HPP_ */
