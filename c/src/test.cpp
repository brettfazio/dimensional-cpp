[[clang::annotate("unit:miles")]] int X = 32;
[[clang::annotate("unit:miles*miles")]] int X2 = X * X;

[[clang::annotate("unit:miles")]] int X_WRONG = X * X; // INVALID
[[clang::annotate("unit:km")]] int X_KM_WRONG = X;    // INVALID
