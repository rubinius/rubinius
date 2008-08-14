#include <iostream>
#include <sstream>
#include <map>
#include <vector>

namespace rubinius {
  class ConfigParser {
  public:
    class Entry {
    public:
      std::string variable;
      std::string value;

      bool is_number();
      bool in_section(std::string prefix);
    };

    typedef std::map<std::string, Entry*> ConfigMap;
    typedef std::vector<Entry*> EntryList;

    ConfigMap variables;

    virtual ~ConfigParser();

    Entry* parse_line(const char* line);
    void   import_stream(std::istream&);
    Entry* find(std::string variable);
    EntryList* get_section(std::string prefix);
  };
}
