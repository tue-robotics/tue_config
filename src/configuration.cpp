#include "tue/config/configuration.h"

#include "config_data.h"
#include "config_node.h"

// YAML parsing
#include "tue/config/loaders/yaml.h"

// Errors
#include <sstream>

// Sync
#include <tue/filesystem/path.h>
#include <boost/filesystem.hpp>

namespace tue
{

// ----------------------------------------------------------------------------------------------------

bool split(const std::string& composite_key, std::string& first, std::string& last)
{
    int idx = composite_key.find('.');
    if (idx >= 0)
    {
        first = composite_key.substr(0, idx);
        last = composite_key.substr(idx + 1);
        return true;
    }
    else
    {
        return false;
    }
}

// ----------------------------------------------------------------------------------------------------

void setValueRec(ConfigNode* cs, const std::string& key, const Variant& value)
{
    std::string first, last;
    if (!split(key, first, last))
        cs->values[key] = value;
    else
    {
        ConfigNode* cs_child = cs->children[first].get();
        cs_child->parent = cs;
        setValueRec(cs_child, last, value);
    }
}

// ----------------------------------------------------------------------------------------------------

Variant valueRec(const ConfigNode* cs, const std::string& key, const Variant& default_value)
{
    std::string first, last;
    if (!split(key, first, last))
    {
        std::map<std::string, Variant>::const_iterator it = cs->values.find(key);
        if (it == cs->values.end())
            return default_value;
        else
        {
            return it->second;
        }
    }
    else
    {
        std::map<std::string, ConfigNodePtr>::const_iterator it = cs->children.find(first);
        if (it == cs->children.end())
            return Variant();
        else
            return valueRec(it->second.get(), last, default_value);
    }
}

// ----------------------------------------------------------------------------------------------------

void removeRec(ConfigNode* cs, const std::string& key)
{
    std::string first, last;
    if (!split(key, first, last))
    {
        cs->children.erase(key);
        cs->values.erase(key);
    }
    else
    {
        std::map<std::string, ConfigNodePtr>::iterator it = cs->children.find(first);
        if (it != cs->children.end())
            removeRec(it->second.get(), last);
    }
}

// ----------------------------------------------------------------------------------------------------

Configuration::Configuration() : data_(new ConfigData), head_(&data_->root), scope_(head_)
{
}

// ----------------------------------------------------------------------------------------------------

Configuration::Configuration(const boost::shared_ptr<ConfigData>& data, ConfigNode* head, ConfigNode* scope)
    : data_(data), head_(head), scope_(scope)
{
}

// ----------------------------------------------------------------------------------------------------

Configuration::~Configuration()
{
}

// ----------------------------------------------------------------------------------------------------

void Configuration::setValue(const std::string& key, const Variant& value)
{
    setValueRec(head_, key, value);
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::checkValue(const std::string& key, Variant& v, RequiredOrOoptional opt)
{
    v = valueRec(head_, key, Variant());
    if (v.isValid())
        return true;
    else
    {
        if (opt == REQUIRED)
            addError("Missing key: '" + key + "'");
        return false;
    }
}

// ----------------------------------------------------------------------------------------------------z

bool Configuration::value(const std::string &key, double& d, RequiredOrOoptional opt)
{
    Variant v;
    if (!checkValue(key, v, opt))
        return false;

    if (!v.isReal())
    {
        addError("Key '" + key + "' must have type REAL");
        return false;
    }

    d = v.toReal();
    return true;
}

// ----------------------------------------------------------------------------------------------------z

bool Configuration::value(const std::string &key, float& f, RequiredOrOoptional opt)
{
    Variant v;
    if (!checkValue(key, v, opt))
        return false;

    if (!v.isReal())
    {
        addError("Key '" + key + "' must have type REAL");
        return false;
    }

    f = v.toReal();
    return true;
}

// ----------------------------------------------------------------------------------------------------z

bool Configuration::value(const std::string &key, int& i, RequiredOrOoptional opt)
{
    Variant v;
    if (!checkValue(key, v, opt))
        return false;

    if (!v.isInt())
    {
        addError("Key '" + key + "' must have type INT");
        return false;
    }

    i = v.toInt();
    return true;
}

// ----------------------------------------------------------------------------------------------------z

bool Configuration::value(const std::string &key, bool& b, RequiredOrOoptional opt)
{
    Variant v;
    if (!checkValue(key, v, opt))
        return false;

    if (!v.isInt())
    {
        addError("Key '" + key + "' must have type INT");
        return false;
    }

    b = v.toInt();
    return true;
}

// ----------------------------------------------------------------------------------------------------z

bool Configuration::value(const std::string &key, std::string& s, RequiredOrOoptional opt)
{
    Variant v;
    if (!checkValue(key, v, opt))
        return false;

    if (!v.isString())
    {
        addError("Key '" + key + "' must have type STRING");
        return false;
    }

    s = v.toString();
    return true;
}

// ----------------------------------------------------------------------------------------------------

Variant Configuration::value(const std::string& key, RequiredOrOoptional opt)
{
    Variant v = valueRec(head_, key, Variant());
    if (v.isValid())
        return v;
    else
    {
        if (opt == REQUIRED)
            addError("Missing key: '" + key + "'");
        return v;
    }
}

// ----------------------------------------------------------------------------------------------------

void Configuration::remove(const std::string& key)
{
    removeRec(head_, key);
}

// ----------------------------------------------------------------------------------------------------

void Configuration::writeGroup(const std::string& group)
{
    ConfigNodePtr c(new ConfigNode(group));
    c->parent = head_;

    head_->children[group] = c;
    head_ = c.get();
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::readGroup(const std::string& group)
{
    std::map<std::string, ConfigNodePtr>::iterator it = head_->children.find(group);
    if (it == head_->children.end())
    {
        return false;
    }

    head_ = it->second.get();
    return true;
}

// ----------------------------------------------------------------------------------------------------

void Configuration::endGroup()
{
    if (head_ == scope_)
    {
        addError("endGroup(): scope limit reached, cannot go up any further.");
        return;
    }

    if (head_->parent)
        head_ = head_->parent;
    else
        addError("endGroup(): ERROR: beginGroup() was not called");
}

// ----------------------------------------------------------------------------------------------------

void Configuration::writeArray(const std::string& array)
{
    writeGroup(array);
    head_->is_array = true;
    head_->idx_array = 0;
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::readArray(const std::string& array)
{
    std::map<std::string, ConfigNodePtr>::iterator it = head_->children.find(array);
    if (it == head_->children.end())
    {
        return false;
    }

    if (!it->second->is_array)
    {
        return false;
    }

    head_ = it->second.get();
    return true;
}

// ----------------------------------------------------------------------------------------------------

void Configuration::endArray()
{
    if (head_ == scope_)
    {
        addError("endArrayItem(): scope limit reached, cannot go up any further.");
        return;
    }

    if (head_->sequence_parent)
    {
        head_ = head_->sequence_parent->parent;
    }
    else if (head_->is_array)
    {
        head_ = head_->parent;
    }
    else
    {
        addError("endArray(): ERROR: beginArray() was not called");
    }
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::nextArrayItem()
{
    if (head_->sequence_parent)
    {
        // head_ is an item in a array. Therefore, change head to it's next sibling

        ConfigNode* a = head_->sequence_parent;
        ++a->idx_array;

        if (a->idx_array >= (int)a->sequence.size())
            return false;

        head_ = a->sequence[a->idx_array].get();
    }
    else
    {
        // head still points to the array itself. First check if this is indeed an array
        if (!head_->is_array)
        {
            addError("nextArrayItem(): ERROR: beginArray() was not called");
            return false;
        }

        // Test is the array is non-empty
        if (head_->sequence.empty())
            return false;

        // Move to its first array item
        head_ = head_->sequence.front().get();
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

void Configuration::addArrayItem()
{
    if (!head_->is_array)
    {
        addError("addArrayItem(): beginArray() was not called.");
        return;
    }

    ConfigNodePtr c(new ConfigNode(""));
    c->sequence_parent = head_;

    head_->sequence.push_back(c);

    head_ = c.get();
}

// ----------------------------------------------------------------------------------------------------

void Configuration::endArrayItem()
{
    if (head_ == scope_)
    {
        addError("endArrayItem(): scope limit reached, cannot go up any further.");
        return;
    }

    if (!head_->sequence_parent)
    {
        addError("endArrayItem(): addArrayItem() was not called.");
        return;
    }

    if (!head_->sequence_parent->is_array)
    {
        addError("endArrayItem(): beginArray() was not called.");
        return;
    }

    head_ = head_->sequence_parent;
}

// ----------------------------------------------------------------------------------------------------

Configuration Configuration::limitScope() const
{
    Configuration c(data_, head_, head_);
    return c;
}

// ----------------------------------------------------------------------------------------------------

void merge(ConfigNode* n1, const ConfigNode& n2, std::string& error)
{
    if (n1->is_array != n2.is_array)
    {
        error += "Trying to merge an array with a non-array: '" + n1->name + "' with '" + n2.name + "'\n";
        return;
    }

    // Merge children
    for(std::map<std::string, ConfigNodePtr>::const_iterator it2 = n2.children.begin(); it2 != n2.children.end(); ++it2)
    {
        const std::string& name = it2->first;
        std::map<std::string, ConfigNodePtr>::iterator it1 = n1->children.find(name);
        if (it1 == n1->children.end())
        {
            // n1 does not have that child. Copy it and add it to n1
            ConfigNodePtr c(new ConfigNode(*it2->second));
            c->parent = n1;
            n1->children[name] = c;
        }
        else
        {
            // n1 already has that child. Merge them.
            merge(it1->second.get(), *it2->second, error);
        }
    }

    // Merge values
    for(std::map<std::string, tue::Variant>::const_iterator it2 = n2.values.begin(); it2 != n2.values.end(); ++it2)
    {
        n1->values[it2->first] = it2->second;
    }

    // n1's sequence is completely replaced by n2's sequence
    n1->sequence.resize(n2.sequence.size());
    for(unsigned int i = 0; i < n2.sequence.size(); ++i)
    {
        ConfigNodePtr c(new ConfigNode(*n2.sequence[i]));
        c->parent = n1;
        n1->sequence[i] = c;
    }
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::add(const Configuration& config)
{
    std::string error;
    merge(head_, *config.head_, error);

    if (!error.empty())
    {
        addError(error);
        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::sync() {

    if (!filename_.empty() )
    {
        std::time_t last_write_time;

        try
        {
             last_write_time = tue::filesystem::Path(filename_).lastWriteTime();
        }
        catch (boost::filesystem3::filesystem_error& e)
        {
            return false;
        }

        if (last_write_time > source_last_write_time_)
        {
            loadFromYAMLFile(filename_);
            source_last_write_time_ = last_write_time;
            return true;
        }        
    }

    return false;
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::loadFromYAMLFile(const std::string& filename)
{
    // Remove possible previous errors
    data_->error.clear();

    // Reset head
    head_ = scope_;

    if (!config::loadFromYAMLFile(filename, *this))
        return false;

    filename_ = filename;
    source_last_write_time_ = tue::filesystem::Path(filename_).lastWriteTime();

    return true;
}

// ----------------------------------------------------------------------------------------------------

void Configuration::addError(const std::string& msg)
{
    // build context
    std::vector<std::string> context;

    ConfigNode* c = head_;
    while(c)
    {
        if (c->sequence_parent)
        {
            std::stringstream s;
            s << "(" << c->sequence_parent->idx_array << ")";
            context.push_back(s.str());
            c = c->sequence_parent;
        }
        else
        {
            context.push_back(c->name);
            c = c->parent;
        }
    }

    if (context.size() > 1)
    {
        data_->error += "In '";

        for(int i = context.size() - 2; i > 0; --i)
        {
            data_->error += context[i] + ".";
        }

        data_->error += context[0] + "': \n\n";
    }
    else
    {
        data_->error += "In root of configuration:\n\n";
    }

    data_->error += "    " + msg + "\n\n";
}

// ----------------------------------------------------------------------------------------------------

bool Configuration::hasError() const
{
    return !data_->error.empty();
}

// ----------------------------------------------------------------------------------------------------

const std::string& Configuration::error() const
{
    return data_->error;
}

// ----------------------------------------------------------------------------------------------------

std::ostream& operator<< (std::ostream& out, const Configuration& c)
{
    c.print(out, *c.head_, "");
    if (c.data_ && !c.data_->error.empty())
        out << "ERROR: " << c.data_->error << std::endl;
    return out;
}

// ----------------------------------------------------------------------------------------------------

void Configuration::print(std::ostream& out, const ConfigNode& cs, const std::string& indent) const
{
    for(std::map<std::string, Variant>::const_iterator it = cs.values.begin(); it != cs.values.end(); ++it)
    {
        out << indent << it->first << ": " << it->second << std::endl;
    }

    for(std::map<std::string, ConfigNodePtr>::const_iterator it = cs.children.begin(); it != cs.children.end(); ++it)
    {
        out << indent << it->first << ":" << std::endl;
        print(out, *it->second, indent + "    ");
    }

    for(unsigned int i = 0; i < cs.sequence.size(); ++i)
    {
        out << indent << "(" << i << ")" << std::endl;
        print(out, *cs.sequence[i], indent + "    ");
    }
}

}
