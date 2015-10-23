#include "tue/config/write.h"

#include "tue/config/data.h"
#include "tue/config/node.h"
#include "tue/config/map.h"
#include "tue/config/sequence.h"


namespace tue
{

namespace config
{

// ----------------------------------------------------------------------------------------------------

struct WriterImpl
{

    WriterImpl(std::ostream& out_, const tue::config::Data& cfg_) : out(out_), cfg(cfg_) {}

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    std::ostream& out;
    const tue::config::Data& cfg;
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

        for(int i = 0; i < n; ++i)
            tab += " ";

        for(int i = 0; i < n - 2; ++i)
            yaml_array_tab += " ";
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    void writeJSON(const NodePtr& n, const std::string& indent)
    {
        out << "{" << newline;

        Map* map = static_cast<Map*>(n.get());

        bool first = true;
        std::string new_indent = indent + tab;

        const std::map<Label, Variant>& values = map->values;
        for(std::map<Label, Variant>::const_iterator it = values.begin(); it != values.end(); ++it)
        {
            if (!first)
                out << "," << newline;
            else
                first = false;

            out << new_indent << "\"" << cfg.getName(it->first) << "\"" << delimiter;

            const Variant& v = it->second;
            if (v.isString())
                out << "\"" << v << "\"";
            else
                out << v;
        }

        const std::map<Label, NodeIdx>& children = map->map_;
        for(std::map<Label, NodeIdx>::const_iterator it = children.begin(); it != children.end(); ++it)
        {
            if (!first)
                out << "," << newline;
            else
                first = false;

            out << new_indent << "\"" << cfg.getName(it->first) << "\"" << delimiter;

            const NodePtr& m = cfg.nodes[it->second];

            if (m->type() == ARRAY)
            {
                out << "[" << newline;

                Sequence* array = static_cast<Sequence*>(m.get());
                const std::vector<NodeIdx>& children = array->children_;
                for(std::vector<NodeIdx>::const_iterator it = children.begin(); it != children.end(); ++it)
                {
                    if (it != children.begin())
                        out << "," << newline;

                    out << new_indent << tab;

                    writeJSON(cfg.nodes[*it], new_indent + tab);
                }

                out << newline << new_indent << "]";
            }
            else
            {
                writeJSON(cfg.nodes[it->second], new_indent);
            }
        }

        out << newline << indent << "}";
    }

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    void writeYAML(const NodePtr& n, const std::string& indent, bool array_item_start)
    {
        Map* map = static_cast<Map*>(n.get());

        const std::map<Label, Variant>& values = map->values;
        for(std::map<Label, Variant>::const_iterator it = values.begin(); it != values.end(); ++it)
        {
            if (array_item_start)
                array_item_start = false;
            else
                out << indent;

            out << cfg.getName(it->first) << delimiter << " " << it->second << std::endl;
        }

        const std::map<Label, NodeIdx>& children = map->map_;
        for(std::map<Label, NodeIdx>::const_iterator it = children.begin(); it != children.end(); ++it)
        {
            if (array_item_start)
                array_item_start = false;
            else
                out << indent;

            out << cfg.getName(it->first) << delimiter << std::endl;

            const NodePtr& m = cfg.nodes[it->second];

            if (m->type() == ARRAY)
            {
                Sequence* array = static_cast<Sequence*>(m.get());
                const std::vector<NodeIdx>& children = array->children_;
                for(std::vector<NodeIdx>::const_iterator it = children.begin(); it != children.end(); ++it)
                {
                    out << indent << yaml_array_tab << "- ";
                    writeYAML(cfg.nodes[*it], indent + tab, true);
                }
            }
            else
            {
                writeYAML(cfg.nodes[it->second], indent + tab, false);
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

}

// ----------------------------------------------------------------------------------------------------

bool toFile(const char* filename, const DataConstPointer& data, WriteType write_type, int indent_size)
{

}

// ----------------------------------------------------------------------------------------------------

}
}
