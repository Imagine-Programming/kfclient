#include <boost/asio.hpp>
#include <kfclient.hpp>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <fort.hpp>

#include "definition.hpp"

#include <memory>

using udp = boost::asio::ip::udp;

#if defined(WIN32) || defined(WIN64)
		static inline const ft_border_style* TABLE_BORDER_STYLE = FT_BASIC_STYLE;
#else 
		static inline const ft_border_style* TABLE_BORDER_STYLE = FT_SOLID_ROUND_STYLE;
#endif 

static inline const ft_border_style* NO_BORDER_STYLE = FT_EMPTY_STYLE;

template <typename T>
void set_border_style(T& table, const commandline::kfclient_cli& cli) {
    table.set_border_style(cli.isset(commandline::descriptors::NAME_CLEAN) ? NO_BORDER_STYLE : TABLE_BORDER_STYLE);
}

static const std::size_t DEFAULT_TIMEOUT = 10;
static const std::size_t DEFAULT_PORT = 27015;

static inline const std::vector<std::string> DETAIL_HEADERS = { "field", "value" };
static inline const std::vector<std::string> RULE_HEADERS = { "rule", "value" };
static inline const std::vector<std::string> PLAYER_HEADERS = { "id", "name", "score", "time" };
static inline const std::vector<std::string> FILTER_PRECEDENCE = { "details", "rules", "players" };

struct client_instance {
    boost::asio::io_context io_context;
    udp::resolver resolver;
    boost::asio::ip::basic_resolver_results<udp> endpoints;
    std::unique_ptr<kfc::kfclient> client;

    client_instance(const std::string& host, const std::string& protocol)
        : io_context(), resolver(io_context) {
            endpoints = resolver.resolve(udp::v4(), host, protocol);
            client = std::make_unique<kfc::kfclient>(io_context, endpoints);
        }
};

using report_function = int(*)(client_instance& instance, const commandline::kfclient_cli& cli);
int report_details(client_instance& instance, const commandline::kfclient_cli& cli);
int report_rules(client_instance& instance, const commandline::kfclient_cli& cli);
int report_players(client_instance& instance, const commandline::kfclient_cli& cli);

static inline const std::unordered_map<std::string, report_function> reporters = {
    { "details", report_details }, { "d", report_details },
    { "rules", report_rules }, { "r", report_rules },
    { "players", report_players }, { "p", report_players }
};

std::unique_ptr<commandline::kfclient_cli> create_cli() {
    using namespace commandline;

    auto cli = std::make_unique<kfclient_cli>(descriptors::PROGRAM_DESC, descriptors::PROGRAM_NAME);

    cli->command().set_help_all_flag("-H,--help-all", "show all help.");

    cli->add_flag(descriptors::DESC_VERBOSE);
    cli->add_flag(descriptors::DESC_CLEAN);
    cli->add_flag(descriptors::DESC_PLAYER_COUNT);
    cli->add_option<std::vector<std::string>>(descriptors::DESC_REPORT)->required(false)->check(CLI::IsMember({ "details", "rules", "players", "d", "r", "p" }));
    cli->add_option<std::size_t>(descriptors::DESC_TIMEOUT)->required(false)->default_val(DEFAULT_TIMEOUT)->default_str(std::to_string(DEFAULT_TIMEOUT));
    cli->add_option<std::string>(descriptors::DESC_HOST)->required(true);
    cli->add_option<std::size_t>(descriptors::DESC_PORT)->required(false)->default_val(DEFAULT_PORT)->default_str(std::to_string(DEFAULT_PORT));

	cli->add_flag(descriptors::DESC_VERSION, [](auto) {
		fmt::print("{0}\n", "1.0.0.0");
		exit(0);
	});

    return cli;
}

std::unique_ptr<client_instance> open_client(const std::string& host, const std::string& protocol) {
    return std::make_unique<client_instance>(host, protocol);
}

void verify_cli(const commandline::kfclient_cli&) {
    using namespace commandline;

    // todo: make sure invalid options are not used together
}

template <typename T>
void report_detail_impl(fort::utf8_table& t, const char* name, const T& value) {
    std::vector<std::string> row(2);
    row[0] = name;
    if constexpr (std::disjunction_v<std::is_same<T, std::uint8_t>, std::is_same<T, std::int8_t>>)
        row[1] = fmt::format("{}", static_cast<std::uint32_t>(value));
    else 
        row[1] = fmt::format("{}", value);
    t.range_write_ln(row.begin(), row.end());
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define report_detail(T, C, N)\
    report_detail_impl(T, #N, (C).N);

int report_details(client_instance& instance, const commandline::kfclient_cli& cli) {
    try {
        const auto& details = instance.client->request_details();
        
        fort::utf8_table table;
        set_border_style(table, cli);

        table << fort::header;
        table.range_write_ln(DETAIL_HEADERS.begin(), DETAIL_HEADERS.end());

        report_detail(table, details, protocol);
        report_detail(table, details, hostname);
        report_detail(table, details, map);
        report_detail(table, details, game_dir);
        report_detail(table, details, game_description);
        report_detail(table, details, steam_app_id);
        report_detail(table, details, player_count);
        report_detail(table, details, player_cap);
        report_detail(table, details, operating_system);
        report_detail(table, details, password_set);
        report_detail(table, details, version);
        report_detail(table, details, waves_total);
        report_detail(table, details, waves_current);

        fmt::print("{0}\n", table.to_string());

        return 0;
    } catch (const std::exception& ex) {
        fmt::print(std::cerr, "could not successfully obtain details: {}\n", ex.what());
        return 2;
    }
}

int report_rules(client_instance& instance, const commandline::kfclient_cli& cli) {
    try {
        const auto& rules = instance.client->request_rules();

        fort::utf8_table table;
        set_border_style(table, cli);

        table << fort::header;
        table.range_write_ln(RULE_HEADERS.begin(), RULE_HEADERS.end());

        for (const auto& rule : rules.rules) {
            std::vector<std::string> row(RULE_HEADERS.size());
            row[0] = rule.name;
            
            if (std::holds_alternative<bool>(rule.value)) {
                row[1] = fmt::format("{}", std::get<bool>(rule.value));
            } else if (std::holds_alternative<double>(rule.value)) {
                row[1] = fmt::format("{}", std::get<double>(rule.value));
            } else if (std::holds_alternative<std::string>(rule.value)) {
                row[1] = fmt::format("{}", std::get<std::string>(rule.value));
            }
            table.range_write_ln(row.begin(), row.end());
        }

        fmt::print("{0}\n", table.to_string());

        return 0;
    } catch (const std::exception& ex) {
        fmt::print(std::cerr, "could not successfully obtain rules: {}\n", ex.what());
        return 2;
    }
}

int report_players(client_instance& instance, const commandline::kfclient_cli& cli) {
    try {
        const auto& players = instance.client->request_players();

        fort::utf8_table table;
        set_border_style(table, cli);

        table << fort::header;
        table.range_write_ln(PLAYER_HEADERS.begin(), PLAYER_HEADERS.end());

        for (const auto& player : players.players) {
            std::vector<std::string> row(PLAYER_HEADERS.size());
            row[0] = fmt::format("{}", static_cast<std::uint32_t>(player.id));
            row[1] = player.name;
            row[2] = fmt::format("{}", player.score);
            row[3] = fmt::format("{}", player.time);
            table.range_write_ln(row.begin(), row.end());
        }

        fmt::print("{0}\n", table.to_string());

        return 0;
    } catch (const std::exception& ex) {
        fmt::print(std::cerr, "could not successfully obtain players: {}\n", ex.what());
        return 2;
    }
}

int main(int argc, const char* argv[]) {
    using namespace commandline;

	// create the dynacli CLI11 wrapper
    std::unique_ptr<commandline::kfclient_cli> cli;
    bool verbose = false;

	try {
        cli = create_cli();
    } catch (const std::exception& error) {
        fmt::print(std::cerr, "error: cannot initialize cli parser: {}\n", error.what());
        return EXIT_FAILURE;
    }

    // parse argv
	try {
		cli->command().parse(argc, argv);
        verbose = cli->isset(descriptors::NAME_VERBOSE);
	} catch (const CLI::ParseError& e) {
		return cli->command().exit(e);
	}

    std::unique_ptr<client_instance> client = nullptr;

    try {
        const auto& host = cli->get<std::string>(descriptors::NAME_HOST);
        const auto& port = cli->get<std::size_t>(descriptors::NAME_PORT);
        client = open_client(host, fmt::format("{}", port));
        if (verbose) fmt::print("error: connection established to udp://{}:{}\n", host, port);
    } catch (const std::exception& ex) {
        fmt::print(std::cerr, "error: could not successfully instantiate client: {}\n", ex.what());
        return EXIT_FAILURE;
    }

    verify_cli(*cli);

    if (cli->isset(descriptors::NAME_PLAYER_COUNT)) {
        try {
            const auto& players = client->client->request_players();
            fmt::print("online players: {}\n", static_cast<std::size_t>(players.count));
        } catch (const std::exception& ex) {
            fmt::print(std::cerr, "could not successfully obtain player count: {}\n", ex.what());
            return EXIT_FAILURE;
        }
    } else {
        if (cli->isset(descriptors::NAME_REPORT)) {
            const auto& report_filters = cli->get<std::vector<std::string>>(descriptors::NAME_REPORT);
            for (const auto& f : FILTER_PRECEDENCE) {
                auto iter = std::find_if(report_filters.begin(), report_filters.end(), [&f](const auto& v){
                    return f == v || (v.size() == 1 && f[0] == v[0]);
                });

                if (iter != report_filters.end() && reporters.count(f) != 0) {
                    auto status = reporters.at(f)(*client, *cli);
                    if (status != 0)
                        return status;
                }
            }
        }
    }
}