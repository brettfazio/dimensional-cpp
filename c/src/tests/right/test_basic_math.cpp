#include "../helper.hh"

[[clang::annotate("unit:miles")]] int X = as_miles(32);
[[clang::annotate("unit:miles*miles")]] int X2 = X * X;
[[clang::annotate("unit:miles")]] int X1 = X2 / X;
int X0 = X / X;