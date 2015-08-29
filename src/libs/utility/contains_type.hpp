#pragma once

#include <type_traits>

namespace gorc {

    template <typename T, typename PackHeadT, typename ...PackT>
    class contains_type
        : public std::integral_constant<bool, std::is_same<T, PackHeadT>::value ||
                                              contains_type<T, PackT...>::value> { };

    template <typename T, typename PackHeadT>
    struct contains_type<T, PackHeadT>
        : public std::integral_constant<bool, std::is_same<T, PackHeadT>::value> { };

}
