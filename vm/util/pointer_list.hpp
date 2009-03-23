#ifndef RBX_UTIL_POINTER_LIST
#define RBX_UTIL_POINTER_LIST

template <typename T>
class PointerList {
  int size_;
  int buffer_size_;
  T** buffer_;

public:

  const static int cIncrements = 5;
  const static int cInitialSize = 5;

  PointerList()
    : size_(0)
      , buffer_size_(cInitialSize)
      , buffer_(new T*[cInitialSize])
  {}

  ~PointerList() {
    delete[] buffer_;
  }

  int size() {
    return size_;
  }

  T* get(int pos) {
    return buffer_[pos];
  }

  void set(int pos, T* frame) {
    if(pos >= buffer_size_) {
      buffer_size_ = pos + cIncrements;
      T** new_buf = new T*[buffer_size_];
      for(int i = 0; i < size_; i++) {
        new_buf[i] = buffer_[i];
      }

      buffer_ = new_buf;
    }

    buffer_[pos] = frame;
    size_ = pos + 1;
  }

  void clear() {
    size_ = 0;
  }

  void push_back(T* frame) {
    set(size_, frame);
  }

  class iterator {
    PointerList* list_;
    int pos_;

  public:
    iterator(PointerList* list, int start_pos)
      : list_(list)
        , pos_(start_pos)
    {}

    iterator()
      : list_(0)
        , pos_(0)
    {}

    T* operator->() {
      return list_->get(pos_);
    }

    T* operator*() {
      return list_->get(pos_);
    }

    void operator++() {
      pos_++;
    }

    void operator++(int) {
      pos_++;
    }

    bool operator==(iterator other) {
      return pos_ == other.pos_;
    }

    bool operator!=(iterator other) {
      return pos_ != other.pos_;
    }

    bool more_p() {
      return pos_ < list_->size();
    }
  };

  typedef iterator const_iterator;

  iterator begin() {
    return iterator(this, 0);
  }

  iterator end() {
    return iterator(this, size_);
  }
};

#endif
