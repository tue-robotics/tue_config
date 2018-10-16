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
        std::cout << "Please provide xml config file." << std::endl;
        return 1;
    }

    std::string filename = argv[1];
//    tue::config::DataPointer data;

    try
    {
//        data = tue::config::fromFile(filename);
      if (!config.loadFromXMLFile(filename))
      {
        std::cout << "Error: could not load config from " << filename << std::endl;
      }
    }
    catch (tue::config::ParseException& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
        return 0;
    }

    tue::config::DataPointer data = config.data();
    tue::config::toStream(std::cout, data, tue::config::YAML, 2);

    return 0;
}
