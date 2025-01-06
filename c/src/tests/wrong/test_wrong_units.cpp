#include "../helper.hh"

[[clang::annotate("unit:miles")]] int X = miles50;
[[clang::annotate("unit:miles")]] int X_WRONG = X * X; // INVALID
[[clang::annotate("unit:km")]] int X_KM_WRONG = X;    // INVALID