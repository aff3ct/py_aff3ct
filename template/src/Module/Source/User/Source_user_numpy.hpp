/**
 * @file Source_user_numpy.hpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief reference: lib/aff3ct/include/Module/Source/User/Source_user_binary.hpp
 * @date 2024-03-13 15:12:47
 * @modified: 2024-03-14 00:35:32
 */
#ifndef SOURCE_USER_NUMPY_HPP_
#define SOURCE_USER_NUMPY_HPP_

#include <vector>

#include "Module/Source/Source.hpp"

namespace aff3ct {
namespace module {
    template <typename B = int>
    class Source_user_numpy : public Source<B> {
    private:
        std::vector<std::vector<B>> source;
        size_t src_counter;
        bool done;

    public:
        Source_user_numpy(const int K, const std::vector<std::vector<B>>& input);
        virtual ~Source_user_numpy() = default;
        bool is_done() const;

    protected:
        void _generate(B* U_K, const size_t frame_id);
    };

}
}

#endif /* SOURCE_USER_NUMPY_HPP_ */
