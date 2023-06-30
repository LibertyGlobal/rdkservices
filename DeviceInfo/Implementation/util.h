#pragma once

#include <core/Enumerate.h>

template<class enum_type>
enum_type str_to_enum(const std::string &str) {
    return WPEFramework::Core::EnumerateType<enum_type>(str.c_str()).Value();
}

template<class enum_type>
std::string enum_to_str(enum_type e) {
    return WPEFramework::Core::EnumerateType<enum_type>(e).Data();
}
