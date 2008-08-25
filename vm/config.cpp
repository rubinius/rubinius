#include <cstring>
#include <cstdlib>
#include <string>
#include "config.hpp"

namespace rubinius {
  /* utility: checks whether string contains only digits */
  static bool is_number(const char *str) {
    while(*str) {
      if(!isdigit(*str)) return false;
      str++;
    }

    return true;
  }


  /* utility: strips trailing non-alnum chars from string */
  static char *trim_str(char *str) {
    int i;
    while(*str && !isalnum(*str) && *str != '/') str++;

    for(i = strlen(str) - 1; str[i] && !isalnum(str[i]) && *str != '/'; i++) {
      str[i] = 0;
    }

    return str;
  }

  ConfigParser::Entry* ConfigParser::parse_line(const char* line) {
    char* var = strdup(line);
    char* equals = strstr(var, "=");

    if(!equals) {
      free(var);
      return NULL;
    }

    /* Split the string. */
    *equals++ = 0;

    Entry* entry = new ConfigParser::Entry();

    entry->variable = std::string(trim_str(var));
    entry->value =    std::string(trim_str(equals));

    free(var);

    return entry;
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

  ConfigParser::Entry* ConfigParser::find(std::string name) {
    ConfigParser::ConfigMap::iterator i = variables.find(name);

    if(i == variables.end()) return NULL;

    return i->second;
  }

  ConfigParser::~ConfigParser() {
    ConfigParser::ConfigMap::iterator i = variables.begin();
    while(i != variables.end()) {
      delete i->second;
      i++;
    }
  }

  bool ConfigParser::Entry::is_number() {
    return rubinius::is_number(value.c_str());
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
}
