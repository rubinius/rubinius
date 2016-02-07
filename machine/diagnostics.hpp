#ifndef RBX_DIAGNOSTICS_HPP
#define RBX_DIAGNOSTICS_HPP

#include <stdint.h>

namespace rubinius {
  namespace diagnostics {
    class Diagnostics {
      bool modified_;

    public:
      Diagnostics() : modified_(false) { }
      virtual ~Diagnostics() { }

      virtual void log() {
        modified_ = false;
      }

      bool modified_p() {
        return modified_;
      }

      void modify() {
        modified_ = true;
      }
    };

    class MemoryDiagnostics : public Diagnostics {
    public:
      int64_t objects_;
      int64_t bytes_;

      MemoryDiagnostics()
        : Diagnostics()
        , objects_(0)
        , bytes_(0)
      { }
    };
  }
}

#endif
