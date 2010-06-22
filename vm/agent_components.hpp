#include <sys/utsname.h>
#include "vm.hpp"
#include "configuration.hpp"
#include "call_frame.hpp"
#include "util/bert.hpp"

namespace rubinius {
namespace agent {

  class Tree;

  class Output {
    bert::IOWriter& writer_;
    bert::Encoder<bert::IOWriter> encoder_;

  public:
    Output(bert::IOWriter& writer_)
      : writer_(writer_)
      , encoder_(writer_)
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
    VariableAccess(STATE, SharedState& ss);

    Tree* root() {
      return root_;
    }

    void read_path(Output& output, const char* ipath);
    void set_path(Output& output, const char* ipath, bert::Value* val);
  };
}}
