#include "../helper.hh"

// Testing complex mathematical expressions with units
M EXPLICIT int miles100 = 100;
[[clang::annotate("unit:miles*miles/hour")]] int complex_rate = (miles100 * miles50) / hours2;

[[clang::annotate("unit:(miles*miles)/(hour*hour)")]] int complex_accel_paren = complex_rate / hours2;
[[clang::annotate("unit:(miles*miles)/(hour*hour*hour)")]] int complex_accel_paren2 = complex_accel_paren / hours2;
