#ifndef kfclient_rules_hpp
#define kfclient_rules_hpp

#include "kfbuffer.hpp"

#include <cstdint>
#include <cstdlib>

#include <string>
#include <vector>
#include <variant>

namespace kfc {
    struct kfrule {
        using variant_t = std::variant<std::string, bool, double>;

        kfrule(const kfbuffer& buff);

        std::string name;
        variant_t value;
    };

    struct kfrules {
        kfrules(const kfbuffer& buff);

        std::uint16_t count = 0;
        std::vector<kfrule> rules;
    };
}

#endif 