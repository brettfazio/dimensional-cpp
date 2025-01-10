#include "../helper.hh"

M EXPLICIT int miles100 = 100;

#define M2H [[clang::annotate("unit:miles*miles/hour")]]

M2H int complex_rate = (miles100 * miles50) / hours2;

#define WRONG [[clang::annotate("minutes")]]

WRONG int drop_unit = static_cast<short>(complex_rate);