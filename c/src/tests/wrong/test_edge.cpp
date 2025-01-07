#include "../helper.hh"

[[clang::annotate("unit:miles")]] int zero_dist = 0 * miles50;  // Should this maintain units?
[[clang::annotate("unit:miles")]] const int const_dist = km25;  // ERROR: Wrong unit in const initialization
