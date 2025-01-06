#include "../helper.hh"

[[clang::annotate("unit:miles")]] int base_dist = miles50;
[[clang::annotate("unit:hour")]] int base_time = hours2;
[[clang::annotate("unit:miles/hour")]] int speed = base_dist / base_time;

// Complex expression with mixed unit errors
[[clang::annotate("unit:miles*miles/hour")]] int invalid_complex = 
    (base_dist * speed) + (base_dist / base_time * km25);  // ERROR: Mixed units in addition

// Incorrect unit inference in conditional
[[clang::annotate("unit:miles")]] int conditional_error = 
    (speed > 50) ? base_dist : base_time;