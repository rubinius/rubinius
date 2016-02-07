#ifndef RBX_ALLOCATOR_H
#define RBX_ALLOCATOR_H

namespace rubinius {

  template <class T>
  class Allocator {

    public:
    std::vector<T*> chunks_;
    uintptr_t free_list_;
    size_t allocations_;
    size_t in_use_;
    utilities::thread::SpinLock lock_;

    static const size_t cChunkSize = 1024;
    static const size_t cChunkLimit = 128;

    Allocator()
      : free_list_((uintptr_t)-1)
      , allocations_(0)
      , in_use_(0)
    {
      lock_.init();
    }

    ~Allocator() {
      for(typename std::vector<T*>::iterator i = chunks_.begin(); i != chunks_.end(); ++i) {
        T* chunk = *i;
        delete[] chunk;
      }
    }

    void allocate_chunk(bool* needs_gc) {
      if(in_use_ != chunks_.size() * cChunkSize) {
        rubinius::bug("Allocating chunk with not all elements used");
      }
      T* chunk = new T[cChunkSize];
      for(size_t i = 0; i < cChunkSize; i++) {
        uintptr_t next_index = chunks_.size() * cChunkSize + i;
        chunk[i].set_next(free_list_);
        free_list_ = next_index;
      }

      chunks_.push_back(chunk);
      allocations_++;
      if(allocations_ >= cChunkLimit) {
        *needs_gc = true;
        allocations_ = 0;
      }
    }

    T* allocate(bool* needs_gc) {
      utilities::thread::SpinLock::LockGuard lg(lock_);
      if(free_list_ == (uintptr_t)-1) allocate_chunk(needs_gc);
      T* t = from_index(free_list_);
      free_list_ = t->next();

      t->clear();
      in_use_++;

      return t;
    }

    uintptr_t allocate_index(bool* needs_gc) {
      utilities::thread::SpinLock::LockGuard lg(lock_);
      if(free_list_ == (uintptr_t)-1) allocate_chunk(needs_gc);

      uintptr_t current_index = free_list_;
      T* t = from_index(free_list_);
      free_list_ = t->next();
      t->clear();
      in_use_++;

      return current_index;
    }

    T* from_index(uintptr_t index) {
      return chunks_[index / cChunkSize] + (index % cChunkSize);
    }

    bool validate(T* ptr) {
      for(typename std::vector<T*>::iterator i = chunks_.begin(); i != chunks_.end(); ++i) {
        T* chunk = *i;
        if(ptr >= chunk && ptr < (chunk + cChunkSize)) {
          return true;
        }
      }
      return false;
    }

    void rebuild_freelist(std::vector<bool>* chunk_marks) {
      free_list_ = (uintptr_t)-1;
      in_use_ = 0;
      for(std::vector<int>::size_type i = chunks_.size() - 1;
          i != (std::vector<int>::size_type) -1; --i) {
        T* chunk = chunks_[i];
        if(!chunk_marks->at(i) && chunk == chunks_.back()) {
          delete[] chunk;
          chunks_.pop_back();
        } else {
          for(size_t j = 0; j < cChunkSize; j++) {
            T* t = &chunk[j];

            if(!t->in_use_p()) {
              t->set_next(free_list_);
              free_list_ = i * cChunkSize + j;
            } else {
              in_use_++;
            }
          }
        }
      }
    }

    class Iterator {
      Allocator<T>* allocator_;
      size_t chunk_nr_;
      size_t chunk_index_;
      T* current_;

    public:
      Iterator(Allocator<T>* allocator)
        : allocator_(allocator)
        , chunk_nr_(0)
        , chunk_index_(0)
        , current_(NULL)
      {
        advance();
      }

      bool more() {
        return current_ != 0;
      }

      void advance() {
        for(;chunk_nr_ < allocator_->chunks_.size(); ++chunk_nr_) {
          T* chunk = allocator_->chunks_[chunk_nr_];
          for(;chunk_index_ < allocator_->cChunkSize; ++chunk_index_) {
            T* t = &chunk[chunk_index_];
            if(t->in_use_p()) {
              current_ = t;
              // Make sure next iteration points to the next element
              ++chunk_index_;
              return;
            }
          }
          chunk_index_ = 0;
        }
        // Nothing found, we're at the end here
        current_ = NULL;
      }

      T* operator->() {
        return current_;
      }

      T* current() {
        return current_;
      }

    };

  };

}

#endif // RBX_ALLOCATOR_H
