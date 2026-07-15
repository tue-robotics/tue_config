#include "resolve_functions.h"
#include "tue/config/resolve_config.h"

// ament_index_cpp deprecated the single-argument get_package_share_directory in 1.13 and removed its
// header entirely in 1.14, replacing it with get_package_share_path. Guard on ament_index_cpp's own
// version (not the ROS distro), because that is the package the API lives in.
#include <ament_index_cpp/version.h>
#if AMENT_INDEX_CPP_VERSION_GTE(1, 13, 0)
#include <ament_index_cpp/get_package_share_path.hpp>
#else
#include <ament_index_cpp/get_package_share_directory.hpp>
#endif
#include <cstdlib> /* getenv */
#include <exception>
#include <sstream>
#include <string>
#include <vector>

#include <filesystem>

#include <cstdint>
#include <iostream>

namespace tue::config
{

namespace
{

enum class resolveResult : std::uint8_t
{
    FAILED,
    SUCCESS,
    SKIPPED
};

// ----------------------------------------------------------------------------------------------------

void argsToString(const std::vector<std::string>& args, std::string& output)
{
    output = "$(";
    for (auto it = args.cbegin(); it != args.cend(); ++it)
    {
        if (it != args.cbegin())
            output += " ";
        output += *it;
    }
    output += ")";
}

// ----------------------------------------------------------------------------------------------------

resolveResult executeResolvefunction(const std::vector<std::string>& args,
                                     const std::string& source,
                                     std::string& result,
                                     std::stringstream& error,
                                     const ResolveConfig& config)
{
    if (args[0] == "rospkg" && args.size() == 2)
    {
        if (!config.rospkg)
            return resolveResult::SKIPPED;

        try
        {
#if AMENT_INDEX_CPP_VERSION_GTE(1, 13, 0)
            result = ament_index_cpp::get_package_share_path(args[1]).string();
#else
            result = ament_index_cpp::get_package_share_directory(args[1]);
#endif
        }
        catch (const std::exception&)
        {
            result.clear();
        }
        if (result.empty())
        {
            error << "ROS package '" << args[1] << "' unknown.";
            return resolveResult::FAILED;
        }

        return resolveResult::SUCCESS;
    }
    if (args[0] == "env" && (args.size() == 2 || args.size() == 3))
    {
        if (!config.env)
            return resolveResult::SKIPPED;

        const char* const env_value = getenv(args[1].c_str());
        if (env_value == nullptr)
        {
            if (args.size() == 3)
            {
                // Default value
                result = args[2];
                return resolveResult::SUCCESS;
            }

            error << "Environment variable '" << args[1] << "' unknown.";
            return resolveResult::FAILED;
        }

        result = env_value;
        return resolveResult::SUCCESS;
    }
    if (args[0] == "file" && args.size() == 2)
    {
        if (!config.file)
            return resolveResult::SKIPPED;

        const std::string& filename = args[1];
        if (filename[0] == '/')
        {
            result = filename;
            return resolveResult::SUCCESS;
        }

        const std::filesystem::path parent_path = std::filesystem::path(source).parent_path();
        if (parent_path.empty())
            result = filename;
        else
            result = (parent_path / filename).string();

        return resolveResult::SUCCESS;
    }

    error << "Unknown resolve function: '" << args[0] << "' with " << args.size() - 1 << " arguments.";

    return resolveResult::FAILED;
}

// ----------------------------------------------------------------------------------------------------

// NOLINTNEXTLINE(misc-no-recursion) — recursive descent parser for nested "$( ... )" expressions.
resolveResult parseResolveFunction(const std::string& str,
                                   const std::string& source,
                                   std::size_t& i,
                                   std::string& result,
                                   std::stringstream& error,
                                   const ResolveConfig& config)
{
    std::vector<std::string> args;
    args.emplace_back("");

    bool inner_function_skipped = false;

    for (; i < str.size();)
    {
        char const c = str[i];

        if (c == '$' && (i + 1) < str.size() && str[i + 1] == '(')
        {
            // Skip "$("
            i += 2;

            std::string arg;
            resolveResult const result = parseResolveFunction(str, source, i, arg, error, config);
            if (result == resolveResult::FAILED)
                return resolveResult::FAILED;
            if (result == resolveResult::SKIPPED)
                inner_function_skipped = true;

            args.back() += arg;
            continue;
        }
        if (c == ')')
        {
            ++i;

            // Check if last argument is empty. If so, remove it
            if (args.back().empty())
                args.pop_back();

            // Check if args are empty. Is so, return false
            if (args.empty())
            {
                error << "Empty resolve function.";
                return resolveResult::FAILED;
            }

            const resolveResult result_code = executeResolvefunction(args, source, result, error, config);
            if (result_code == resolveResult::SKIPPED)
            {
                argsToString(args, result);
                return result_code;
            }

            if (inner_function_skipped)
            {
                std::string resolved_args;
                argsToString(args, resolved_args);
                error << "Inner resolve function skipped, but not this one: " << resolved_args << ".";
                return resolveResult::FAILED;
            }

            return result_code;
        }
        if (c == ' ')
        {
            if (!args.back().empty())
                args.emplace_back("");
            ++i;
        }
        else
        {
            args.back() += c;
            ++i;
        }
    }

    error << "Missing ')'.";
    return resolveResult::FAILED;
}

} // namespace

// ----------------------------------------------------------------------------------------------------

bool resolve(const std::string& str,
             const std::string& source,
             std::string& result,
             std::stringstream& error,
             const ResolveConfig& config)
{
    // Easy way out when nothing needs to be resolved
    if (config.AllFalse())
    {
        result = str;
        return true;
    }

    std::size_t i = 0;
    while (i < str.size())
    {
        std::size_t const i_sign = str.find("$(", i);
        if (i_sign == std::string::npos)
        {
            result += str.substr(i);
            return true;
        }

        result += str.substr(i, i_sign - i);

        // Skip until after "$("
        i = i_sign + 2;

        std::string subresult;
        if (parseResolveFunction(str, source, i, subresult, error, config) == resolveResult::FAILED)
            return false;

        result += subresult;
    }

    return true;
}

} // namespace tue::config
