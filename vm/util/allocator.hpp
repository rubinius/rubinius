#ifndef RBX_ALLOCATOR_H
#define RBX_ALLOCATOR_H

namespace rubinius {

  template <class T>
  class Allocator {

    public:
      std::vector<T*> chunks_;
      T* free_list_;
      size_t allocations_;
      int in_use_;

      static const size_t cChunkSize = 1024;
      static const size_t cChunkLimit = 32;

      Allocator()
        : free_list_(NULL)
        , allocations_(0)
        , in_use_(0)
      {}

      ~Allocator() {
        for(typename std::vector<T*>::iterator i = chunks_.begin(); i != chunks_.end(); ++i) {
          T* chunk = *i;
          delete[] chunk;
        }
      }

      void allocate_chunk(bool* needs_gc) {
        T* chunk = new T[cChunkSize];
        for(size_t i = 0; i < cChunkSize; i++) {
          chunk[i].clear();
          chunk[i].set_next(free_list_);
          free_list_ = &chunk[i];
        }

        chunks_.push_back(chunk);
        allocations_++;
        if(allocations_ >= cChunkLimit) {
          *needs_gc = true;
          allocations_ = 0;
        }
      }

      T* allocate(bool* needs_gc) {
        if(!free_list_) allocate_chunk(needs_gc);
        T* t = free_list_;
        free_list_ = t->next();

        t->clear();

        in_use_++;

        return t;
      }

  };

}

#endif // RBX_ALLOCATOR_H
