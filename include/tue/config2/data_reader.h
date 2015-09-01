#ifndef _DATA_READER_H_
#define _DATA_READER_H_

#include "tue/config2/reader.h"
#include "tue/config2/data.h"

namespace tue
{
namespace config2
{

class DataReader : public Reader
{

public:

    DataReader(const Data& data) : data_(data), idx_(0, MAP) {}

    ~DataReader() {}

    bool readInt(const std::string& key, int& v)
    {
        return data_.getInt(idx_, key, v);
    }

    bool readFloat(const std::string& key, double& v)
    {
        return data_.getFloat(idx_, key, v);
    }

    bool readString(const std::string& key, std::string& v)
    {
        return data_.getString(idx_, key, v);
    }

    bool readGroup(const std::string& name)
    {
        DataIndex new_idx = data_.readGroup(idx_, name);
        if (new_idx.type == INVALID)
            return false;

        type_stack_.push_back('g');

        idx_ = new_idx;
        return true;
    }

    bool endGroup()
    {
        if (type_stack_.empty() || type_stack_.back() != 'g')
            return false;

        type_stack_.pop_back();

        idx_ = data_.getParent(idx_);
        return true;
    }

    bool readArray(const std::string& name)
    {
        DataIndex new_idx = data_.readArray(idx_, name);
        if (new_idx.type == INVALID)
            return false;

        type_stack_.push_back('a');
        array_idx_stack_.push_back(0);

        idx_ = new_idx;
        return true;

    }

    bool endArray()
    {
        if (type_stack_.empty() || type_stack_.back() != 'a')
            return false;

        type_stack_.pop_back();
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

private:

    const Data& data_;

    DataIndex idx_;

    std::vector<char> type_stack_;

    std::vector<unsigned int> array_idx_stack_;

};

} // end namespace tue

} // end namespace config2

#endif
