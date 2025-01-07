#include "../helper.hh"

struct Data {
    M int x;
};

constexpr int X = 32;

// Right now the plugin does not support c-style casts
// so this will be considered a correct initialization.
Data d = (Data){.x = X};