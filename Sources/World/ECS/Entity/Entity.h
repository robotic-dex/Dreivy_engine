#pragma once
#include <cstdint>

using Entity = uint32_t;
struct Name {
    std::string value;
};
static constexpr Entity InvalidEntity = 0;