#include "kfrules.hpp"

#include <algorithm>
#include <cmath>

kfc::kfrule::kfrule(const kfbuffer& buff) {
    std::string value_temp;
    buff.consume(name, value_temp);

    // is it a boolean?
    if (value_temp == "True" || value_temp == "False") {
        value = value_temp == "True";
        return;
    }
    
    // or perhaps a numeric value?
    char* end = nullptr;
    double val = strtod(value_temp.c_str(), &end);
    if (end != value_temp.c_str() && *end == '\0' && val != HUGE_VAL) { 
        value = val;
        return;
    }

    // nope, just a string
    value = value_temp;
}

kfc::kfrules::kfrules(const kfbuffer& buff) {
    buff.consume(count);

    for (std::uint16_t i = 0; i < count; ++i) 
        rules.emplace_back(buff);
}