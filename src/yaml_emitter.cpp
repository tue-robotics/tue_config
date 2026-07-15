#include "tue/config/yaml_emitter.h"
#include "tue/config/data.h"
#include "tue/config/data_pointer.h"
#include "tue/config/label.h"
#include "tue/config/node.h"

#include "tue/config/map.h"
#include "tue/config/node_type.h"
#include "tue/config/sequence.h"
#include "tue/config/types.h"
#include "tue/config/variant.h"
#include <map>
#include <ostream>
#include <string>
#include <vector>

namespace tue::config
{

// ----------------------------------------------------------------------------------------------------

YAMLEmitter::YAMLEmitter() = default;

// ----------------------------------------------------------------------------------------------------

namespace
{

// NOLINTNEXTLINE(misc-no-recursion) — recursive tree walk over the config document.
void emitRecursive(std::ostream& out,
                   const tue::config::Data& cfg,
                   const NodePtr& n,
                   const std::string& indent,
                   bool ignore_first_indent = false)
{
    if (n->type() == ARRAY)
    {
        const auto* array = dynamic_cast<Sequence*>(n.get());
        const std::vector<NodeIdx>& children = array->children_;
        for (const unsigned int it : children)
        {
            out << indent << "- ";
            emitRecursive(out, cfg, cfg.nodes[it], indent + "  ", true);
        }
    }
    else
    {
        const Map* map = dynamic_cast<Map*>(n.get());

        const std::map<Label, Variant>& values = map->values;
        for (const auto& value : values)
        {
            if (!ignore_first_indent)
                out << indent;
            else
                ignore_first_indent = false;

            out << cfg.getName(value.first) << ": " << value.second << '\n';
        }

        const std::map<Label, NodeIdx>& children = map->map_;
        for (const auto& it : children)
        {
            if (!ignore_first_indent)
                out << indent;
            else
                ignore_first_indent = false;

            out << cfg.getName(it.first) << ": " << '\n';
            emitRecursive(out, cfg, cfg.nodes[it.second], indent + "    ");
        }
    }
}

} // namespace

// ----------------------------------------------------------------------------------------------------

void YAMLEmitter::emit(const tue::config::DataConstPointer& cfg, std::ostream& out, const std::string& indent)
{
    if (!cfg.data)
        return;

    emitRecursive(out, *cfg.data, cfg.data->nodes[cfg.idx], indent);
}

} // namespace tue::config
