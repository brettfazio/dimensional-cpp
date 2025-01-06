#include "../helper.hh"

[[clang::annotate("unit:miles/hour")]] int velocity = miles50 / hours2;
[[clang::annotate("unit:miles/hour/hour")]] int acceleration = velocity / hours2;
[[clang::annotate("unit:miles")]] int distance = velocity * hours2;