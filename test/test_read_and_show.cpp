#include <tue/config/reader.h>
#include <tue/config/writer.h>
#include <tue/config/data.h>

#include <tue/config/yaml_emitter.h>

#include <tue/config/binary_writer.h>
#include <tue/config/binary_reader.h>

#include <tue/config/reader_writer.h>

#include <tue/config/write.h>

#include <iostream>
#include <sstream>

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    tue::config::ReaderWriter config;

    if (argc < 2)
    {
        std::cout << "Please provide yaml config file." << std::endl;
        return 1;
    }

    std::string yaml_filename = argv[1];
    config.loadFromYAMLFile(yaml_filename);

    if (config.hasError())
    {
        std::cout << config.error() << std::endl;
        return 1;
    }

//    std::cout << config.data() << std::endl;

    tue::config::toStream(std::cout, config.data(), tue::config::YAML, 2);

    return 0;
}
