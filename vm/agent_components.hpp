#ifndef RBX_WINDOWS
#include <sys/utsname.h>
#endif

#include "vm.hpp"
#include "configuration.hpp"
#include "call_frame.hpp"
#include "util/bert.hpp"

namespace rubinius {
namespace agent {

  class Tree;

  class Output {
    bert::Encoder<bert::IOWriter> encoder_;

  public:
    Output(bert::IOWriter& writer_)
      : encoder_(writer_)
    {}

    bert::Encoder<bert::IOWriter>& e() {
      return encoder_;
    }

    void error(const char* reason) {
      e().write_tuple(2);
      e().write_atom("error");
      e().write_binary(reason);
    }

    void ok(const char* kind) {
      e().write_tuple(3);
      e().write_atom("ok");
      e().write_atom(kind);
    }
  };

  class VariableAccess {
    Tree* root_;
    Tree* system_;

  public:
    VariableAccess(STATE);
    ~VariableAccess();

    Tree* root() {
      return root_;
    }

    bool read_path(Output& output, const char* ipath);
    bool set_path(Output& output, const char* ipath, bert::Value* val);
  };
}}
