#include "../helper.hh"
#include <vector>

void foo() {
    M EXPLICIT constexpr int x = 5;
    Miles m = {x};

    KM int y = m.value;
}