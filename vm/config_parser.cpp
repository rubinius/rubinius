#include <cstring>
#include <cstdlib>
#include <string>

#include "config_parser.hpp"
#include "configuration.hpp"

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

    for(i = strlen(str); i-- && !isalnum(str[i]) && str[i] != '/';) {
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


  void ConfigParser::process_argv(int argc, char** argv) {
    for(int i=1; i < argc; i++) {
      char* arg = argv[i];
      if(strncmp(arg, "-X", 2) == 0) {
        import_line(arg + 2);
        continue;
      }

      if(arg[0] != '-' || strcmp(arg, "--") == 0) return;
    }
  }

  ConfigParser::Entry* ConfigParser::parse_line(const char* line) {
    char* var = strdup(line);
    char* equals = strchr(var, '=');

    Entry* entry = new ConfigParser::Entry();

    if(equals) {
      /* Split the string. */
      *equals++ = 0;
    }

    entry->variable = std::string(trim_str(var));

    // Just the variable name means true, as in enable
    if(equals) {
      entry->value = std::string(trim_str(equals));
    } else {
      entry->value = std::string("true");
    }

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
           i->second->in_section("jit.")) {
          std::cout << "Warning: Unrecognized VM option '" << i->first << "'\n";
        }
      }
    }
  }
}
