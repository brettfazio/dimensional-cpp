#include "../helper.hh"

[[clang::annotate("unit:miles/hour")]] int MPH = as_miles(60) / as_hour(1);