require 'inline'

class Platform
  
  inline do |b|
    b.include "<sys/types.h>"
    b.include "<sys/uio.h>"
    b.include "<unistd.h>"
    b.include "<sys/time.h>"
    b.add_compile_flags "-I include"
    b.include '"rubinius.h"'
    b.add_type_converter('OBJECT','NUM2UINT','UINT2NUM')
    
    b.c_singleton <<-CODE
    int io_write_old(int fd, int size, char *str) {
      ssize_t ret;
      
      ret = write(fd, (void*)str, (size_t)size);
      
      return (int)ret;
    }
    CODE
    
    b.c_singleton <<-CODE
    int io_write(OBJECT fd_o, OBJECT size_o, OBJECT str_o) {
      long fd = (long)FIXNUM_TO_INT(fd_o);
      long sz = (long)FIXNUM_TO_INT(size_o);
      char *str = (char*)BYTES_OF(str_o);
      ssize_t ret;
      
      ret = write(fd, str, (size_t)sz);
      
      return (int)ret;
    }
    CODE
    
    b.c_singleton <<-CODE
    int io_read_old(int fd, int size, int str_p) {
      ssize_t ret;
      void *str;
      str = (void*)str_p;
      
      ret = read(fd, str, (size_t)size);
      
      return (int)ret;
    }
    CODE
    
    b.c_singleton <<-CODE
    int io_read(OBJECT fd_o, OBJECT size_o, OBJECT str_o) {
      ssize_t ret;
      void *str = (void*)BYTES_OF(str_o);
      long fd = FIXNUM_TO_INT(fd_o);
      long sz = FIXNUM_TO_INT(size_o);
      
      ret = read(fd, str, (size_t)sz);
      
      return (int)ret;
    }
    CODE
    
    b.c_singleton <<-CODE
    int create_pipe(OBJECT lhs, OBJECT rhs) {
      int fds[2];
      int ret;
      
      ret = pipe(fds);
      if(ret) { return ret; }
      SET_FIELD(lhs, 0, INT_TO_FIXNUM(fds[0]));
      SET_FIELD(rhs, 0, INT_TO_FIXNUM(fds[1]));
      
      return ret;
    }
    CODE
    
    b.prefix <<-CODE
    struct time_data {
      struct timeval tv;
      struct timezone tz;
    };
    CODE
    
    b.c_singleton <<-CODE
    int time_data_size() {
      return sizeof(struct time_data);
    }
    CODE
    
    b.c_singleton <<-CODE
    int fill_time(OBJECT time) {
      struct time_data td;
      struct time_data *tdp;
      OBJECT data;
      int ret;
      
      ret = gettimeofday(&td.tv, &td.tz);
      
      if(ret) { return ret; }
      data = NTH_FIELD(time, 1);
      
      tdp = (struct time_data*)BYTES_OF(data);
      
      *tdp = td;
      
      return ret;
    }
    CODE
    
    b.c_singleton <<-CODE
    int print_time(OBJECT time_o, OBJECT format_o, OBJECT str_o, OBJECT str_size) {
      struct tm *time;
      time_t secs;
      char *format;
      char *str;
      int s_sz;
      size_t out;
      OBJECT sec_s;
      struct time_data *tdp;
      
      format = (char*)BYTES_OF(format_o);
      str = (char*)BYTES_OF(str_o);
      s_sz = FIXNUM_TO_INT(str_size);
      
      sec_s = NTH_FIELD(time_o, 1);
      tdp = (struct time_data*)BYTES_OF(sec_s);
      secs = tdp->tv.tv_sec;
      
      time = localtime(&secs);
      // printf("Seconds: %d, %d (%s, %d, %d)\\n", secs, time->tm_year, format, s_sz, str_size);
      
      out = strftime(str, s_sz, format, time);
      
      return (int)out;
    }
    CODE
  end
end