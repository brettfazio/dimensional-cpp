#include "../helper.hh"

[[clang::annotate("unit:miles")]] int X = as_miles(32);
[[clang::annotate("unit:miles")]] int X_WRONG = X * X; // INVALID
[[clang::annotate("unit:km")]] int X_KM_WRONG = X;    // INVALID