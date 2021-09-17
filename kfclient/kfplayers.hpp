#ifndef kfclient_players_hpp
#define kfclient_players_hpp

#include "kfbuffer.hpp"

#include <cstdint>
#include <cstdlib>

#include <string>
#include <vector>

namespace kfc {
    struct kfplayer {
        explicit kfplayer(const kfbuffer& buff);

        std::uint8_t id = 0;
        std::string name;
        std::uint32_t score = 0;
        std::uint32_t time = 0;
    };

    struct kfplayers {
        explicit kfplayers(const kfbuffer& buff);

        std::uint8_t count = 0;
        std::vector<kfplayer> players;
    };
}

#endif 