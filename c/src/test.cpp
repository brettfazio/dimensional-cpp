#include <cstdio>

template<typename T>
[[clang::annotate("unit:miles")]] 
T as_miles(T value) { return value; }

template<typename T>
[[clang::annotate("unit:km")]] 
T as_km(T value) { return value; }

template<typename T>
[[clang::annotate("unit:hour")]] 
T as_hour(T value) { return value; }

[[clang::annotate("unit:miles")]] int X = as_miles(32);
[[clang::annotate("unit:miles*miles")]] int X2 = X * X;
[[clang::annotate("unit:miles")]] int X1 = X2 / X;
int X0 = X / X;

[[clang::annotate("unit:miles/hour")]] int MPH = as_miles(60) / as_hour(1);

[[clang::annotate("unit:miles")]] int X_WRONG = X * X; // INVALID
[[clang::annotate("unit:km")]] int X_KM_WRONG = X;    // INVALID

int X_DROP_UNITS_WRONG = X;

[[clang::annotate("unit:km")]] int X_KM_VIA_FUNC_WRONG = as_miles(1);    // INVALID

[[clang::annotate("unit:km")]] int Y = as_km(5);
// Possibly right? This is effectively an explicit cast.
[[clang::annotate("unit:miles")]] int X_MILES_FROM_KM = as_miles(Y);

void foo() {
    printf("bar");
}