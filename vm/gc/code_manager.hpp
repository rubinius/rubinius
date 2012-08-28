#ifndef RBX_GC_CODE_MANAGER
#define RBX_GC_CODE_MANAGER

#include "util/thread.hpp"

namespace rubinius {
  class CodeResource;
  class SharedState;


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
    const static int cDefaultChunkSize = 64;

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

    utilities::thread::Mutex mutex_;

    SharedState* shared_;

    int chunk_size_;
    Chunk* first_chunk_;
    Chunk* last_chunk_;

    Chunk* current_chunk_;
    int current_index_;

    int freed_resources_;
    int total_allocated_;
    int total_freed_;

    size_t bytes_used_;

  public:
    int freed_resources() {
      return freed_resources_;
    }

    int total_allocated() {
      return total_allocated_;
    }

    int total_freed() {
      return total_freed_;
    }

    SharedState* shared() {
      return shared_;
    }

    size_t& size() {
      return bytes_used_;
    }

  public:
    CodeManager(SharedState* shared, int chunk_size=cDefaultChunkSize);
    ~CodeManager();

    void add_resource(CodeResource* cr);
    void clear_marks();
    void sweep();
    int  calculate_size();

  private:
    void add_chunk();
  };
}

#endif
