#include "../helper.hh"

[[clang::annotate("unit:miles")]] int X = miles50;
int X_DROP_UNITS_WRONG = X;