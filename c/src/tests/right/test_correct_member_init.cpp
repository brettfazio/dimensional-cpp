#include "../helper.hh"

struct Data {
    M int x;
};

M EXPLICIT constexpr int X = 32;

Data d = (Data){.x = X};