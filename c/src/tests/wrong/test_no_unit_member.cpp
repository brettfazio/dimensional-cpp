#include "../helper.hh"

struct Data {
    int x;
};

M EXPLICIT constexpr int X = 32;

Data d = {.x = X};