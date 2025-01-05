template<typename T>
[[clang::annotate("unit:miles")]] 
T as_miles(T value) { return value; }

template<typename T>
[[clang::annotate("unit:km")]] 
T as_km(T value) { return value; }

template<typename T>
[[clang::annotate("unit:hour")]] 
T as_hour(T value) { return value; }