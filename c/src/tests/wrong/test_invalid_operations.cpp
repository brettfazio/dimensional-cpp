#include "../helper.hh"

[[clang::annotate("unit:miles")]] int dist = as_miles(100);
[[clang::annotate("unit:hour")]] int time = as_hour(2);
[[clang::annotate("unit:miles")]] int invalid_add = dist + time;  // ERROR: Cannot add different units
[[clang::annotate("unit:miles*hour")]] int invalid_product = dist + (dist * time);  // ERROR: Cannot add different unit types
