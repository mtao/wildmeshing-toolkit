#pragma once

// clang-format off
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <filesystem>
// clang-format on

namespace wmtk {

bool has_user_overloaded_logger_level();
///
/// Retrieves the current logger.
///
/// @return     A const reference to WildmeshToolkit's logger object.
///
spdlog::logger& logger();

///
/// Retrieves the logger for the optimization.
///
/// @return     A const reference to WildmeshToolkit's  optimization logger object.
///
spdlog::logger& opt_logger();

///
/// Setup a logger object to be used by WildmeshToolkit. Calling this function with other WildmeshToolkit function
/// is not thread-safe.
///
/// @param[in]  logger  New logger object to be used by WildmeshToolkit. Ownership is shared with WildmeshToolkit.
///
void set_logger(std::shared_ptr<spdlog::logger> logger);


///
/// Add a  file sink for the main wmtk logger. It is not thread-safe.
///
/// @param[in] sink_path  Path to the new sink
/// @param[in] replace_others Whether this sink replaces the default sink
/// @param[in] level level = the level that the output sink will write with. Default will be the current wmtk logger's first active sink's level
///
void add_file_sink(
    const std::filesystem::path& sink_name,
    bool replace_other_sinks = false,
    spdlog::level::level_enum level = spdlog::level::n_levels);

///
/// Setup a logger object to be used by WildmeshToolkit optimization. Calling this function with other WildmeshToolkit function
/// is not thread-safe.
///
/// @param[in]  logger  New logger object to be used by WildmeshToolkit logger. Ownership is shared with WildmeshToolkit.
///
void set_opt_logger(std::shared_ptr<spdlog::logger> logger);

[[noreturn]] void log_and_throw_error(const std::string& msg);

template <typename... Args>
[[noreturn]] void log_and_throw_error(const std::string& msg, const Args&... args)
{
    log_and_throw_error(fmt::format(fmt::runtime(msg), args...));
}
} // namespace wmtk
