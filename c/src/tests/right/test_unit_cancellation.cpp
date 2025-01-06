#include "../helper.hh"

[[clang::annotate("unit:miles")]] int dist1 = miles50;
[[clang::annotate("unit:miles")]] int dist2 = miles50;
int ratio = dist1 / dist2;  // Units cancel out
[[clang::annotate("unit:miles")]] int dist3 = ratio * dist2;  // Valid multiplication with unitless value
