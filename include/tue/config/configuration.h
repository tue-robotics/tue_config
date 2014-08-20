#ifndef TUE_CONFIG_CONFIGURATION_H_
#define TUE_CONFIG_CONFIGURATION_H_

#include <string>

#include "tue/config/variant.h"

#include <boost/shared_ptr.hpp>

// Sync
#include <ctime>

class ConfigData;
class ConfigSet;

namespace tue
{

class Configuration
{

public:

    Configuration();

    ~Configuration();


    void setValue(const std::string& key, const Variant& value);

    Variant value(const std::string& key);

    bool value(const std::string &key, float& f);

    bool value(const std::string &key, double& d);

    bool value(const std::string &key, int& i);

    bool value(const std::string &key, bool& b);

    bool value(const std::string &key, std::string& s);

    void remove(const std::string& key);


    // Group operations

    void writeGroup(const std::string& group);

    bool readGroup(const std::string& group);

    void endGroup();


    // Array operations

    void writeArray(const std::string& array);

    bool readArray(const std::string& array);

    void endArray();

    bool nextArrayItem();

    void addArrayItem();

    void endArrayItem();


    // Scope operations

    Configuration limitScope() const;


    // Error handling

    bool hasError() const;

    const std::string& error() const;


    // Syncing and Loading

    bool sync();

    bool loadFromYAMLFile(const std::string& filename);

    friend std::ostream& operator<< (std::ostream& out, const Configuration& c);

private:

    boost::shared_ptr<ConfigData> data_;

    ConfigSet* head_;

    ConfigSet* scope_;

    std::string filename_;

    std::time_t source_last_write_time_;

    Configuration(const boost::shared_ptr<ConfigData>& data, ConfigSet* head, ConfigSet* scope);

    bool checkValue(const std::string& key, Variant& v);

    void addError(const std::string& msg);

    void print(std::ostream& out, const ConfigSet& cs, const std::string& indent = "") const;

};

}

#endif
