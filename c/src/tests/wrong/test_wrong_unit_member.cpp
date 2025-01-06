#include "../helper.hh"

struct Data {
    H int x;
};

M EXPLICIT constexpr int X = 32;

Data d = (Data){.x = X};