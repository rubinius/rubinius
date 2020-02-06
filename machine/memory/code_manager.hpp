#ifndef RBX_GC_CODE_MANAGER
#define RBX_GC_CODE_MANAGER

#include "diagnostics.hpp"

#include <stdint.h>
#include <mutex>

namespace rubinius {
namespace memory {
  class CodeResource;


  /**
   * Manages memory for code-based resources that are owned by Ruby objects,
   * such as MachineCode instances, JIT code, FFI resources etc.
   *
   * These objects are not directly accessible via Ruby code, but are used by
   * the VM to support the running of Ruby code. As such, these objects also
   * need to be garbage collected, since references to these VM internal
   * objects are only held by Ruby objects (e.g. methods); when the associated
   * Ruby object is no longer reachable, these objects can also be collected.
   *
   * Unlike the other memory managers that manage memory for Ruby objects,
   * objects managed by this class are created and destroyed using new and
   * delete.
   */

  class CodeManager {
  public:

  private:
    const static int cDefaultChunkSize = 1024 * 1024;

    /**
     * A chunk of memory used to store an array of references to CodeResource
     * instances. Chunks also maintain a next pointer, so that a linked list
     * of Chunks can be created for handling an arbitrarily large list of
     * CodeResource references.
     */

    struct Chunk {
      CodeResource** resources;
      Chunk* next;

      Chunk(int size);
      ~Chunk();
    };

    std::mutex mutex_;

    int chunk_size_;
    Chunk* first_chunk_;
    Chunk* last_chunk_;

    Chunk* current_chunk_;
    int current_index_;

    diagnostics::CodeManager* diagnostic_;

  public:
    diagnostics::CodeManager* diagnostic() {
      return diagnostic_;
    }

  public:
    CodeManager(int chunk_size=cDefaultChunkSize);
    ~CodeManager();

    void add_resource(CodeResource* cr, bool* collect_now);
    void clear_marks();
    void sweep(STATE);

  private:
    void add_chunk();
  };
}
}

#endif
