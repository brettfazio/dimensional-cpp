#define M [[clang::annotate("unit:miles")]]

template<typename T>
M T as_miles(T value) { return value; }

#define KM [[clang::annotate("unit:km")]]

template<typename T>
KM T as_km(T value) { return value; }

template<typename T>
[[clang::annotate("unit:hour")]] 
T as_hour(T value) { return value; }

struct Miles {
    M int value;
};
