#ifndef RBX_DIAGNOSTICS_EMITTER_HPP
#define RBX_DIAGNOSTICS_EMITTER_HPP

namespace rubinius {
  namespace diagnostics {
    class Emitter {
    public:
      Emitter() { }

      static Emitter* create(STATE);

      virtual void transmit(Diagnostic*) { }
    };

    class FileEmitter : public Emitter {
      std::string path_;
      int fd_;

    public:
      FileEmitter(STATE, std::string path);

      virtual void transmit(Diagnostic* data);
    };
  }
}

#endif
