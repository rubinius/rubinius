#include <cstring>
#include <cstdlib>
#include <string>
#include <fstream>

#include "config_parser.hpp"
#include "configuration.hpp"

namespace rubinius {
  /* utility: checks whether string contains only digits */
  static bool is_number(const char *str) {
    if(*str == '-') str++;

    while(*str) {
      if(!isdigit(*str)) return false;
      str++;
    }

    return true;
  }

  static bool valid_char(char c) {
    return isalnum(c) || c == '-';
  }


  /* utility: strips trailing non-alnum chars from string */
  static char *trim_str(char *str) {
    int i;
    while(*str && !valid_char(*str) && *str != '/') str++;

    for(i = strlen(str); i-- && !valid_char(str[i]) && str[i] != '/';) {
      str[i] = 0;
    }

    return str;
  }

  ConfigParser::~ConfigParser() {
    ConfigParser::ConfigMap::iterator i = variables.begin();
    while(i != variables.end()) {
      delete i->second;
      i++;
    }
  }

  bool ConfigParser::load_file(std::string path) {
    std::ifstream stream(path.c_str());
    if(!stream) return false;

    import_stream(stream);

    return true;
  }

  bool ConfigParser::process_internal(std::string key, std::string val) {
    if(key == "include") {
      if(!load_file(val)) {
        std::cerr << "WARNING: Unable to include '" << val << "'.\n";
      }
    } else if(key == "include_maybe") {
      load_file(val);
    } else {
      return false;
    }

    return true;
  }

  ConfigParser::Entry* ConfigParser::parse_line(const char* line) {
    char* var = strdup(line);
    char* equals = strchr(var, '=');

    if(equals) {
      /* Split the string. */
      *equals++ = 0;
    }

    std::string variable = std::string(trim_str(var));

    std::string value;
    // Just the variable name means true, as in enable
    if(equals) {
      value = std::string(trim_str(equals));
    } else {
      value = std::string("true");
    }

    if(process_internal(variable, value)) {
      free(var);
      return NULL;
    }

    Entry* entry = new ConfigParser::Entry();
    entry->variable = variable;
    entry->value = value;

    free(var);

    return entry;
  }

  void ConfigParser::import_line(const char* line) {
    ConfigParser::Entry* entry = parse_line(line);
    if(entry) {
      variables[entry->variable] = entry;
    }
  }

  void ConfigParser::import_stream(std::istream& stream) {
    while(!stream.eof()) {
      std::string line;
      getline(stream, line);
      ConfigParser::Entry* entry = parse_line(line.c_str());
      if(entry) {
        variables[entry->variable] = entry;
      }
    }
  }

  void ConfigParser::import_many(std::string string) {
    size_t start = 0;
    size_t end = 0;
    while((end = string.find(";", start)) != std::string::npos) {
      std::string sub = string.substr(start, end-start);
      import_line(sub.c_str());
      start = end + 1;
    }

    std::string sub = string.substr(start);
    import_line(sub.c_str());
  }

  ConfigParser::Entry* ConfigParser::find(std::string name) {
    ConfigParser::ConfigMap::iterator i = variables.find(name);

    if(i == variables.end()) return NULL;

    return i->second;
  }

  bool ConfigParser::Entry::is_number() {
    return rubinius::is_number(value.c_str());
  }

  bool ConfigParser::Entry::is_true() {
    return value == "true";
  }

  long ConfigParser::Entry::to_i() {
    return strtol(value.c_str(), NULL, 10);
  }

  bool ConfigParser::Entry::in_section(std::string prefix) {
    return variable.find(prefix) == 0;
  }

  ConfigParser::EntryList* ConfigParser::get_section(std::string prefix) {
    ConfigParser::EntryList* list = new ConfigParser::EntryList(0);

    ConfigParser::ConfigMap::iterator i = variables.begin();
    while(i != variables.end()) {
      if(i->second->in_section(prefix)) {
        list->push_back(i->second);
      }
      i++;
    }

    return list;
  }

  void ConfigParser::update_configuration(Configuration& config) {
    for(ConfigParser::ConfigMap::iterator i = variables.begin();
        i != variables.end();
        i++) {
      if(!config.import(i->first.c_str(), i->second->value.c_str())) {
        if(i->second->in_section("vm.") ||
           i->second->in_section("jit.") ||
           i->second->in_section("gc.")) {
          std::cout << "Warning: Unrecognized VM option '" << i->first << "'\n";
        }
      }
    }
  }
}
