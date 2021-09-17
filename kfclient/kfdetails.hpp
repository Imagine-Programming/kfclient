#ifndef kfclient_details_hpp
#define kfclient_details_hpp

#include "libdef.hpp"
#include "kfbuffer.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>
#include <unordered_map>

namespace kfc {
    struct kfdetails {
    public:
        explicit kfdetails(const kfbuffer& buff);

        std::uint8_t protocol = 0;
        std::string hostname;
        std::string map;
        std::string game_dir;
        std::string game_description;
        std::uint16_t steam_app_id = 0;
        std::uint8_t player_count = 0;
        std::uint8_t player_cap = 0;
        std::uint8_t unknown1 = 0;
        std::uint8_t unknown2 = 0;
        std::uint8_t operating_system = 0;
        bool password_set = false; // std::uint8_t != 0;
        std::uint8_t unknown3 = 0;
        std::string version; // 4
        std::uint32_t unknown4 = 0;
        std::uint32_t unknown5 = 0;
        std::uint32_t unknown6 = 0;
        std::string additional_string;
        
        std::unordered_map<std::string, std::string> additional;

        std::int32_t waves_total = 0;
        std::int32_t waves_current = 0;
    };
}

#endif 