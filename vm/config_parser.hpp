#ifndef RBX_CONFIG_PARSER
#define RBX_CONFIG_PARSER

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

namespace rubinius {
  class Configuration;

  class ConfigParser {
  public:
    class Entry {
    public:
      std::string variable;
      std::string value;

      bool is_number();
      bool is_true();
      bool in_section(std::string prefix);
      long to_i();
    };

    typedef std::map<std::string, Entry*> ConfigMap;
    typedef std::vector<Entry*> EntryList;

    ConfigMap variables;

    virtual ~ConfigParser();

    bool   load_file(std::string path);
    Entry* parse_line(const char* line);
    void   import_line(const char* line);
    void   import_many(std::string string);
    void   import_stream(std::istream&);
    Entry* find(std::string variable);
    EntryList* get_section(std::string prefix);

    void   update_configuration(Configuration&);

  private:
    bool   process_internal(std::string key, std::string val);
  };
}

#endif
