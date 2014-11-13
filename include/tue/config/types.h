#ifndef TUE_CONFIG2_TYPES_H_
#define TUE_CONFIG2_TYPES_H_

#include <boost/shared_ptr.hpp>

namespace tue
{

namespace config
{

enum RequiredOrOoptional
{
    REQUIRED,
    OPTIONAL
};

class Node;
typedef boost::shared_ptr<Node> NodePtr;
typedef boost::shared_ptr<const Node> NodeConstPtr;

class Data;
typedef boost::shared_ptr<Data> DataPtr;
typedef boost::shared_ptr<const Data> DataConstPtr;

typedef unsigned int NodeIdx;

//class Variant
//{
//public:

//    Variant() {}
//    Variant(int i) : i_(i) {}

//    template<typename T>
//    bool get(T& t) { t = i_; }

//    friend std::ostream& operator<< (std::ostream& out, const Variant& c)
//    {
//        out << c.i_;
//        return out;
//    }

//private:

//    int i_;
//};

}

} // end namespace tue

#endif
