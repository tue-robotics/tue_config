#ifndef TUE_CONFIG2_DATA_H_
#define TUE_CONFIG2_DATA_H_

#include <vector>
#include <map>
#include <string>

#include <cstring> // memcpy

namespace tue
{
namespace config2
{

// ----------------------------------------------------------------------------------------------------

enum NodeType
{
    ARRAY,
    MAP,
    INT,
    FLOAT,
    STRING,
    INVALID
};

// ----------------------------------------------------------------------------------------------------

struct DataIndex
{
    DataIndex() {}
    DataIndex(unsigned int idx_, NodeType type_) : idx(idx_), type(type_) {}

    unsigned int idx;
    NodeType type;
};

// ----------------------------------------------------------------------------------------------------

struct Data
{
    Data()
    {
        // Add the root map
        maps.push_back(std::map<std::string, DataIndex>());
        map_parents.push_back(DataIndex(0, INVALID));
    }

    std::vector<std::vector<DataIndex> > arrays;
    std::vector<DataIndex> array_parents;

    std::vector<std::map<std::string, DataIndex> > maps;
    std::vector<DataIndex> map_parents;

    std::vector<char> pods;

    void addFloat(const DataIndex& n, const std::string& label, double v)
    {
        maps[n.idx][label] = DataIndex(addPOD(v), FLOAT);
    }

    void addInt(const DataIndex& n, const std::string& label, int v)
    {
        maps[n.idx][label] = DataIndex(addPOD(v), INT);
    }

    void addString(const DataIndex& n, const std::string& label, const std::string& v)
    {
        DataIndex& m = maps[n.idx][label];
        m.idx = pods.size();
        m.type = STRING;
        pods.resize(m.idx + v.size() + 1);
        memcpy(&pods[m.idx], v.c_str(), v.size() + 1);
    }

    const char* getString(const DataIndex& n) const
    {
        return &pods[n.idx];
    }

    bool getString(const DataIndex &n, const std::string& key, std::string& v) const
    {
        const std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::const_iterator it = current_map.find(key);
        if (it == current_map.end())
            return false;

        const DataIndex& m = it->second;
        if (m.type != STRING)
            return false;

        v = getString(m);
        return true;
    }

    double getFloat(const DataIndex& n) const
    {
        return pod<double>(n.idx);
    }

    bool getFloat(const DataIndex &n, const std::string& key, double& v) const
    {
        const std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::const_iterator it = current_map.find(key);
        if (it == current_map.end())
            return false;

        const DataIndex& m = it->second;
        if (m.type == FLOAT)
            v = pod<double>(m.idx);
        else if (m.type == INT)
            v = pod<int>(m.idx);
        else
            return false;

        return true;
    }

    int getInt(const DataIndex& n) const
    {
        return pod<int>(n.idx);
    }

    bool getInt(const DataIndex &n, const std::string& key, int& v) const
    {
        const std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::const_iterator it = current_map.find(key);
        if (it == current_map.end())
            return false;

        const DataIndex& m = it->second;
        if (m.type != INT)
            return false;

        v = pod<int>(m.idx);
        return true;
    }

    DataIndex readGroup(const DataIndex& n, const std::string& name) const
    {
        const std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::const_iterator it = current_map.find(name);

        if (it == current_map.end() || it->second.type != MAP)
            return DataIndex(0, INVALID);

        return it->second;
    }

    DataIndex addGroup(const DataIndex& n, const std::string& name)
    {
        std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::iterator it = current_map.find(name);

        if (it != current_map.end())
        {
            // Group already exists
            DataIndex& m = it->second;
            if (m.type == MAP)
                return m;
        }

        // Group does not yet exist
        DataIndex& m = current_map[name];
        m.idx = maps.size();
        m.type = MAP;
        maps.push_back(std::map<std::string, DataIndex>());
        map_parents.push_back(n);
        return m;
    }

    DataIndex readArray(const DataIndex& n, const std::string& name) const
    {
        const std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::const_iterator it = current_map.find(name);

        if (it == current_map.end() || it->second.type != ARRAY)
            return DataIndex(0, INVALID);

        return it->second;
    }

    DataIndex addArray(const DataIndex& n, const std::string& name)
    {
        std::map<std::string, DataIndex>& current_map = maps[n.idx];
        std::map<std::string, DataIndex>::iterator it = current_map.find(name);

        if (it != current_map.end())
        {
            // Array already exists
            DataIndex& m = it->second;
            if (m.type == ARRAY)
                return m;
        }

        // Array does not yet exist
        DataIndex& m = current_map[name];
        m.idx = arrays.size();
        m.type = ARRAY;
        arrays.push_back(std::vector<DataIndex>());
        array_parents.push_back(n);
        return m;
    }

    DataIndex addArrayItem(const DataIndex& n)
    {
        std::vector<DataIndex>& array = arrays[n.idx];

        // Construct new map
        DataIndex m(maps.size(), MAP);
        maps.push_back(std::map<std::string, DataIndex>());
        map_parents.push_back(n);

        // Push index to the map in the array
        array.push_back(m);

        return m;
    }

    DataIndex getParent(const DataIndex& n) const
    {
        if (n.type == MAP)
            return map_parents[n.idx];
        else if (n.type == ARRAY)
            return array_parents[n.idx];
    }

    bool empty() const { return maps[0].empty(); }

private:

    template<typename T>
    T& pod(unsigned int i)
    {
        return *reinterpret_cast<T*>(&pods[i]);
    }

    template<typename T>
    const T& pod(unsigned int i) const
    {
        return *reinterpret_cast<const T*>(&pods[i]);
    }

    template<typename T>
    unsigned int addPOD(const T& v)
    {
        unsigned int idx = pods.size();
        pods.resize(idx + sizeof(T));
        pod<T>(idx) = v;
        return idx;
    }

};


} // end namespace tue

} // end namespace config2

#endif
