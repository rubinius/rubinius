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

    size_t items_size() const {
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
      , set_(false)
    {
      config->add_item(this);
    }

    virtual ~ConfigItem() {}

    virtual void set(const char* str) = 0;
    virtual void print_value(std::ostream& stream) = 0;

    virtual bool set_maybe(const char* key, const char* val) {
      if(strcmp(name_, key) != 0) return false;

      set(val);
      return true;
    }

    virtual bool integer_p() {
      return false;
    }

    const char* name() const {
      return name_;
    }

    bool set_p() const {
      return set_;
    }

    const char* description() const {
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
      set_ = true;
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

    virtual bool integer_p() {
      return true;
    }

    operator long() const {
      return value;
    }
  };

  class Bytes : public ConfigItem {
  public:
    long value;

    Bytes(Configuration* config, const char* name, int def = 0)
      : ConfigItem(config, name)
      , value(def)
    {}

    virtual void set(const char* str) {
      char* end;
      value = strtol(str, &end, 0);
      switch(*end) {
      case 'g':
      case 'G':
        value *= 1073741824;
        break;
      case 'm':
      case 'M':
        value *= 1048576;
        break;
      case 'k':
      case 'K':
        value *= 1024;
        break;
      }
    }

    virtual void print_value(std::ostream& stream) {
      stream << value;
    }

    operator long() const {
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
      set_ = true;
      value = std::string(str);
    }

    virtual void print_value(std::ostream& stream) {
      if(value.empty()) {
        stream << "\"\"";
      } else {
        stream << value;
      }
    }

    operator const char*() const {
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
      set_ = true;
      value = convert(str);
    }

    virtual void print_value(std::ostream& stream) {
      stream << (value ? "true" : "false");
    }

    operator bool() const {
      return value;
    }
  };

  class Radio : public ConfigItem {
  public:
    int value;
    int which;

  private:
    std::vector<const char*> names_;
    std::vector<int> values_;

  public:
    Radio(Configuration* config, const char* name)
      : ConfigItem(config, name)
    {
      value = 0;
      which = -1;
    }

    int add(const char* name, int v, bool def=false) {
      int w = names_.size();
      names_.push_back(name);
      values_.push_back(v);

      if(def) {
        value = v;
        which = w;
      }

      return w;
    }

    virtual bool set_maybe(const char* key, const char* val) {
      for(size_t i = 0; i < names_.size(); i++) {
        if(strcmp(names_[i], key) == 0) {
          which = i;
          value = values_[i];
          return true;
        }
      }

      return ConfigItem::set_maybe(key, val);
    }

    virtual void set(const char* str) {
      for(size_t i = 0; i < names_.size(); i++) {
        if(strcmp(names_[i], str) == 0) {
          which = i;
          value = values_[i];
          return;
        }
      }

      which = -1;
      value = 0;
    }

    virtual void print_value(std::ostream& stream) {
      if(which < 0) {
        stream << "<no value>";
      } else {
        stream << names_[which];
      }

      stream << " (possible:";
      for(size_t i = 0; i < names_.size(); i++) {
        stream << " " << names_[i];
      }

      stream << ")";
    }

    operator int() const {
      return value;
    }
  };

  class BoolSet : public Bool {
    std::vector<Bool*> sub_bools_;

  public:
    BoolSet(Configuration* config, const char* name)
      : Bool(config, name)
    {}

    virtual void set(const char* str) {
      set_ = true;
      Bool::set(str);

      for(std::vector<Bool*>::iterator i = sub_bools_.begin();
          i != sub_bools_.end();
          ++i) {
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
        ++i) {
      ConfigItem* item = *i;
      if(item->set_maybe(key, val)) return true;
    }

    return false;
  }

  inline ConfigItem* Configuration::find(const char* key) {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        ++i) {
      ConfigItem* item = *i;
      if(item->equal_p(key)) return item;
    }

    return 0;
  }

  inline void Configuration::print(bool desc) {
    for(Items::iterator i = items_.begin();
        i != items_.end();
        ++i) {
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
