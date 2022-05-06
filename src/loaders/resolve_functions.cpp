#include "resolve_functions.h"

#include <vector>
#include <ros/package.h>
#include <stdlib.h>     /* getenv */

#include <tue/filesystem/path.h>

#include <iostream>
#include <iterator>

namespace tue
{

namespace config
{

enum resolveResult
{
    failed,
    success,
    skipped
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

resolveResult executeResolvefunction(const std::vector<std::string>& args, const std::string& source, std::string& result, std::stringstream& error, const ResolveConfig& config)
{
    if (args[0] == "rospkg" && args.size() == 2)
    {
        if (!config.rospkg)
            return resolveResult::skipped;

        result = ros::package::getPath(args[1]);
        if (result.empty())
        {
            error << "ROS package '" << args[1] << "' unknown.";
            return resolveResult::failed;
        }

        return resolveResult::success;
    }
    else if (args[0] == "env" && (args.size() == 2 || args.size() == 3))
    {
        if (!config.env)
            return resolveResult::skipped;

        char* env_value;
        env_value = getenv(args[1].c_str());
        if (env_value == nullptr)
        {
            if (args.size() == 3)
            {
                // Default value
                result = args[2];
                return resolveResult::success;
            }

            error << "Environment variable '" << args[1] << "' unknown.";
            return resolveResult::failed;
        }

        result = env_value;
        return resolveResult::success;
    }
    else if (args[0] == "file" && args.size() == 2)
    {
        if (!config.file)
            return resolveResult::skipped;

        const std::string& filename = args[1];
        if (filename[0] == '/')
        {
            result = filename;
            return resolveResult::success;
        }

        tue::filesystem::Path parent_path = tue::filesystem::Path(source).parentPath();
        if (parent_path.string().empty())
            result = filename;
        else
            result = parent_path.join(filename).string();

        return resolveResult::success;
    }

    error << "Unknown resolve function: '" << args[0] << "' with " << args.size() - 1 << " arguments.";

    return resolveResult::failed;
}

// ----------------------------------------------------------------------------------------------------

resolveResult parseResolveFunction(const std::string& str, const std::string& source, std::size_t& i, std::string& result, std::stringstream& error, const ResolveConfig& config)
{
    std::vector<std::string> args;
    args.push_back("");

    bool inner_function_skipped = false;

    std::size_t i_start = i;

    for(; i < str.size();)
    {
        char c = str[i];

        if (c == '$' && (i + 1) < str.size() && str[i + 1] == '(')
        {
            // Skip "$("
            i += 2;

            std::string arg;
            resolveResult result = parseResolveFunction(str, source, i, arg, error, config);
            if (result == resolveResult::failed)
                return resolveResult::failed;
            else if (result == resolveResult::skipped)
                inner_function_skipped = true;

            args.back() += arg;
            continue;
        }
        else if (c == ')')
        {
            ++i;

            // Check if last argument is empty. If so, remove it
            if (args.back().empty())
                args.pop_back();

            // Check if args are empty. Is so, return false
            if (args.empty())
            {
                error << "Empty resolve function.";
                return resolveResult::failed;
            }

            resolveResult result_code = executeResolvefunction(args, source, result, error, config);
            if (result_code == resolveResult::skipped)
            {
                argsToString(args, result);
                return result_code;
            }

            if (inner_function_skipped)
            {
                std::string resolved_args;
                argsToString(args, resolved_args);
                error << "Inner resolve function skipped, but not this one: " << resolved_args << ".";
                return resolveResult::failed;
            }

            return result_code;
        }
        else if (c == ' ')
        {
            if (!args.back().empty())
                args.push_back("");
            ++i;
        }
        else
        {
            args.back() += c;
            ++i;
        }
    }

    error << "Missing ')'.";
    return resolveResult::failed;
}

// ----------------------------------------------------------------------------------------------------

bool resolve(const std::string& str, const std::string& source, std::string& result, std::stringstream& error, const ResolveConfig& config)
{
    // Easy way out when nothing needs to be resolved
    if (config.AllFalse())
    {
        result = str;
        return true;
    }

    std::size_t i = 0;
    while(i < str.size())
    {
        std::size_t i_sign = str.find("$(", i);
        if (i_sign == std::string::npos)
        {
            result += str.substr(i);
            return true;
        }

        result += str.substr(i, i_sign - i);

        // Skip until after "$("
        i = i_sign + 2;

        std::string subresult;
        if (!parseResolveFunction(str, source, i, subresult, error, config)) // failed: 0
            return false;

        result += subresult;
    }

    return true;
}

}

}

