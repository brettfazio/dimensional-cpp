#include "../helper.hh"

template<typename T>
[[clang::annotate("unit:hour*hour")]]
T square_hours([[clang::annotate("unit:hour")]] T h) {
    return h * h;
}

[[clang::annotate("unit:hour*hour")]] int H_INPUT_WRONG = square_hours(km25);
