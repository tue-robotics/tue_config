#include "tue/config2/json.h"

#include "tue/config2/data.h"

namespace tue
{
namespace config2
{
namespace json
{

// ----------------------------------------------------------------------------------------------------

struct EmitterImpl
{

    EmitterImpl(const Data& data_) : data(data_) {}

    void emitJSON(const Data& data, const std::map<std::string, Node>& map, const std::string& indent, std::ostream& out)
    {
        out << "{" << newline;

        std::string new_indent = indent + tab;

        for(std::map<std::string, Node>::const_iterator it = map.begin(); it != map.end(); ++it)
        {
            if (it != map.begin())
                out << "," << newline;

            out << new_indent;

            if (quoted_keys)
                out << "\"" << it->first << "\"";
            else
                out << it->first;

            out << delimiter;

            const Node& n = it->second;
            if (n.type == FLOAT)
            {
                out << "FLOAT";
            }
            else if (n.type == INT)
            {
                out << "INT";
            }
            else if (n.type == STRING)
            {
                out << "\"STRING\"";
            }
            else if (n.type == MAP)
            {
                emitJSON(data, data.maps[n.idx], new_indent, out);
            }
            else if (n.type == ARRAY)
            {
                out << "[" << std::endl;
                const std::vector<Node>& array = data.arrays[n.idx];
                for(std::vector<Node>::const_iterator it2 = array.begin(); it2 != array.end(); ++it2)
                {
                    if (it2 != array.begin())
                        out << "," << std::endl;

                    out << new_indent + tab;
                    emitJSON(data, data.maps[it2->idx], new_indent + tab, out);
                }
                out << newline << new_indent << "]";
            }
        }

        out << newline << indent << "}";
    }

    const Data& data;
    std::string newline;
    std::string tab;
    std::string delimiter;
    bool quoted_keys;

};

// ----------------------------------------------------------------------------------------------------

bool write(const Data& data, const std::string& filename)
{

}

// ----------------------------------------------------------------------------------------------------

bool write(const Data& data, std::ostream& out)
{
    if (data.maps.empty())
        return false;

    EmitterImpl p(data);
    p.delimiter = ":";

//    if (!(options & EMIT_MINIMAL) || (options & EMIT_YAML))
    {
        p.newline = "\n";
        p.tab = "    ";
        p.delimiter += " ";
    }

//    p.quoted_keys = (options & EMIT_JSON) || (options & EMIT_QUOTED_KEYS);
    p.quoted_keys = true;

    std::string indent = "";

    p.emitJSON(data, data.maps[0], indent, out);

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool read(const std::string& filename, Data& data)
{
    return false;
}

// ----------------------------------------------------------------------------------------------------

bool read(std::istream& in, Data& data)
{
    return false;
}

}

} // end namespace tue

} // end namespace config2

