#include "../helper.hh"

// Testing complex mathematical expressions with units
M EXPLICIT int miles100 = 100;
[[clang::annotate("unit:miles*miles/hour")]] int complex_rate = (miles100 * miles50) / hours2;
[[clang::annotate("unit:miles*miles/hour*hour")]] int complex_accel = complex_rate * hours2;

// Testing parenthesized expressions
[[clang::annotate("unit:miles/hour")]] int speed1 = miles100 / hours2;
[[clang::annotate("unit:miles/hour")]] int avg_speed = (speed1 + speed1) / 2;
