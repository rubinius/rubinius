#ifndef RBX_CONFIG_PARSER
#define RBX_CONFIG_PARSER

#include <string>
#include <iostream>
#include <vector>

namespace rubinius {
  class Configuration;

  class ConfigParser {
  public:
    class Entry {
    public:
      std::string variable;
      std::string value;

      bool is_number() const;
      bool is_true() const;
      bool in_section(std::string prefix) const;
      long to_i() const;
    };

    typedef std::vector< std::pair<std::string, Entry*> > ConfigVector;
    typedef std::vector<Entry*> EntryList;

    ConfigVector variables;

    virtual ~ConfigParser();

    bool   load_file(std::string path);
    Entry* parse_line(const char* line);
    void   import_line(const char* line);
    void   set(const char* name, const char* val);
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
