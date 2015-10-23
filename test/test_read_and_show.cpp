#include <tue/config/reader.h>
#include <tue/config/writer.h>
#include <tue/config/data.h>

#include <tue/config/yaml_emitter.h>

#include <tue/config/binary_writer.h>
#include <tue/config/binary_reader.h>

#include <tue/config/reader_writer.h>

#include <tue/config/write.h>
#include <tue/config/read.h>

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

    std::string filename = argv[1];
    tue::config::DataPointer data;

    try
    {
        data = tue::config::fromFile(filename);
    }
    catch (tue::config::ParseException& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 0;
    }
    tue::config::toStream(std::cout, data, tue::config::YAML, 2);

    return 0;
}
