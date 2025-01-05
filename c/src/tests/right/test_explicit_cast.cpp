#include "../helper.hh"

[[clang::annotate("unit:km")]] int Y = as_km(5);
[[clang::annotate("unit:miles")]] int X_MILES_FROM_KM = as_miles(Y);