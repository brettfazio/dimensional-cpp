#include "../helper.hh"

[[clang::annotate("unit:miles/hour")]] int speed = miles50 / hours2;
[[clang::annotate("unit:miles/hour")]] int wrong_speed = speed + speed * hours2;  // ERROR: Adding incompatible units
[[clang::annotate("unit:miles*hour")]] int wrong_unit = miles50 * hours2 * hours2;  // ERROR: Extra multiplication changes unit
