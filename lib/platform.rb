require 'inline'

class Platform
  
  inline do |b|
    b.include "<sys/types.h>"
    b.include "<sys/uio.h>"
    b.include "<unistd.h>"
    
    b.c_singleton <<-CODE
    int io_write(int fd, int size, char *str) {
      ssize_t ret;
      
      ret = write(fd, (void*)str, (size_t)size);
      
      return (int)ret;
    }
    CODE
    
    b.c_singleton <<-CODE
    int io_read(int fd, int size, int str_p) {
      ssize_t ret;
      void *str;
      str = (void*)str_p;
      
      ret = read(fd, str, (size_t)size);
      
      return (int)ret;
    }
    CODE
  end
end