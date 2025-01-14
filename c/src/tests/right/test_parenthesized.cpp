#include "../helper.hh"

// Testing complex mathematical expressions with units
M EXPLICIT int miles100 = 100;

// Testing parenthesized expressions
[[clang::annotate("unit:miles/hour")]] int speed1 = miles100 / hours2;
[[clang::annotate("unit:miles/hour")]] int avg_speed = (speed1 + speed1) / 2;
