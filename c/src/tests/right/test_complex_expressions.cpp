#include "../helper.hh"

// Testing complex mathematical expressions with units
M EXPLICIT int miles100 = 100;
[[clang::annotate("unit:miles*miles/hour")]] int complex_rate = (miles100 * miles50) / hours2;
[[clang::annotate("unit:(miles*miles)")]] int complex_accel = complex_rate * hours2;
// Example of parentheses in unit annotations working properly
// with order of operations.
[[clang::annotate("unit:(miles*miles)/hour*hour")]] int complex_accel_paren = complex_rate * hours2;
