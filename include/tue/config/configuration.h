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

enum RequiredOrOoptional
{
    REQUIRED,
    OPTIONAL
};

class Configuration
{

public:

    Configuration();

    ~Configuration();


    void setValue(const std::string& key, const Variant& value);

    Variant value(const std::string& key, RequiredOrOoptional opt = REQUIRED);

    bool value(const std::string &key, float& f, RequiredOrOoptional opt = REQUIRED);

    bool value(const std::string &key, double& d, RequiredOrOoptional opt = REQUIRED);

    bool value(const std::string &key, int& i, RequiredOrOoptional opt = REQUIRED);

    bool value(const std::string &key, bool& b, RequiredOrOoptional opt = REQUIRED);

    bool value(const std::string &key, std::string& s, RequiredOrOoptional opt = REQUIRED);

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

    void addError(const std::string& msg);


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

    bool checkValue(const std::string& key, Variant& v, RequiredOrOoptional opt = REQUIRED);

    void print(std::ostream& out, const ConfigSet& cs, const std::string& indent = "") const;

};

}

#endif
