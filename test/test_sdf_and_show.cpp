#include <iostream>

#include <tue/config/read.h>
#include <tue/config/reader_writer.h>
#include <tue/config/write.h>

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    tue::config::ReaderWriter config;

    if (argc < 2)
    {
        std::cout << "Please provide sdf config file." << std::endl;
        return 1;
    }

    std::string filename = argv[1];

    try
    {
      if (!config.loadFromSDFFile(filename))
      {
        std::cout << "Error: could not load config from " << filename << std::endl;
      }
    }
    catch (tue::config::ParseException& e)
    {
        std::cout << "Error:" << std::endl << e.what() << std::endl;
        return 0;
    }

    if (config.hasError())
    {
        std::cout << "Error:" << std::endl << config.error() << std::endl;
        return 0;
    }

    tue::config::DataPointer data = config.data();
    std::cout << "\nResult: " << std::endl;
    tue::config::toStream(std::cout, data, tue::config::YAML, 2);

    return 0;
}
