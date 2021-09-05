#ifndef commandline_definition_hpp
#define commandline_definition_hpp

#include "dynacli.hpp"
#include <string>
#include <vector>

namespace commandline {
	namespace descriptors {
		static constexpr auto PROGRAM_NAME = "kfclient";
		static constexpr auto PROGRAM_DESC = "a very simple command line utility that implements libkfclient for obtaining information about Killing Floor 2 servers";

		static constexpr auto NAME_VERSION = "version";
		static constexpr const option_descriptor DESC_VERSION(NAME_VERSION, "-v,--version", "display the version of kfclient.");

		static constexpr auto NAME_VERBOSE = "verbose";
		static constexpr const option_descriptor DESC_VERBOSE(NAME_VERBOSE, "-V,--verbose", "output more information.");

        static constexpr auto NAME_CLEAN = "clean";
        static constexpr const option_descriptor DESC_CLEAN(NAME_CLEAN, "-c,--clean-tables", "output tables without borders.");

        static constexpr auto NAME_TIMEOUT = "timeout";
        static constexpr const option_descriptor DESC_TIMEOUT(NAME_TIMEOUT, "-t,--timeout", "the timeout for datagram operations.");

        static constexpr auto NAME_PLAYER_COUNT = "playercount";
        static constexpr const option_descriptor DESC_PLAYER_COUNT(NAME_PLAYER_COUNT, "-P,--player-count", "output the player count and nothing else");

        static constexpr auto NAME_REPORT = "report";
        static constexpr const option_descriptor DESC_REPORT(NAME_REPORT, "-r,--report", "report a category of information (details, rules, players)");

        static constexpr auto NAME_HOST = "host";
        static constexpr const option_descriptor DESC_HOST(NAME_HOST, "host", "the host of the Killing Floor 2 server to connect to.");

        static constexpr auto NAME_PORT = "port";
        static constexpr const option_descriptor DESC_PORT(NAME_PORT, "port", "the port on [host] on which the Killing Floor 2 server can be polled for information. By default, this is 27015");
	}

	// Define the types that are supported in this implementation of DynaCli
	using kfclient_cli = commandline::dynacli<
		bool, std::size_t, std::string, std::vector<std::string>
	>;
}

#endif 