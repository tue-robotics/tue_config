#ifndef _DATA_READER_H_
#define _DATA_READER_H_

#include "tue/config2/reader.h"
#include "tue/config2/data.h"
#include <sstream>

namespace tue
{
namespace config2
{

// ----------------------------------------------------------------------------------------------------

enum RequiredOrOoptional
{
    REQUIRED,
    OPTIONAL
};

// ----------------------------------------------------------------------------------------------------

class DataReader : public Reader
{

public:

    DataReader(const Data& data) : data_(data), idx_(0, MAP), has_error_(false) {}

    ~DataReader() {}

    bool readInt(const std::string& key, int& v, RequiredOrOoptional opt = REQUIRED)
    {
        if (!data_.getInt(idx_, key, v))
        {
            if (opt == REQUIRED)
                addError("Expected integer with key '" + key + "', found none.");
            return false;
        }
        return true;
    }

    bool readFloat(const std::string& key, double& v, RequiredOrOoptional opt = REQUIRED)
    {
        if (!data_.getFloat(idx_, key, v))
        {
            if (opt == REQUIRED)
                addError("Expected float with key '" + key + "', found none.");
            return false;
        }
        return true;
    }

    bool readString(const std::string& key, std::string& v, RequiredOrOoptional opt = REQUIRED)
    {
        if (!data_.getString(idx_, key, v))
        {
            if (opt == REQUIRED)
                addError("Expected string with key '" + key + "', found none.");
            return false;
        }
        return true;
    }

    bool readGroup(const std::string& name, RequiredOrOoptional opt = OPTIONAL)
    {
        DataIndex new_idx = data_.readGroup(idx_, name);
        if (new_idx.type == INVALID)
        {
            if (opt == REQUIRED)
                addError("Expected group with name '" + name + "', found none.");
            return false;
        }

        type_stack_.push_back('g');
        context_stack_.push_back(name);

        idx_ = new_idx;
        return true;
    }

    bool endGroup()
    {
        if (type_stack_.empty() || type_stack_.back() != 'g')
            return false;

        type_stack_.pop_back();
        context_stack_.pop_back();

        idx_ = data_.getParent(idx_);
        return true;
    }

    bool readArray(const std::string& name, RequiredOrOoptional opt = OPTIONAL)
    {
        DataIndex new_idx = data_.readArray(idx_, name);
        if (new_idx.type == INVALID)
        {
            if (opt == REQUIRED)
                addError("Expected array with name '" + name + "', found none.");
            return false;
        }

        type_stack_.push_back('a');
        context_stack_.push_back(name);
        array_idx_stack_.push_back(0);

        idx_ = new_idx;
        return true;

    }

    bool endArray()
    {
        if (type_stack_.empty() || type_stack_.back() != 'a')
            return false;

        type_stack_.pop_back();
        context_stack_.pop_back();
        array_idx_stack_.pop_back();

        idx_ = data_.getParent(idx_);
        return true;
    }

    bool nextArrayItem()
    {
        if (type_stack_.empty() || type_stack_.back() != 'a')
            return false;

        unsigned int& i = array_idx_stack_.back();

        if (i > 0)
            idx_ = data_.getParent(idx_);

        const std::vector<DataIndex>& v = data_.arrays[idx_.idx];
        if (i >= v.size())
            return false;

        idx_ = v[i];
        ++i;
        return true;
    }

    void addError(const std::string& message)
    {
        if (!has_error_)
        {
            error_ << std::endl << std::endl;
        }

        error_ << "In ";

        if (type_stack_.empty())
        {
            error_ << "root";
        }
        else
        {
            error_ << "'";

            unsigned int j = 0;
            for(unsigned int i = 0; i < type_stack_.size(); ++i)
            {
                if (i > 0)
                    error_ << ".";

                error_ << context_stack_[i];

                if (type_stack_[i] == 'a')
                    error_ << "[" << array_idx_stack_[j++] - 1 << "]";
            }

            error_ << "'";
        }

        error_ << ":" << std::endl << std::endl << "    " << message << std::endl << std::endl;

        has_error_ = true;
    }

    void setErrorContext(const std::string& context)
    {
        if (context_stack_.empty())
            return;

        context_stack_.back() = context;
    }

    bool hasError() const
    {
        return has_error_;
    }

    std::string error() const
    {
        return error_.str();
    }

private:

    const Data& data_;

    DataIndex idx_;

    std::vector<char> type_stack_;

    std::vector<unsigned int> array_idx_stack_;

    std::vector<std::string> context_stack_;


    // Error

    bool has_error_;

    std::stringstream error_;

};

} // end namespace tue

} // end namespace config2

#endif
