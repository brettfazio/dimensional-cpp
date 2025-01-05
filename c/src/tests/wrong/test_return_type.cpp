#include "../helper.hh"

[[clang::annotate("unit:km")]] int X_KM_VIA_FUNC_WRONG = as_miles(1);    // INVALID