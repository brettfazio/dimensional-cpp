#include "../helper.hh"
#include <array>

#define M [[clang::annotate("unit:miles")]]

static std::array<Miles, 32> values;

M int val = values[0].value;
