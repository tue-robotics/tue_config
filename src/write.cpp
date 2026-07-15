#include "tue/config/write.h"

#include "tue/config/data.h"
#include "tue/config/data_pointer.h"
#include "tue/config/label.h"
#include "tue/config/map.h"
#include "tue/config/node.h"
#include "tue/config/node_type.h"
#include "tue/config/sequence.h"
#include "tue/config/types.h"
#include "tue/config/variant.h"

#include <algorithm>
#include <fstream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace tue::config
{

// ----------------------------------------------------------------------------------------------------

struct WriterImpl
{

    WriterImpl(std::ostream& out_, const tue::config::Data& cfg_) : out(&out_), cfg(&cfg_) {}

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    std::ostream* out;
    const tue::config::Data* cfg;
    std::string tab;
    std::string yaml_array_tab;
    std::string newline;
    std::string delimiter;

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    void setIndentSize(int n)
    {
        tab.clear();

        if (n <= 0)
        {
            newline.clear();
            return;
        }

        newline = "\n";

        for (int i = 0; i < n; ++i)
            tab += " ";

        for (int i = 0; i < n - 2; ++i)
            yaml_array_tab += " ";
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // NOLINTNEXTLINE(misc-no-recursion) — recursive JSON serialisation of the config tree.
    void writeJSON(const NodePtr& n, const std::string& indent)
    {
        *out << "{" << newline;

        const Map* map = dynamic_cast<Map*>(n.get());

        bool first = true;
        std::string const new_indent = indent + tab;

        const std::map<Label, Variant>& values = map->values;
        for (const auto& value : values)
        {
            if (!first)
                *out << "," << newline;
            else
                first = false;

            *out << new_indent << "\"" << cfg->getName(value.first) << "\"" << delimiter;

            const Variant& v = value.second;
            if (v.isString())
                *out << "\"" << v << "\"";
            else
                *out << v;
        }

        const std::map<Label, NodeIdx>& children = map->map_;
        for (const auto& it : children)
        {
            if (!first)
                *out << "," << newline;
            else
                first = false;

            *out << new_indent << "\"" << cfg->getName(it.first) << "\"" << delimiter;

            const NodePtr& m = cfg->nodes[it.second];

            if (m->type() == ARRAY)
            {
                *out << "[" << newline;

                const auto* array = dynamic_cast<Sequence*>(m.get());
                const std::vector<NodeIdx>& children = array->children_;
                for (auto it = children.begin(); it != children.end(); ++it)
                {
                    if (it != children.begin())
                        *out << "," << newline;

                    *out << new_indent << tab;

                    writeJSON(cfg->nodes[*it], new_indent + tab);
                }

                *out << newline << new_indent << "]";
            }
            else
            {
                writeJSON(cfg->nodes[it.second], new_indent);
            }
        }

        *out << newline << indent << "}";
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    // NOLINTNEXTLINE(misc-no-recursion) — recursive YAML serialisation of the config tree.
    void writeYAML(const NodePtr& n, const std::string& indent, bool array_item_start)
    {
        const Map* map = dynamic_cast<Map*>(n.get());

        const std::map<Label, Variant>& values = map->values;
        for (const auto& value : values)
        {
            if (array_item_start)
                array_item_start = false;
            else
                *out << indent;

            *out << cfg->getName(value.first) << delimiter << " " << value.second << '\n';
        }

        const std::map<Label, NodeIdx>& children = map->map_;
        for (const auto& it : children)
        {
            if (array_item_start)
                array_item_start = false;
            else
                *out << indent;

            *out << cfg->getName(it.first) << delimiter << '\n';

            const NodePtr& m = cfg->nodes[it.second];

            if (m->type() == ARRAY)
            {
                const auto* array = dynamic_cast<Sequence*>(m.get());
                const std::vector<NodeIdx>& children = array->children_;
                for (const unsigned int it : children)
                {
                    *out << indent << yaml_array_tab << "- ";
                    writeYAML(cfg->nodes[it], indent + tab, true);
                }
            }
            else
            {
                writeYAML(cfg->nodes[it.second], indent + tab, false);
            }
        }
    }
};

// ----------------------------------------------------------------------------------------------------

void toStream(std::ostream& s, const DataConstPointer& data, WriteType write_type, int indent_size)
{
    if (!data.data)
        return;

    WriterImpl writer(s, *data.data);

    if (write_type == XML)
    {
        throw std::logic_error("Writing XML is not yet implemented");
    }
    if (write_type == YAML)
    {
        writer.delimiter = ":";
        writer.setIndentSize(std::max<int>(indent_size, 2));
        writer.writeYAML(data.data->nodes[data.idx], "", false);
    }
    else // JSON
    {
        if (indent_size > 0)
            writer.delimiter = ": ";
        else
            writer.delimiter = ":";

        writer.setIndentSize(indent_size);
        writer.writeJSON(data.data->nodes[data.idx], "");
    }
}

// ----------------------------------------------------------------------------------------------------

bool toFile(const std::string& filename, const DataConstPointer& data, WriteType write_type, int indent_size)
{
    return toFile(filename.c_str(), data, write_type, indent_size);
}

// ----------------------------------------------------------------------------------------------------

bool toFile(const char* filename, const DataConstPointer& data, WriteType write_type, int indent_size)
{
    std::ofstream out;
    out.open(filename);
    if (!out.is_open())
        return false;

    toStream(out, data, write_type, indent_size);
    return true;
}

// ----------------------------------------------------------------------------------------------------

} // namespace tue::config
