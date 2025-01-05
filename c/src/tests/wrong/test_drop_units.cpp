#include "../helper.hh"

[[clang::annotate("unit:miles")]] int X = as_miles(32);
int X_DROP_UNITS_WRONG = X;