#include "../helper.hh"
#include <vector>

void foo() {
    std::vector<Miles> values{};
    values.push_back({5});
    // Drop unit
    int val = values[0].value;
}