#ifndef RBX_DIAGNOSTICS_FORMATTER_HPP
#define RBX_DIAGNOSTICS_FORMATTER_HPP

#include <rapidjson/document.h>

#include <string>

namespace rubinius {
  namespace diagnostics {
    class Formatter {
    protected:
      rapidjson::Document document_;

    public:
      Formatter()
        : document_()
      {
        document_.SetObject();
        document_.AddMember("type", "Diagnostics", document_.GetAllocator());
      }

      virtual ~Formatter() {}

      virtual void set_type(const char* type) {
        document_["type"].SetString(type, document_.GetAllocator());
      }

      virtual void update() {}
      virtual std::string to_string();
    };

    class ImmixFormatter : public Formatter {
      ImmixDiagnostics* diagnostics_;

    public:
      ImmixFormatter(ImmixDiagnostics* d);
      ~ImmixFormatter() {}

      virtual void update();
      virtual std::string to_string();
    };

    class SymbolFormatter : public Formatter {
      SymbolDiagnostics* diagnostics_;

    public:
      SymbolFormatter(SymbolDiagnostics* d);
      ~SymbolFormatter() {}

      virtual void update();
    };
  }
}

#endif
