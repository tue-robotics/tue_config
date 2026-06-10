#include "tue/config/reader_writer.h"
#include "tue/config/data.h"
#include "tue/config/node.h"

// Merge
#include "tue/config/map.h"
#include "tue/config/sequence.h"

#include <boost/make_shared.hpp>

#include <console_bridge/console.h>

// SDF
#include "tue/config/loaders/sdf.h"

// XML
#include "tue/config/loaders/xml.h"

// YAML
#include "tue/config/loaders/yaml.h"
#include "tue/config/yaml_emitter.h"
#include <sstream>

// Sync
#include <chrono>
#include <filesystem>

namespace tue
{
namespace config
{

namespace
{

// Convert a std::filesystem::file_time_type to std::time_t.
// C++17-portable conversion: assumes file_clock epoch maps to system_clock epoch
// (true on glibc/libstdc++ and libc++ on Linux).
std::time_t toTimeT(std::filesystem::file_time_type ftime)
{
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - decltype(ftime)::clock::now() + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

} // namespace

// ----------------------------------------------------------------------------------------------------

ReaderWriter::ReaderWriter() : idx_(0), scope_(0), cfg_(new Data), error_(new Error) {}

// ----------------------------------------------------------------------------------------------------

ReaderWriter::ReaderWriter(DataPointer& cfg) : idx_(cfg.idx), scope_(0), cfg_(cfg.data), error_(new Error)
{
    if (!cfg_)
    {
        cfg_.reset(new Data);
        cfg.data = cfg_;
    }
}

// ----------------------------------------------------------------------------------------------------

ReaderWriter::~ReaderWriter() {}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::read(const std::string& name, const NodeType type, const RequiredOrOptional opt)
{
    Label label;
    NodeIdx child_idx; // Needed for checking if the child node is indeed the type(map/array) we are looking for.
    if (cfg_->getLabel(name, label) && cfg_->nodes[idx_]->readGroup(label, child_idx))
    {
        // check if child matches the type you want to read.
        if (cfg_->nodes[child_idx]->type() == type)
        {
            idx_ = child_idx;
            return true;
        }
    }

    if (opt == REQUIRED)
        addError("Expected group: '" + name + "', found none.");

    return false;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::end()
{
    if (idx_ == scope_)
        return false;

    NodeIdx parent = cfg_->getParent(idx_);
    if (parent == static_cast<NodeIdx>(-1))
        return false;

    idx_ = parent;

    // If the parent is an array, go up one more
    if (cfg_->nodes[idx_]->type() == ARRAY)
        idx_ = cfg_->getParent(idx_);

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::next()
{
    if (cfg_->nodes[idx_]->type() == ARRAY)
        return cfg_->nodes[idx_]->firstChild(idx_);

    NodeIdx right_sibling = cfg_->getRightSibling(idx_);
    if (right_sibling == static_cast<NodeIdx>(-1))
        return false;

    idx_ = right_sibling;
    return true;
}

// ----------------------------------------------------------------------------------------------------

ReaderWriter ReaderWriter::limitScope() const
{
    ReaderWriter r(*this);
    r.scope_ = r.idx_;
    return r;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::hasChild(const std::string& name, NodeType type) const
{
    Label label;
    NodeIdx child_idx; // Needed for checking if the child node is indeed the type(map/array) we are looking for.
    if (cfg_->getLabel(name, label) && cfg_->nodes[idx_]->readGroup(label, child_idx))
    {
        // check if child matches the type you want to read.
        return cfg_->nodes[child_idx]->type() == type;
    }
    return false;
}

// ----------------------------------------------------------------------------------------------------

void ReaderWriter::addError(const std::string& msg)
{
    std::string& error_msg = error_->message;

    // build context
    std::vector<std::string> context;

    if (error_context_)
    {
        error_msg += *error_context_ + "\n\n";
    }
    else
    {
        // Default error context

        NodeIdx c = idx_;
        while (c != static_cast<NodeIdx>(-1))
        {
            const NodePtr& n = cfg_->nodes[c];
            context.push_back(n->name());
            c = cfg_->getParent(c);
        }

        if (context.size() > 1)
        {
            error_msg += "In '";

            for (int i = context.size() - 2; i > 0; --i)
            {
                error_msg += context[i] + ".";
            }

            error_msg += context[0] + "': \n\n";
        }
        else
        {
            error_msg += "In root of configuration:\n\n";
        }
    }

    error_msg += "    " + msg + "\n\n";
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::writeGroup(const std::string& name)
{
    if (cfg_->nodes[idx_]->type() != MAP)
        return false;

    Label label = cfg_->getOrAddLabel(name);

    if (cfg_->nodes[idx_]->readGroup(label, idx_))
        return true;

    // If no child node with name is known, create a new one.
    NodeIdx n = cfg_->addNode(boost::make_shared<Map>(label), idx_);

    if (!cfg_->nodes[idx_]->addGroup(label, n, idx_))
        return false;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::writeArray(const std::string& name)
{
    if (cfg_->nodes[idx_]->type() != MAP)
        return false;

    Label label = cfg_->getOrAddLabel(name);

    if (cfg_->nodes[idx_]->readGroup(label, idx_))
        return true;

    // If no child node with name is known, create a new one.
    NodeIdx n = cfg_->addNode(boost::make_shared<Sequence>(label), idx_);

    if (!cfg_->nodes[idx_]->addGroup(label, n, idx_))
        return false;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::addArrayItem()
{
    if (cfg_->nodes[idx_]->type() != ARRAY)
        return false;

    NodePtr node = boost::make_shared<Map>("");
    NodeIdx n = cfg_->addNode(node, idx_);

    NodeIdx previous;
    if (!cfg_->nodes[idx_]->add(n, previous))
        return false;

    if (previous != static_cast<NodeIdx>(-1))
        cfg_->setRightSibling(previous, n);

    idx_ = n;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::endArrayItem()
{
    NodeIdx parent = cfg_->getParent(idx_);
    if (parent == static_cast<NodeIdx>(-1))
        return false;

    idx_ = parent;
    return true;
}

// ----------------------------------------------------------------------------------------------------

std::string ReaderWriter::toYAMLString() const
{
    YAMLEmitter emitter;
    std::stringstream s;
    emitter.emit(DataConstPointer(cfg_, idx_), s);
    return s.str();
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::loadFromSDFFile(const std::string& filename)
{
    // Remove possible previous errors
    error_->message.clear();

    // Reset head
    idx_ = scope_;

    if (!config::loadFromSDFFile(filename, *this))
        return false;

    filename_ = filename;
    source_last_write_time_ = toTimeT(std::filesystem::last_write_time(filename_));

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::loadFromXMLFile(const std::string& filename)
{
    // Remove possible previous errors
    error_->message.clear();

    // Reset head
    idx_ = scope_;

    if (!config::loadFromXMLFile(filename, *this))
        return false;

    filename_ = filename;
    source_last_write_time_ = toTimeT(std::filesystem::last_write_time(filename_));

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::loadFromYAMLFile(const std::string& filename, const ResolveConfig& resolve_config)
{
    // Remove possible previous errors
    error_->message.clear();

    // Reset head
    idx_ = scope_;

    if (!config::loadFromYAMLFile(filename, *this, resolve_config))
        return false;

    filename_ = filename;
    source_last_write_time_ = toTimeT(std::filesystem::last_write_time(filename_));
    resolve_config_ = resolve_config;

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool ReaderWriter::sync()
{

    if (!filename_.empty())
    {
        std::time_t last_write_time;

        try
        {
            last_write_time = toTimeT(std::filesystem::last_write_time(filename_));
        }
        catch (std::filesystem::filesystem_error& e)
        {
            return false;
        }

        if (last_write_time > source_last_write_time_)
        {
            std::string extension = std::filesystem::path(filename_).extension().string();
            if (extension == ".sdf" || extension == ".world")
                loadFromSDFFile(filename_);
            else if (extension == ".xml")
                loadFromXMLFile(filename_);
            else if (extension == ".yml" || extension == ".yaml")
                loadFromYAMLFile(filename_, resolve_config_);
            else
                CONSOLE_BRIDGE_logError("[ReaderWriter::Sync] extension: '%s' is not supported.", extension.c_str());
            source_last_write_time_ = last_write_time;
            return true;
        }
    }

    return false;
}

// ----------------------------------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& out, const ReaderWriter& rw)
{
    out << DataPointer(rw.cfg_, rw.idx_);
    return out;
}

} // end namespace config

} // end namespace tue
