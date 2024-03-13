/**
 * @file Source_user_numpy.hxx
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief 
 * @date 2024-03-13 21:27:20
 * @modified: 2024-03-13 21:30:08
 */

#include "Module/Module.hpp"
#include "Module/Source/User/Source_user_numpy.hpp"

namespace aff3ct
{
namespace module
{
    template <typename B>
    Task& Source_user_numpy<B>::operator[](const src::tsk t)
    {
        switch (t) {
        case src::tsk::generate:
            return Module::operator[](static_cast<size_t>(src::tsk::generate));
        default:
            break;
        }

        std::stringstream message;
        message << "Unknown task";
        throw tools::runtime_error(__FILE__, __LINE__, __func__, message.str());
    }
}
}