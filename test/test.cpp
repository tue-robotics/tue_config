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

void test3()
{
    tue::Configuration config;
    config.setValue("bla", 10);
    config.writeArray("array");

    config.addArrayItem();
    config.setValue("x", 1.2);
    config.setValue("y", 3.4);
    config.endArrayItem();

    config.addArrayItem();
    config.setValue("foo", "abc");
    config.setValue("bar", "def");
    config.endArrayItem();

    config.endArray();

    std::cout << config << std::endl;

    config.readArray("array");
    if (config.nextArrayItem())
        std::cout << config << std::endl;
    config.endArray();

    std::cout << config << std::endl;
}

// ----------------------------------------------------------------------------------------------------

void test4()
{
    tue::config::DataPointer cfg1;
    tue::config::Writer c1(cfg1);
    c1.setValue("x", 1.2);
    c1.setValue("y", 2.7);
    c1.writeGroup("group1");
    c1.writeGroup("group2");
    c1.writeGroup("group3");
    c1.endGroup(); // group 3

    c1.writeArray("array1");

    c1.addArrayItem();
    c1.setValue("foo", 10);
    c1.setValue("bar", "test");
    c1.endArrayItem();

    c1.endArray();

    c1.writeArray("array2");
    c1.addArrayItem();
    c1.setValue("a", 1);
    c1.setValue("b", 2);
    c1.endArrayItem();
    c1.endArray();

    c1.setValue("boo", "baa");
    c1.endGroup(); // group 2
    c1.endGroup(); // group 1

    std::cout << cfg1 << std::endl;

    // ---

    tue::config::DataPointer cfg2;
    tue::config::Writer c2(cfg2);
    c2.setValue("z", 4);
    c2.writeGroup("group1");
    c2.writeGroup("group2");
    c2.writeGroup("group3");
    c2.setValue("test", 123);
    c2.endGroup();

    c2.writeArray("array1");
    c2.addArrayItem();
    c2.setValue("e", 7);
    c2.setValue("f", 7);
    c2.endArrayItem();

    c2.addArrayItem();
    c2.setValue("L", 1);
    c2.setValue("S", 2);
    c2.endArrayItem();
    c2.endArray();

    c2.endGroup();

    c2.endGroup();

    // ---

    std::cout << "-----" << std::endl;
    std::cout << cfg2 << std::endl;

    cfg1.add(cfg2);

    std::cout << cfg1 << std::endl;
}

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
    tue::config::DataPointer cfg;
    tue::config::Writer writer(cfg);

    std::cout << "Empty: " << cfg.empty() << std::endl;

    writer.setValue("x", 3.1);

    std::cout << "Empty: " << cfg.empty() << std::endl;

    writer.setValue("y", 4);

    writer.writeGroup("foo");
    writer.setValue("bar", 5);
    writer.setValue("bla", 7);

    writer.writeArray("array");

    writer.addArrayItem();
    writer.setValue("x", 123);
    writer.setValue("y", 567);
    writer.end();

    writer.addArrayItem();
    writer.setValue("x", 456);
    writer.setValue("y", 567);

    writer.writeArray("array2");

    writer.addArrayItem();
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

        reader.read("foo", tue::config::MAP);

        int bla;
        reader.value("bla", bla);
        std::cout << "bla = " << bla << std::endl;

        reader.read("array", tue::config::ARRAY);
        while(reader.next())
        {
            int x;
            if (reader.value("x", x))
                std::cout << " - x = " << x << std::endl;

            if (reader.read("array2", tue::config::ARRAY))
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

    std::cout << "---------------------- TEST 2 ----------------------" << std::endl;
    test2();
    std::cout << "----------------------------------------------------" << std::endl;

    std::cout << "---------------------- TEST 3 ----------------------" << std::endl;
    test3();
    std::cout << "----------------------------------------------------" << std::endl;

    std::cout << "---------------------- TEST 4 ----------------------" << std::endl;
    test4();
    std::cout << "----------------------------------------------------" << std::endl;

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
