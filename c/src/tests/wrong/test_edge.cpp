#include "../helper.hh"

[[clang::annotate("unit:miles")]] int zero_dist = 0 * as_miles(100);  // Should this maintain units?
[[clang::annotate("unit:miles")]] int uninitialized_dist;  // ERROR: Uninitialized with units
[[clang::annotate("unit:miles")]] const int const_dist = as_km(100);  // ERROR: Wrong unit in const initialization
