#include <tue/config2/data_writer.h>
#include <tue/config2/data_reader.h>
#include <tue/config2/json.h>

int main(int argc, char **argv)
{
    tue::config2::DataWriter w;

    w.writeGroup("test");
    w.writeInt("i1", 123);
    w.writeFloat("f1", 123.456);
    w.writeGroup("foo");
    w.writeString("s1", "abcdefg");
    w.writeBool("b1", false);
    w.endGroup();
    w.endGroup();

    w.writeGroup("test2");
    w.writeInt("i2", 123);
    w.writeFloat("f2", 123.456);
    w.writeString("s2", "abcdefg");
    w.writeBool("b2", true);
    w.endGroup();

    w.writeGroup("test");
    w.writeInt("i2", 456);

    w.writeArray("array1");
    for(int x = 0; x < 3; ++x)
    {
        w.writeArrayItem();

        w.writeArray("array2");
        for(int y = 0; y < 3; ++y)
        {
            w.writeArrayItem();
            w.writeInt("x", x);
            w.writeInt("y", y);
            w.endArrayItem();
        }
        w.endArray();

        w.endArrayItem();
    }
    w.endArray();
    w.endGroup();



    // - - - - - - - - - - - - - - - - - - - -

    tue::config2::json::write(w.data(), std::cout, 4);

    // - - - - - - - - - - - - - - - - - - - -

    tue::config2::DataReader r(w.data());
    if (r.readGroup("test"))
    {
        if (r.readArray("array1"))
        {
            while(r.nextArrayItem())
            {
                if (r.readArray("array2"))
                {
                    while(r.nextArrayItem())
                    {
                        double x, y;
                        r.readFloat("x", x);

                        if (x > 1.5)
                            r.setErrorContext("BLAAAA");

                        r.readFloat("q", y);
                        std::cout << "x = " << x << ", y = " << y << std::endl;
                    }
                    r.endArray();
                }
            }

            r.endArray();

            int i;
            if (r.readInt("i1", i))
                std::cout << "i1 = " << i << std::endl;
        }

        r.endGroup();
    }

    if (r.readGroup("test2"))
    {
        std::string i;
        if (r.readString("s", i))
            std::cout << "s2 = " << i << std::endl;
        r.endGroup();
    }

    if (r.hasError())
    {
        std::cout << r.error() << std::endl;
    }

    // - - - - - - - - - - - - - - - - - - - -

    if (argc > 1)
    {
        tue::config2::Data data;
        tue::config2::json::readFromFile(argv[1], data);
        tue::config2::json::write(data, std::cout, 4);
    }

    return 0;
}
