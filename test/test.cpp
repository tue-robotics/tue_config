#include <tue/config/reader.h>
#include <tue/config/writer.h>
#include <tue/config/data.h>

#include <tue/config/yaml_emitter.h>

#include <tue/config/binary_writer.h>
#include <tue/config/binary_reader.h>

#include <tue/config/configuration.h>

#include <iostream>
#include <sstream>

// ----------------------------------------------------------------------------------------------------

void test1()
{
}

// ----------------------------------------------------------------------------------------------------

void test2_bar(tue::Configuration config)
{
    double y;
    if (config.value("y", y))
        std::cout << "y = " << y << std::endl;

    // Try to move out of scope. This should not affect the caller.
    config.endGroup();
    config.endGroup();
    config.endGroup();
}

void test2_foo(tue::Configuration config)
{
    std::cout << "----- test2_foo -----" << std::endl;
    std::cout << config << std::endl;

    if (config.readGroup("parameters"))
    {
        test2_bar(config.limitScope());
        config.endGroup();
    }

    int v;
    if (config.value("abc", v))
        std::cout << "abc = " << v << std::endl;

    // Open it again accidentally, but forget to close
    if (config.readGroup("parameters"))
        std::cout << "Succesfully opened" << std::endl;

    std::cout << config << std::endl;

    std::cout << "---------------------" << std::endl;
}

void test2()
{
    // Writing a config
    tue::Configuration config;
    config.setValue("main_value", 10);

    config.writeGroup("group1");

    config.setValue("abc", 123);
    config.setValue("def", "bla");

    config.writeGroup("parameters");
    config.setValue("x", 0.1);
    config.setValue("y", 0.2);
    config.setValue("z", 0.3);
    config.endGroup();

    config.endGroup();

    std::cout << config.toYAMLString() << std::endl;

    // Reading
    if (config.readGroup("group1"))
    {
        test2_foo(config);
        config.endGroup();
    }

    int main_value;
    if (config.value("main_value", main_value))
        std::cout << "main_value = " << main_value << std::endl;
}

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    tue::config::Configuration cfg;
    tue::config::Writer writer(cfg);

    writer.setValue("x", 3.1);
    writer.setValue("y", 4);

    writer.writeGroup("foo");
    writer.setValue("bar", 5);
    writer.setValue("bla", 7);

    writer.writeArray("array");

    writer.nextArrayItem();
    writer.setValue("x", 123);
    writer.setValue("y", 567);
    writer.end();

    writer.nextArrayItem();
    writer.setValue("x", 456);
    writer.setValue("y", 567);

    writer.writeArray("array2");

    writer.nextArrayItem();
    writer.setValue("foo", 1);
    writer.setValue("bar", 2);
    writer.end();

    writer.end();

    writer.end();

    writer.end();

    writer.end(); // end foo


    // TODO: "z" is still not placed right!

    writer.setValue("z", 99);

    std::cout << "--------------------------------------------------" << std::endl;

    tue::config::YAMLEmitter emitter;
    std::stringstream ss;
    emitter.emit(cfg, ss);
    std::cout << ss.str() << std::endl;

    std::cout << "--------------------------------------------------" << std::endl;

    {
        tue::config::Reader reader(cfg);

        float x, y, z;
        if (reader.value("x", x) && reader.value("y", y) && reader.value("z", z))
            std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;

        reader.read("foo");

        int bla;
        reader.value("bla", bla);
        std::cout << "bla = " << bla << std::endl;

        reader.read("array");
        while(reader.next())
        {
            int x;
            if (reader.value("x", x))
                std::cout << " - x = " << x << std::endl;

            if (reader.read("array2"))
            {
                while(reader.next())
                {
                    int bar;
                    if (reader.value("bar", bar))
                        std::cout << "   array2.bar = " << bar << std::endl;
                }
                reader.end();
            }
        }

        reader.end();

        reader.value("bla", bla);
        std::cout << "bla = " << bla << std::endl;

        if (reader.value("x", x) && reader.value("y", y) && reader.value("z", z))
            std::cout << "x = " << x << ", y = " << y << ", z = " << z << std::endl;
    }

    std::cout << "--------------------------------------------------" << std::endl;

    test2();

    std::cout << "--------------------------------------------------" << std::endl;

//    tue::config::Configuration cfg2;
//    tue::config::Writer bw(cfg2);

    tue::config::BinaryWriter bw;

    std::cout << "Begin" << std::endl;
    bw.writeGroup("mesh");

    bw.writeArray("vertices");

    for(unsigned int i = 0; i < 10; ++i)
    {
        bw.nextArrayItem();
        bw.setValue("x", 6);
        bw.setValue("y", 5);
        bw.setValue("z", 4);
    }

    bw.end();
    bw.end();

    std::cout << "Writing done" << std::endl;


//    bw.print();


    std::cout << bw.byte_size() << std::endl;

    tue::config::BinaryReader br(bw.dataPointer(), bw.byte_size());
    br.read("mesh");
    br.read("vertices");

    int i = 0;
    while(br.next())
    {
        ++i;
        int x, y, z;
        if (br.value("x", y) && br.value("y", x) && br.value("z", z))
            std::cout << i << ": " << x << ", " << y << ", " << z << std::endl;
    }

    br.end();
    br.end();

    std::cout << "Reading done" << std::endl;

    return 0;
}
