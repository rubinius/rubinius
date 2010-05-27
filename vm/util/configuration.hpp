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

    bool import(const char* key, const char* val);
    void print(bool desc=false);
    ConfigItem* find(const char* key);

    size_t items_size() {
      return items_.size();
    }

    Items::iterator items_begin() {
      return items_.begin();
    }

    Items::iterator items_end() {
      return items_.end();
    }
  };

  class ConfigItem {
  public:
    const char* name_;
    const char* description_;
    bool set_;

    ConfigItem(Configuration* config, const char* name)
      : name_(name)
      , description_(0)
    {
      config->add_item(this);
    }

    virtual ~ConfigItem() {}

    virtual void set(const char* str) = 0;
    virtual void print_value(std::ostream& stream) = 0;

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

    const char* description() {
      return description_;
    }

    void set_description(const char* str) {
      description_ = str;
    }

    bool equal_p(const char* key) {
      return strcmp(name_, key) == 0;
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

    virtual void print_value(std::ostream& stream) {
      stream << value;
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

    virtual void print_value(std::ostream& stream) {
      if(value.size() == 0) {
        stream << "\"\"";
      } else {
        stream << value;
      }
    }

    operator const char*() {
      return value.c_str();
    }
  };

  class Bool : public ConfigItem {
  public:
    bool value;

    Bool(Configuration* config, const char* name, bool def=false)
      : ConfigItem(config, name)
      , value(def)
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

    virtual void print_value(std::ostream& stream) {
      stream << (value ? "true" : "false");
    }

    operator bool() {
      return value;
    }
  };

  class BoolSet : public Bool {
    std::vector<Bool*> sub_bools_;

  public:
    bool value;

    BoolSet(Configuration* config, const char* name)
      : Bool(config, name)
    {}

    virtual void set(const char* str) {
      Bool::set(str);

      for(std::vector<Bool*>::iterator i = sub_bools_.begin();
          i != sub_bools_.end();
          i++) {
        (*i)->set(str);
      }
    }

    void add(Bool& sub) {
      sub_bools_.push_back(&sub);
    }
  };

  inline bool Configuration::import(const char* key, const char* val) {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        i++) {
      ConfigItem* item = *i;
      if(item->set_maybe(key, val)) return true;
    }

    return false;
  }

  inline ConfigItem* Configuration::find(const char* key) {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        i++) {
      ConfigItem* item = *i;
      if(item->equal_p(key)) return item;
    }

    return 0;
  }

  inline void Configuration::print(bool desc) {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        i++) {
      ConfigItem* item = *i;
      std::cout << item->name() << ": ";
      item->print_value(std::cout);
      std::cout << "\n";
      if(desc) {
        if(const char* desc = item->description()) {
          std::cout << "  " << desc << "\n";
        }
        std::cout << "\n";
      }
    }
  }
}

#endif
