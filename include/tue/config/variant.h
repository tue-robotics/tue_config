#ifndef TUE_CONFIG_VARIANT_H_
#define TUE_CONFIG_VARIANT_H_

#include <vector>
#include <string>
#include <ostream>

namespace tue
{

class Variant
{

public:

    typedef std::vector<Variant>::iterator iterator;
    typedef std::vector<Variant>::const_iterator const_iterator;

    Variant() : type_(UNDEFINED) {}
    Variant(const char* v) : type_(STRING), string_(v) {}
    Variant(const std::string& v) : type_(STRING), string_(v) {}
    Variant(double v) : type_(REAL), real_(v) {}
    Variant(int v) : type_(INT), int_(v) {}

    ~Variant() {}

    inline bool isValid() const { return type_ != UNDEFINED; }
    inline bool isString() const { return type_ == STRING; }
    inline bool isReal() const { return type_ == REAL || type_ == INT; }
    inline bool isInt() const { return type_ == INT; }
    inline bool isArray() const { return type_ == ARRAY; }

    inline const std::string& toString() const { return string_; }
    inline double toReal() const { return (type_ == INT) ? int_ : real_; }
    inline int toInt() const { return int_; }
    inline const std::vector<Variant>& toArray() const { return array_; }

    // Array functions

    inline size_t size() const { return array_.size(); }

    inline iterator begin() { return array_.begin(); }
    inline iterator end() { return array_.end(); }

    inline const_iterator begin() const { return array_.begin(); }
    inline const_iterator end() const { return array_.end(); }

    void add(const Variant& v) { type_ = ARRAY; array_.push_back(v); }

    friend std::ostream& operator<< (std::ostream& out, const Variant& v) {
        switch (v.type_)
        {
        case UNDEFINED: out << "?"; break;
        case STRING: out << v.string_; break;
        case REAL: out << v.real_; break;
        case INT: out << v.int_; break;
        case ARRAY:
            out << "[";
            for(const_iterator it = v.begin(); it != v.end(); ++it)
                out << *it << " ";
            out << "]";
            break;
        }
        return out;
    }

private:

    enum Type
    {
        UNDEFINED,
        STRING,
        REAL,
        INT,
        ARRAY
    };

    Type type_;

    std::string string_;
    double real_;
    int int_;
    std::vector<Variant> array_;

};

}

#endif
