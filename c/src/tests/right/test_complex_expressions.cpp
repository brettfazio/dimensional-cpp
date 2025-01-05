#include "../helper.hh"

// Testing complex mathematical expressions with units
[[clang::annotate("unit:miles*miles/hour")]] int complex_rate = (as_miles(100) * as_miles(50)) / as_hour(2);
[[clang::annotate("unit:miles*miles/hour*hour")]] int complex_accel = complex_rate * as_hour(3);

// Testing parenthesized expressions
[[clang::annotate("unit:miles/hour")]] int speed1 = as_miles(120) / as_hour(2);
[[clang::annotate("unit:miles/hour")]] int speed2 = as_miles(60) / as_hour(1);
[[clang::annotate("unit:miles/hour")]] int avg_speed = (speed1 + speed2) / 2;
