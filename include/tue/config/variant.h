#ifndef TUE_CONFIG2_VARIANT_H_
#define TUE_CONFIG2_VARIANT_H_

// Directly taken from http://stackoverflow.com/questions/5319216/implementing-a-variant-class

#include <boost/shared_ptr.hpp>

namespace tue
{
namespace config
{

//template <typename T>
//struct TypeWrapper
//{
//    typedef T TYPE;
//    typedef const T CONSTTYPE;
//    typedef T& REFTYPE;
//    typedef const T& CONSTREFTYPE;
//};

//template <typename T>
//struct TypeWrapper<const T>
//{
//    typedef T TYPE;
//    typedef const T CONSTTYPE;
//    typedef T& REFTYPE;
//    typedef const T& CONSTREFTYPE;
//};

//template <typename T>
//struct TypeWrapper<const T&>
//{
//    typedef T TYPE;
//    typedef const T CONSTTYPE;
//    typedef T& REFTYPE;
//    typedef const T& CONSTREFTYPE;
//};

//template <typename T>
//struct TypeWrapper<T&>
//{
//    typedef T TYPE;
//    typedef const T CONSTTYPE;
//    typedef T& REFTYPE;
//    typedef const T& CONSTREFTYPE;
//};

//class Variant2
//{
//public:
//    Variant2() { }

//    template<class T>
//    Variant2(T inValue) :
//        mImpl(new VariantImpl<typename TypeWrapper<T>::TYPE>(inValue))
//    {
//    }

//    template<class T>
//    typename TypeWrapper<T>::REFTYPE getValue()
//    {
//        return dynamic_cast<VariantImpl<typename TypeWrapper<T>::TYPE>&>(*mImpl.get()).mValue;
//    }

//    template<class T>
//    typename TypeWrapper<T>::CONSTREFTYPE getValue() const
//    {
//        return dynamic_cast<VariantImpl<typename TypeWrapper<T>::TYPE>&>(*mImpl.get()).mValue;
//    }

//    template<class T>
//    void setValue(typename TypeWrapper<T>::CONSTREFTYPE inValue)
//    {
//        mImpl.reset(new VariantImpl<typename TypeWrapper<T>::TYPE>(inValue));
//    }

//    friend std::ostream& operator<< (std::ostream& out, const Variant2& v)
//    {
//        v.mImpl->print(out);
//        return out;
//    }


//private:
//    struct AbstractVariantImpl
//    {
//        virtual ~AbstractVariantImpl() {}

//        virtual void print(std::ostream& out) const {}
//    };

//    template<class T>
//    struct VariantImpl : public AbstractVariantImpl
//    {
//        VariantImpl(T inValue) : mValue(inValue) { }

//        ~VariantImpl() {}

//        T mValue;

//        void print(std::ostream& out) const { out << mValue; }
//    };

//    boost::shared_ptr<AbstractVariantImpl> mImpl;
//};

class Variant
{

public:

    Variant() : type_('?') {}

    Variant(const double& d) : type_('d'), d_(d) {}
    Variant(int i) : type_('i'), i_(i) {}
    Variant(const std::string& s) : type_('s'), s_(s) {}
    Variant(const char* s) : type_('s'), s_(s) {}

    bool getValue(int& v) { return checkAndGet(i_, 'i', v); }
    bool getValue(double& v) { return checkAndGet(d_, 'd', v) || checkAndGet((double)i_, 'i', v); }
    bool getValue(float& v) { return checkAndGet((float)d_, 'd', v) || checkAndGet((float)i_, 'i', v); }
    bool getValue(std::string& v) { return checkAndGet(s_, 's', v); }

private:

    char type_;

    union {
        int i_;
        double d_;
    };

    std::string s_;

    template<typename T>
    inline bool checkAndGet(const T& v, char type, T& out)
    {
        if (type != type_)
            return false;
        out = v;
        return true;
    }

    friend std::ostream& operator<< (std::ostream& out, const Variant& v)
    {
        switch (v.type_)
        {
        case 'i': out << v.i_;
            break;
        case 'd': out << v.d_;
            break;
        case 's': out << v.s_;
            break;
        default: out << "?";
            break;
        }

        return out;
    }

};

} // end namespace tue

} // end namespace config

#endif
