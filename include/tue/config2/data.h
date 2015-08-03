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
    STRING
};

// ----------------------------------------------------------------------------------------------------

struct Node
{
    Node() {}
    Node(unsigned int idx_, NodeType type_) : idx(idx_), type(type_) {}

    unsigned int idx;
    NodeType type;
};

// ----------------------------------------------------------------------------------------------------

struct Data
{
    std::vector<std::vector<Node> > arrays;
    std::vector<unsigned int> array_parents;

    std::vector<std::map<std::string, Node> > maps;
    std::vector<unsigned int> map_parents;

    std::vector<char> pods;

    void addFloat(const Node& n, const std::string& label, double v)
    {
        maps[n.idx][label] = Node(addPOD(v), FLOAT);
    }

    void addInt(const Node& n, const std::string& label, int v)
    {
        maps[n.idx][label] = Node(addPOD(v), INT);
    }

    void addString(const Node& n, const std::string& label, const std::string& v)
    {
        Node& m = maps[n.idx][label];
        m.idx = pods.size();
        pods.resize(m.idx + v.size() + 1);
        memcpy(&pods[m.idx], v.c_str(), v.size() + 1);
    }

    const char* getString(const Node& n)
    {
        return &pods[n.idx];
    }

    unsigned int getParent(const Node& n)
    {
        if (n.type == MAP)
            return map_parents[n.idx];
        else if (n.type == ARRAY)
            return array_parents[n.idx];
    }

    Node addGroup();

private:

    template<typename T>
    T& pod(unsigned int i)
    {
        return *reinterpret_cast<T*>(&pods[i]);
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
