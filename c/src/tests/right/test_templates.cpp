#include "../helper.hh"

template<typename T>
[[clang::annotate("unit:miles/hour")]]
T calculate_speed([[clang::annotate("unit:miles")]] T distance, 
                 [[clang::annotate("unit:hour")]] T time) {
    return distance / time;
}

template<typename T>
[[clang::annotate("unit:km*km")]]
T squared_distance([[clang::annotate("unit:km")]] T dist) {
    return dist * dist;
}

[[clang::annotate("unit:miles/hour")]] int valid_speed = calculate_speed(as_miles(240), as_hour(4));
[[clang::annotate("unit:km*km")]] int valid_area = squared_distance(as_km(10));