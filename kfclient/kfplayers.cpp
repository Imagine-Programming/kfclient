#include "kfplayers.hpp"

kfc::kfplayer::kfplayer(const kfbuffer& buff) {
    buff.consume(id, name, score, time);
}

kfc::kfplayers::kfplayers(const kfbuffer& buff) {
    buff.consume(count);

    for (std::uint8_t i = 0; i < count; ++i) 
        players.emplace_back(buff);
} 