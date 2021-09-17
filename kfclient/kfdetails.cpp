#include "kfdetails.hpp"

kfc::kfdetails::kfdetails(const kfbuffer& buff) { 
    buff.consume(protocol);
    buff.consume(hostname, map, game_dir, game_description);
    buff.consume(steam_app_id, player_count, player_cap, unknown1, unknown2, operating_system);
    buff.consume_cast<std::uint8_t>(password_set);
    buff.consume(unknown3);
    buff.consume_string(version, 4);
    buff.consume(unknown4, unknown5, unknown6);
    buff.consume(additional_string);

    std::size_t i = 0;
    std::string key;
    std::string value;
    bool processing_key = true;

    while (additional_string[i] != '\0') {
        if (additional_string[i] == ',') {
            additional[key] = value;
            key = value = "";
            processing_key = true;
            i++;
        } else if (additional_string[i] == ':') {
            processing_key = false;
            i++;
        } else {
            if (processing_key) { 
                key += additional_string[i++];
            } else {
                value += additional_string[i++];
            }
        }
    }

    if (additional.count("d") != 0) 
        waves_total = std::stol(additional.at("d"));
    if (additional.count("e") != 0)
        waves_current = std::stol(additional.at("e"));
}