#ifndef RBX_UTIL_CONFIGURATION_HPP
#define RBX_UTIL_CONFIGURATION_HPP

#include <list>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

namespace config {
  class ConfigItem;

  typedef std::list<ConfigItem*> Items;

  class Configuration {
    Items items_;

  public:
    void add_item(ConfigItem* item) {
      items_.push_back(item);
    }

    void import(const char* key, const char* val);
    void print();
  };

  class ConfigItem {
  public:
    const char* name_;
    bool set_;

    ConfigItem(Configuration* config, const char* name)
      : name_(name)
    {
      config->add_item(this);
    }

    virtual ~ConfigItem() {}

    virtual void set(const char* str) = 0;
    virtual void print_value() = 0;

    bool set_maybe(const char* key, const char* val) {
      if(strcmp(name_, key) != 0) return false;

      set_ = true;
      set(val);
      return true;
    }

    const char* name() {
      return name_;
    }

    bool set_p() {
      return set_;
    }
  };

  class Integer : public ConfigItem {
  public:
    long value;

    Integer(Configuration* config, const char* name, int def = 0)
      : ConfigItem(config, name)
      , value(def)
    {}

    virtual void set(const char* str) {
      // true means either likely they set it with no value
      if(strcmp("true", str) == 0) {
        value = 1;
      } else {
        value = strtol(str, NULL, 0);
      }
    }

    virtual void print_value() {
      std::cout << value;
    }

    operator long() {
      return value;
    }
  };

  class String : public ConfigItem {
  public:
    std::string value;

    String(Configuration* config, const char* name)
      : ConfigItem(config, name)
      , value("")
    {}

    virtual void set(const char* str) {
      value = std::string(str);
    }

    virtual void print_value() {
      std::cout << value;
    }

    operator const char*() {
      return value.c_str();
    }
  };

  class Bool : public ConfigItem {
  public:
    bool value;

    Bool(Configuration* config, const char* name)
      : ConfigItem(config, name)
      , value(false)
    {}

    static bool convert(const char* str) {
      if(*str == '0'           ||
         !strcmp(str, "no")    ||
         !strcmp(str, "NO")    ||
         !strcmp(str, "false") ||
         !strcmp(str, "FALSE")) return false;

      return true;
    }

    virtual void set(const char* str) {
      value = convert(str);
    }

    virtual void print_value() {
      std::cout << (value ? "true" : "false");
    }

    operator bool() {
      return value;
    }
  };

  inline void Configuration::import(const char* key, const char* val) {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        i++) {
      ConfigItem* item = *i;
      if(item->set_maybe(key, val)) return;
    }
  }

  inline void Configuration::print() {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        i++) {
      ConfigItem* item = *i;
      std::cout << item->name() << ": ";
      item->print_value();
      std::cout << "\n";
    }
  }
}

#endif
