#include <tue/config2/data_writer.h>
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
    w.endGroup();

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

    // - - - - - - - - - - - - - - - - - - - -

    tue::config2::json::write(w.data(), std::cout, 8);

    return 0;
}
