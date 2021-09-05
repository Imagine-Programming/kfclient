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
        kfdetails(const kfbuffer& buff);

        std::uint8_t protocol;
        std::string hostname;
        std::string map;
        std::string game_dir;
        std::string game_description;
        std::uint16_t steam_app_id;
        std::uint8_t player_count;
        std::uint8_t player_cap;
        std::uint8_t unknown1;
        std::uint8_t unknown2;
        std::uint8_t operating_system;
        bool password_set; // std::uint8_t != 0;
        std::uint8_t unknown3;
        std::string version; // 4
        std::uint32_t unknown4;
        std::uint32_t unknown5;
        std::uint32_t unknown6;
        std::string additional_string;
        
        std::unordered_map<std::string, std::string> additional;

        std::uint32_t waves_total;
        std::uint32_t waves_current;
    };
}

#endif 