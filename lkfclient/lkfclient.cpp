#include <lua.hpp>
#include <kfclient.hpp>

#include <memory>
#include <type_traits>

template <typename T>
struct lkfclient_is_integer 
    : std::conjunction<
        std::is_integral<T>,
        std::negation<std::is_same<T, bool>>
    > {};

template <typename T, std::enable_if_t<lkfclient_is_integer<T>::value, int> = 0>
void push(lua_State* L, T v)                  { lua_pushinteger(L, static_cast<lua_Integer>(v)); }
void push(lua_State* L, bool v)               { lua_pushboolean(L, static_cast<int>(v)); }
void push(lua_State* L, const std::string& v) { lua_pushstring(L, v.c_str()); }

#define push_field(L, S, F)\
    lua_pushstring(L, #F);\
    push(L, S.F);\
    lua_rawset(L, -3);

#define push_function(L, N, F)\
    lua_pushstring(L, N);\
    lua_pushcfunction(L, F);\
    lua_rawset(L, -3);

static constexpr const char meta_name[] = "kfclient";

struct lkfclient_instance {
    boost::asio::io_context context;
    boost::asio::ip::udp::resolver resolver;
    boost::asio::ip::basic_resolver_results<boost::asio::ip::udp> endpoints;

    std::unique_ptr<kfc::kfclient> client;

    lkfclient_instance()
        : context(), resolver(context) {}
};

static int lkfclient_open(lua_State* L) {
    const auto host = luaL_checkstring(L, 1);
    const auto port = luaL_checkinteger(L, 2);
    const auto sprt = lua_tostring(L, 2);

    auto memory = lua_newuserdata(L, sizeof(lkfclient_instance));
    auto instance = new (memory) lkfclient_instance();
    luaL_setmetatable(L, meta_name);

    try {
        instance->endpoints = instance->resolver.resolve(boost::asio::ip::udp::v4(), host, sprt);
        instance->client = std::make_unique<kfc::kfclient>(instance->context, instance->endpoints);
    } catch (const std::exception& ex) {
        luaL_error(L, "kfclient cannot be established: %s", ex.what());
    }

    return 1;
}

static int lkfclient__gc(lua_State* L) {
    auto instance = static_cast<lkfclient_instance*>(luaL_checkudata(L, 1, meta_name));
    instance->~lkfclient_instance();
    return 0;
}

static int lkfclient_instance_close(lua_State* L) {
    auto instance = static_cast<lkfclient_instance*>(luaL_checkudata(L, 1, meta_name));
    instance->client = nullptr;
    return 0;
}

static int lkfclient_instance_details(lua_State* L) {
    auto instance = static_cast<lkfclient_instance*>(luaL_checkudata(L, 1, meta_name));

    try {
        const auto& details = instance->client->request_details();

        lua_newtable(L);
        push_field(L, details, protocol);
        push_field(L, details, hostname);
        push_field(L, details, map);
        push_field(L, details, game_dir);
        push_field(L, details, game_description);
        push_field(L, details, steam_app_id);
        push_field(L, details, player_count);
        push_field(L, details, player_cap);
        push_field(L, details, unknown1);
        push_field(L, details, unknown2);
        push_field(L, details, operating_system);
        push_field(L, details, password_set);
        push_field(L, details, unknown3);
        push_field(L, details, version);
        push_field(L, details, unknown4);
        push_field(L, details, unknown5);
        push_field(L, details, unknown6);
        push_field(L, details, additional_string);
        push_field(L, details, waves_total);
        push_field(L, details, waves_current);
        
        lua_pushstring(L, "additional");
        
        { lua_newtable(L);
            for (const auto& pair : details.additional) {
                lua_pushstring(L, pair.first.c_str());
                lua_pushstring(L, pair.second.c_str());
                lua_rawset(L, -3);
            }
        lua_rawset(L, -3); } 

        return 1;
    } catch (const std::exception& ex) {
        luaL_error(L, "%s", ex.what());
        return 0;
    }
}

static int lkfclient_instance_rules(lua_State* L) {
    auto instance = static_cast<lkfclient_instance*>(luaL_checkudata(L, 1, meta_name));

    try {
        const auto& rules = instance->client->request_rules();

        lua_newtable(L);
        for (const auto& rule : rules.rules) {
            lua_pushstring(L, rule.name.c_str());
            if (std::holds_alternative<double>(rule.value))
                lua_pushnumber(L, static_cast<lua_Number>(std::get<double>(rule.value)));
            else if (std::holds_alternative<bool>(rule.value))
                lua_pushboolean(L, static_cast<int>(std::get<bool>(rule.value)));
            else if (std::holds_alternative<std::string>(rule.value))
                lua_pushstring(L, std::get<std::string>(rule.value).c_str());
            lua_rawset(L, -3);
        }

        return 1;
    } catch (const std::exception& ex) {
        luaL_error(L, "%s", ex.what());
        return 0;
    }
}

static int lkfclient_instance_players(lua_State* L) {
    auto instance = static_cast<lkfclient_instance*>(luaL_checkudata(L, 1, meta_name));

    try {
        const auto& players = instance->client->request_players();

        lua_Integer index = 0;
        lua_newtable(L);
        for (const auto& player : players.players) {
            lua_newtable(L);
            push_field(L, player, id);
            push_field(L, player, name);
            push_field(L, player, score);
            push_field(L, player, time);
            lua_rawseti(L, -2, ++index);
        }

        return 1;
    } catch (const std::exception& ex) {
        luaL_error(L, "%s", ex.what());
        return 0;
    }
}

static const luaL_Reg lkfclient_api[] = {
    { "open", lkfclient_open }, 
    { nullptr, nullptr }
};

extern "C" int luaopen_kfclient(lua_State* L) {
    luaL_newmetatable(L, meta_name);
    {
        lua_pushstring(L, "__name");
        lua_pushstring(L, meta_name);
        lua_rawset(L, -3);

        push_function(L, "__gc", lkfclient__gc);

        lua_pushstring(L, "__index");
        lua_newtable(L);
        {
            push_function(L, "close", lkfclient_instance_close);
            push_function(L, "details", lkfclient_instance_details);
            push_function(L, "rules", lkfclient_instance_rules);
            push_function(L, "players", lkfclient_instance_players);
        }
        lua_rawset(L, -3);
    }

    lua_pop(L, 1);

    luaL_newlib(L, lkfclient_api);

    return 1;
}