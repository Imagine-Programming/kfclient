#pragma once 

#ifndef dynacli_h
#define dynacli_h

#include <CLI/CLI.hpp>
#include <unordered_map>
#include <type_traits>
#include <functional>
#include <optional>
#include <variant>
#include <memory>
#include <string>
#include <cstdint>

namespace commandline {
    namespace detail {
        /// <summary>
        /// An internal type to keep track of command options, flags and their values.
        /// </summary>
        /// <typeparam name="TTypes">a list of types of values that can be held by dynacli</typeparam>
        template <typename ...Types>
        struct internal_option {
            CLI::Option* Option = nullptr;
            std::variant<Types...> Value;
        };

        /*
            contains<...> can be used to determine if a variadic template argument pack contains a specific type.
            It is used in hindsight to ensure 'bool' is in that list, so that dynacli can handle flags.
        */

        template <typename T, typename... Args>
        struct contains;

        template <typename T>
        struct contains<T> : std::false_type {};

        template <typename T, typename... Args>
        struct contains<T, T, Args...> : std::true_type {};

        template <typename T, typename A, typename... Args>
        struct contains<T, A, Args...> : contains<T, Args...> {};
    }

    /// <summary>
    /// option_descriptor is a struct that should describe command line flags and options.
    /// See ArgumentNames.hpp and hindsight.cpp for usage.
    /// </summary>
    struct option_descriptor {
        const char* Name;
        const char* Flag;
        const char* Desc;

        /// <summary>
        /// Construct a constexpr new option_descriptor.
        /// </summary>
        /// <param name="name">The option or flag lookup name.</param>
        /// <param name="flag">The option or flag notation in CLI (i.e. "-v,--version", "-b", "positional").</param>
        /// <param name="desc">The description, as seen in the --help and --help-all lists.</param>
        constexpr option_descriptor(const char* name, const char* flag, const char* desc)
            : Name(name), Flag(flag), Desc(desc) {

        }
    };

    /// <summary>
    /// dynacli is a wrapper around CLI11 which can also hold the values for all options, by internally utilizing std::variant.
    /// </summary>
    /// <typeparam name="TTypes">A list of types supported by the created instance of dynacli.</typeparam>
    /// <remarks>
    /// note: aliases for types can not be used, has i.e. HANDLE and void* are the same; std::variant wants only one occurrence per type.
    ///       You can use dynacli<bool, void*> and instance.get<HANDLE>(name) for example, as variant will cast identical types.
    /// </remarks>
    template <typename ...TTypes>
    class dynacli {
        /// <summary>
        /// Bool is required.
        /// </summary>
        static_assert(
            detail::contains<bool, TTypes...>::value,
            "bool is required as a type to be able to work with flags."
        );

    private:
        CLI::App  created_command_;
        CLI::App& command_;

        std::unordered_map<std::string, detail::internal_option<TTypes...>> options_;
        std::unordered_map<std::string, std::unique_ptr<dynacli<TTypes...>>> subcommands_;

        /// <summary>
        /// Throws an exception when <paramref name="name"/> does not exist as option.
        /// </summary>
        /// <param name="name">The option name to assert the existence for.</param>
        void assert_exist(const std::string& name) const {
            if (!exists(name))
                throw std::runtime_error("invalid option name, does not exist: " + name);
        }

        /// <summary>
        /// Throws an exception when <paramref name="name"/> exists as option.
        /// </summary>
        /// <param name="name">The option name to assert the existence for.</param>
        void assert_not_exists(const std::string& name) const {
            if (exists(name))
                throw std::runtime_error("invalid option name, already exists: " + name);
        }

        /// <summary>
        /// Throws an exception when the option named <paramref name="name"/> does not hold the type <typeparamref name="TValue"/>.
        /// </summary>
        /// <param name="name">The option name to assert the type for.</param>
        /// <typeparam name="TValue">The required type.</typeparam>
        template<typename TValue>
        void assert_holds_alternative(const std::string& name) const {
            if (!holds_alternative<TValue>(name))
                throw std::runtime_error("invalid option type for: " + name);
        }

        /// <summary>
        /// Throws an exception when the subcommand named <paramref name="name"/> does not exist.
        /// </summary>
        /// <param name="name">The subcommand name to assert the existence for.</param>
        void assert_subcommand_exist(const std::string& name) const {
            if (!subcommand_exist(name))
                throw std::runtime_error("invalid subcommand: " + name);
        }
    public:
        /// <summary>
        /// Construct a new dynacli instance from a pointer to a <see cref="::CLI::App"/> instance.
        /// </summary>
        /// <param name="command">The CLI11 command.</param>
        explicit dynacli(CLI::App* command)
            : command_(*command) {

        }

        /// <summary>
        /// Construct a new dynacli instance from a reference to a <see cref="::CLI::App"/> instance.
        /// </summary>
        /// <param name="command">The CLI11 command.</param>
        explicit dynacli(CLI::App& command)
            : command_(command) {

        }

        /// <summary>
        /// Construct a new dynacli instance, which in turn constructs a new <see cref="::CLI::App"/> instance.
        /// </summary>
        /// <param name="description">The description for the command (or program).</param>
        explicit dynacli(const std::string& description)
            : created_command_(CLI::App(description)), command_(created_command_) {

        }

        /// <summary>
        /// Construct a new dynacli instance, which in turn constructs a new <see cref="::CLI::App"/> instance.
        /// </summary>
        /// <param name="description">The description for the command (or program).</param>
        /// <param name="app_name">The name for the command (or program).</param>
        dynacli(const std::string& description, const std::string& app_name)
            : created_command_(CLI::App(description, app_name)), command_(created_command_) {

        }

        /// <summary>
        /// Get a const reference to the internal <see cref="CLI::App"/> instance.
        /// </summary>
        /// <returns>The internal <see cref="CLI::App"/> instance.</returns>
        [[nodiscard]] const CLI::App& command() const {
            return command_;
        }

        /// <summary>
        /// Get a reference to the internal <see cref="CLI::App"/> instance.
        /// </summary>
        /// <returns>The internal <see cref="CLI::App"/> instance.</returns>
        CLI::App& command() {
            return command_;
        }

        /// <summary>
        /// Determines if an option with the name <paramref name="name"/> exists.
        /// </summary>
        /// <param name="name">The name of the option.</param>
        /// <returns>When the option exists, true is returned.</returns>
        [[nodiscard]] bool exists(const std::string& name) const {
            return options_.count(name) != 0;
        }

        /// <summary>
        /// Determines if an option with the name <paramref name="name"/> contains the type <typeparamref name="TValue"/>.
        /// </summary>
        /// <param name="name">The name of the option.</param>
        /// <typeparam name="TValue">The required value.</typeparam>
        /// <returns>When the option <paramref name="name"/> holds a <typeparamref name="TValue"/>, true is returned.</returns>
        template <typename TValue>
        [[nodiscard]] bool holds_alternative(const std::string& name) const {
            assert_exist(name);

            return std::holds_alternative<TValue>(options_.at(name).Value);
        }

        template <typename TValue>
        const TValue& get(const std::string& name) const {
            assert_exist(name);
            assert_holds_alternative<TValue>(name);
            return std::get<TValue>(options_.at(name).Value);
        }

        template <typename TValue>
        TValue& get(const std::string& name) {
            assert_exist(name);
            assert_holds_alternative<TValue>(name);
            return std::get<TValue>(options_.at(name).Value);
        }

        template <typename TValue>
        void set(const std::string& name, const TValue& value) {
            assert_exist(name);
            assert_holds_alternative<TValue>(name);
            std::get<TValue>(options_.at(name).Value) = value;
        }

        template <typename TValue>
        std::optional<TValue> get_optional(const std::string& name) const {
            if (!exists(name) || !holds_alternative<TValue>(name))
                return {};

            return std::get<TValue>(options_.at(name).Value);
        }

        template <typename TValue>
        std::optional<TValue> get_isset(const std::string& name) const {
            if (!isset(name))
                return {};
            return std::get<TValue>(options_.at(name).Value);
        }

        template <typename TValue>
        TValue get_isset_or(const std::string& name, const TValue& def = {}) const {
            if (!isset(name))
                return def;
            return std::get<TValue>(options_.at(name).Value);
        }

        [[nodiscard]] bool isset(const std::string& name) const {
            assert_exist(name);
            return static_cast<bool>(*options_.at(name).Option);
        }

        [[nodiscard]] bool anyset(const std::initializer_list<std::string>& names) const {
            for (const auto& name : names) {
                if (isset(name))
                    return true;
            }

            return false;
        }

        bool subcommand_anyset(const std::initializer_list<std::string>& subcommands, const std::initializer_list<std::string>& names) {
            for (const auto& subcommand : subcommands) {
                if (get_subcommand(subcommand).anyset(names))
                    return true;
            }

            return false;
        }

        CLI::Option* add_flag(const std::string& name, const std::string& flag, const std::string& desc) {
            assert_not_exists(name);

            auto& option = options_[name];
            option.Value = false;
            option.Option = command_.add_flag(flag, std::get<bool>(option.Value), desc);
            return option.Option;
        }

        CLI::Option* add_flag(const std::string& name, const std::string& flag, const std::string& desc, std::function<void(size_t)>&& func) {
            assert_not_exists(name);

            auto& option = options_[name];
            option.Value = {};
            option.Option = command_.add_flag(flag, std::move(func), desc);

            return option.Option;
        }

        CLI::Option* add_flag(const option_descriptor& descriptor) {
            return add_flag(descriptor.Name, descriptor.Flag, descriptor.Desc);
        }

        CLI::Option* add_flag(const option_descriptor& descriptor, std::function<void(size_t)>&& func) {
            return add_flag(descriptor.Name, descriptor.Flag, descriptor.Desc, std::move(func));
        }

        template <typename TValue>
        CLI::Option* add_option(const std::string& name, const std::string& flag, const std::string& desc) {
            static_assert(
                detail::contains<TValue, TTypes...>::value,
                "invalid type specified in TValue, is not contained in TTypes... of container"
                );

            assert_not_exists(name);
            TValue value = {};
            auto& option = options_[name];
            option.Value = value;
            option.Option = command_.add_option(flag, std::get<TValue>(option.Value), desc);

            return option.Option;
        }

        template <typename TValue>
        CLI::Option* add_option(const option_descriptor& descriptor) {
            return add_option<TValue>(descriptor.Name, descriptor.Flag, descriptor.Desc);
        }

        CLI::Option* get_option(const std::string& name) {
            assert_exist(name);
            return options_.at(name).Option;
        }

        dynacli<TTypes...>& add_subcommand(const std::string& name, const std::string& description) {
            auto app = command().add_subcommand(name, description);
            auto cli = std::make_unique<dynacli<TTypes...>>(app);
            subcommands_[name] = std::move(cli);
            return *subcommands_[name];
        }

        [[nodiscard]] bool subcommand_exist(const std::string& name) const {
            return subcommands_.count(name) != 0;
        }

        dynacli<TTypes...>& get_subcommand(const std::string& name) {
            assert_subcommand_exist(name);
            return *subcommands_.at(name);
        }

        const dynacli<TTypes...>& get_subcommand(const std::string& name) const {
            assert_subcommand_exist(name);
            return *subcommands_.at(name);
        }

        [[nodiscard]] bool is_subcommand_chosen() const {
            for (const auto& pair : subcommands_) {
                if (pair.second->command().parsed())
                    return true;
            }

            return false;
        }

        [[nodiscard]] bool is_subcommand_chosen(const std::string& name) const {
            assert_subcommand_exist(name);
            return subcommands_.at(name)->command().parsed();
        }

        [[nodiscard]] const std::string& get_chosen_subcommand_name() const {
            for (const auto& pair : subcommands_) {
                if (pair.second->command().parsed())
                    return pair.first;
            }

            throw std::runtime_error("no subcommand was chosen.");
        }

        dynacli<TTypes...>& operator[](const std::string& name) {
            return get_subcommand(name);
        }

        const dynacli<TTypes...>& operator[](const std::string& name) const {
            return get_subcommand(name);
        }
    };
}

#endif 