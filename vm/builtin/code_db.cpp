#include "builtin/class.hpp"
#include "builtin/code_db.hpp"
#include "builtin/compiled_code.hpp"
#include "builtin/string.hpp"

#include "object_utils.hpp"
#include "ontology.hpp"

namespace rubinius {
  void CodeDB::init(STATE) {
    GO(codedb).set(ontology::new_class_under(state, "CodeDB", G(rubinius)));
    G(codedb)->set_object_type(state, CodeDBType);
  }

  CodeDB* CodeDB::open(STATE, String* path) {
    CodeDB* codedb = create(state, path->c_str(state));

    return codedb;
  }

  CodeDB* CodeDB::create(STATE, const char* path) {
    CodeDB* codedb = state->vm()->new_object<CodeDB>(G(codedb));

    return codedb;
  }

  CompiledCode* CodeDB::load(STATE, String* m_id) {
    return nil<CompiledCode>();
  }

  Object* CodeDB::store(STATE, CompiledCode* code) {
    return cNil;
  }
}
