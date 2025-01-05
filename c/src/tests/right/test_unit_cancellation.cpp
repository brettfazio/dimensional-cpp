#include "../helper.hh"

[[clang::annotate("unit:miles")]] int dist1 = as_miles(100);
[[clang::annotate("unit:miles")]] int dist2 = as_miles(200);
int ratio = dist1 / dist2;  // Units cancel out
[[clang::annotate("unit:miles")]] int dist3 = ratio * dist2;  // Valid multiplication with unitless value
