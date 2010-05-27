#ifndef RBX_GC_CODE_MANAGER
#define RBX_GC_CODE_MANAGER

#include "util/thread.hpp"

namespace rubinius {
  class CodeResource;
  class SharedState;

  class CodeManager {
    const static int cDefaultChunkSize = 64;

    struct Chunk {
      CodeResource** resources;
      Chunk* next;

      Chunk(int size);
      ~Chunk();
    };

    thread::Mutex mutex_;

    SharedState* shared_;

    int chunk_size_;
    Chunk* first_chunk_;
    Chunk* last_chunk_;

    Chunk* current_chunk_;
    int current_index_;

    int freed_resources_;
    int total_allocated_;
    int total_freed_;

    int bytes_used_;

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

    int& size() {
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
