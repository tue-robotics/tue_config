#include "resolve_functions.h"

#include <vector>
#include <ros/package.h>
#include <stdlib.h>     /* getenv */

#include <tue/filesystem/path.h>

#include <iostream>

namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

bool executeResolvefunction(const std::vector<std::string>& args, const std::string& source, std::string& result, std::stringstream& error)
{
    if (args[0] == "rospkg" && args.size() == 2)
    {
        result = ros::package::getPath(args[1]);
        if (result.empty())
        {
            error << "ROS package '" << args[1] << "' unknown.";
            return false;
        }

        return true;
    }
    else if (args[0] == "env" && (args.size() == 2 || args.size() == 3))
    {
        char* env_value;
        env_value = getenv(args[1].c_str());
        if (env_value == nullptr)
        {
            if (args.size() == 3)
            {
                // Default value
                result = args[2];
                return true;
            }

            error << "Environment variable '" << args[1] << "' unknown.";
            return false;
        }

        result = env_value;
        return true;
    }
    else if (args[0] == "file" && args.size() == 2)
    {
        const std::string& filename = args[1];
        if (filename[0] == '/')
        {
            result = filename;
            return true;
        }

        tue::filesystem::Path parent_path = tue::filesystem::Path(source).parentPath();
        if (parent_path.string().empty())
            result = filename;
        else
            result = parent_path.join(filename).string();

        return true;
    }

    error << "Unknown resolve function: '" << args[0] << "' with " << args.size() - 1 << " arguments.";

    return false;
}

// ----------------------------------------------------------------------------------------------------

bool parseResolveFunction(const std::string& str, const std::string& source, std::size_t& i, std::string& result, std::stringstream& error)
{
    std::vector<std::string> args;
    args.push_back("");

    for(; i < str.size();)
    {
        char c = str[i];

        if (c == '$' && (i + 1) < str.size() && str[i + 1] == '(')
        {
            // Skip "$("
            i += 2;

            std::string arg;
            if (!parseResolveFunction(str, source, i, arg, error))
                return false;

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
                return false;
            }

            return executeResolvefunction(args, source, result, error);
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
    return false;
}

// ----------------------------------------------------------------------------------------------------

bool resolve(const std::string& str, const std::string& source, std::string& result, std::stringstream& error)
{
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
        if (!parseResolveFunction(str, source, i, subresult, error))
            return false;

        result += subresult;
    }

    return true;
}

}

}

