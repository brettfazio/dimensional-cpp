#include "../helper.hh"

M int get_miles(M int miles) {
    return miles;
}

[[clang::annotate("unit:km")]] int X_KM_VIA_FUNC_WRONG = get_miles(miles50);    // INVALID