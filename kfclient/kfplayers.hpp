#ifndef kfclient_players_hpp
#define kfclient_players_hpp

#include "kfbuffer.hpp"

#include <cstdint>
#include <cstdlib>

#include <string>
#include <vector>

namespace kfc {
    struct kfplayer {
        kfplayer(const kfbuffer& buff);

        std::uint8_t id;
        std::string name;
        std::uint32_t score;
        std::uint32_t time;
    };

    struct kfplayers {
        kfplayers(const kfbuffer& buff);

        std::uint8_t count;
        std::vector<kfplayer> players;
    };
}

#endif 