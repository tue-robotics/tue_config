#include <tue/config/configuration.h>

#include <iostream>

// ----------------------------------------------------------------------------------------------------

void test1(const std::string& filename)
{
    tue::Configuration config;

    config.loadFromYAMLFile(filename);

    std::cout << config << std::endl;

    while(true)
    {
        if (config.sync())
        {
            std::cout << "Changed:" << std::endl << std::endl << config << std::endl;
        }

        usleep(10000);
    }
}

// ----------------------------------------------------------------------------------------------------

void test2()
{
    tue::Configuration config;

    config.writeArray("array");

    config.addArrayItem();

    config.writeArray("array2");

    config.addArrayItem();
    config.setValue("a", 1);
    config.endArrayItem();

    config.addArrayItem();
    config.setValue("b", 2);
    config.endArrayItem();

    config.endArray();

    config.endArrayItem();

    config.addArrayItem();
    config.setValue("lib", "foo");
    config.endArrayItem();

    config.addArrayItem();
    config.setValue("lib", "bar");
    config.endArrayItem();

    config.endArray();

    config.readArray("array");

    while(config.nextArrayItem())
    {
        std::cout << config << std::endl;
    }

    config.endArray();

    std::cout << "----------" << std::endl;

    std::cout << config << std::endl;
}

// ----------------------------------------------------------------------------------------------------

void test3()
{
    tue::Configuration config;
    config.writeGroup("group");

    // create sub scope
    tue::Configuration config2 = config.limitScope();
    config2.writeGroup("foo");
    config2.setValue("bar", 123);

    // Accidentally do not close group 'foo': ...
    // config2.endGroup(); // foo

    // config still refers to scope 'group', so NOT closing 'foo' causes NO problems
    config.endGroup(); // group

    std::cout << config << std::endl;

    std::cout << config.value("group.foo.br") << std::endl;

    config.readGroup("group");
    config.readGroup("foo");
    std::cout << config.value("br") << std::endl;
    config.endArray();
    config.endGroup();
    config.endGroup();

    std::cout << config.error() << std::endl;
}

// ----------------------------------------------------------------------------------------------------

void test4()
{
    tue::Configuration config;

    config.setValue("v1", "test");

    int v;
    std::cout << config.value("v1", v) << std::endl;

    if (config.hasError())
    {
        std::cout << "ERROR: " << config.error() << std::endl;
    }
    else
    {
        std::cout << v << std::endl;
    }
}

// ----------------------------------------------------------------------------------------------------

void test5()
{
    tue::Configuration config1;
    config1.setValue("foo", "test");
    config1.setValue("bar", 123.456);
    config1.writeGroup("group1");
    config1.setValue("test", 100);
    config1.setValue("test2", "bla");
    config1.endGroup();

    tue::Configuration config2;
    config2.setValue("foo", "replaced");
    config2.writeGroup("group1");
    config2.setValue("test2", "also replaced");
    config2.endGroup();
    config2.writeGroup("group2");
    config2.setValue("name", "this is a new group");
    config2.endGroup();

    config1.add(config2);

    std::cout << config1 << std::endl;
    std::cout << "---" << std::endl;
    std::cout << config2 << std::endl;
}

// ----------------------------------------------------------------------------------------------------

int main(int argc, char **argv) {

    tue::Configuration config;

    if (argc > 1)
    {
        test1(argv[1]);
    }
    else
    {
        test5();
    }

    return 0;
}
