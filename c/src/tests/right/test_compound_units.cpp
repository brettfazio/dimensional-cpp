#include "../helper.hh"

[[clang::annotate("unit:miles/hour")]] int velocity = as_miles(60) / as_hour(1);
[[clang::annotate("unit:miles/hour/hour")]] int acceleration = velocity / as_hour(2);
[[clang::annotate("unit:miles")]] int distance = velocity * as_hour(3);