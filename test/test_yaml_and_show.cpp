#include <tue/config/reader.h>
#include <tue/config/writer.h>
#include <tue/config/data.h>

#include <tue/config/yaml_emitter.h>

#include <tue/config/binary_writer.h>
#include <tue/config/binary_reader.h>

#include <tue/config/reader_writer.h>

#include <tue/config/write.h>
#include <tue/config/read.h>

#include "../src/loaders/loader_functions.h"

#include <iostream>
#include <sstream>
#include <vector>

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: test_yaml_and_show YAML [resolve_env resolve_file resolve_rospkg]" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    tue::config::DataPointer data;

    tue::config::ResolveConfig resolve_config;
    if (argc >= 5)
    {
        bool parsing_error = false;
        std::vector<bool*> resolve_config_values = { &resolve_config.env, &resolve_config.file, &resolve_config.rospkg };
        for (uint index = 2; index<5; ++index)
        {
            std::string arg(argv[index]);
            char* pEnd;
            int i = static_cast<int>(std::strtol(arg.c_str(), &pEnd, 10));
            if (pEnd[0] == 0)
            {
                *resolve_config_values[index-2] = i;
                continue;
            }

            bool b;
            if (tue::config::strToBool(arg, b))
            {
                *resolve_config_values[index-2] = b;
                continue;
            }
            parsing_error = true;
            std::cerr << "Could not resolve '" << arg << "' to a boolean" << std::endl;
        }
        if (parsing_error)
            return 1;
    }

    std::cout << resolve_config.env << std::endl << resolve_config.file << std::endl << resolve_config.rospkg << std::endl;

    try
    {
        data = tue::config::fromFile(filename, resolve_config);
    }
    catch (tue::config::ParseException& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 0;
    }
    tue::config::toStream(std::cout, data, tue::config::YAML, 2);

    return 0;
}
