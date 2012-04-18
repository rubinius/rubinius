#ifndef RBX_UTIL_LOCAL_BUFFER_HPP
#define RBX_UTIL_LOCAL_BUFFER_HPP

namespace rubinius {
  struct LocalBuffer {
    void* buffer;

    LocalBuffer(size_t size) {
      buffer = malloc(size);
    }

    ~LocalBuffer() {
      free(buffer);
    }
  };
}

#endif
