#include "../helper.hh"

template<typename T>
[[clang::annotate("unit:km")]]
T wrong_conversion([[clang::annotate("unit:miles")]] T miles) {
    return miles;  // ERROR: Return type units don't match
}

template<typename T>
[[clang::annotate("unit:miles/hour")]]
T invalid_speed([[clang::annotate("unit:km")]] T distance,  // ERROR: Parameter units don't match return type
                [[clang::annotate("unit:hour")]] T time) {
    return distance / time;
}