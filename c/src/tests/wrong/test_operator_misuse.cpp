#include "../helper.hh"

[[clang::annotate("unit:miles")]] int dist = as_miles(100);
[[clang::annotate("unit:hour")]] int time = as_hour(2);
[[clang::annotate("unit:miles/hour")]] int speed = dist / time;
[[clang::annotate("unit:miles/hour")]] int wrong_speed = speed + speed * time;  // ERROR: Adding incompatible units
[[clang::annotate("unit:miles*hour")]] int wrong_unit = dist * time * time;  // ERROR: Extra multiplication changes unit
