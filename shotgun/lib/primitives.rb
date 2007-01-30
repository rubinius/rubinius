
require 'cpu/primitives'

class ShotgunPrimitives
  
  Header = ""
  
  def generate_select(fd, op="prim")
    i = 0
    order = CPU::Primitives::Primitives
    fd.puts "switch(#{op}) {"
    order.each do |ins|
      code = send(ins) rescue nil
      if code
        fd.puts "   case #{i}: {"
        fd.puts code
        fd.puts "   break;\n    }"
      else
        STDERR.puts "Problem with CPU primitive: #{ins}"
      end
      i += 1
    end
    i = CPU::Primitives::FirstRuntimePrimitive
    order = CPU::Primitives::RuntimePrimitives
    order.each do |ins|
      code = send(ins) rescue nil
      if code
        fd.puts "   case #{i}: {"
        fd.puts code
        fd.puts "   break; \n   }"
      else
        STDERR.puts "Problem with runtime primitive: #{ins}"
      end
      i += 1
    end
    fd.puts "}"
    fd.puts
  end

  def generate_declarations(fd)
    fd.puts "int _int, j, k, m;"
    fd.puts "OBJECT _lit, t1, t2, t3;"
    fd.puts "char buffer[1024];"
  end
    
  def noop
    "stack_pop();"
  end
  
  def add
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      stack_push(fixnum_add(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def bignum_add
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( INTEGER_P(t1) ) {
      stack_push(bignum_add(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def sub
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      stack_push(fixnum_sub(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def bignum_sub
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( INTEGER_P(t1) ) {
      stack_push(bignum_sub(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def fixnum_mul
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      stack_push(fixnum_mul(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def fixnum_size
    <<-CODE
    self = stack_pop();

    if( FIXNUM_P(self) ) {
      stack_push(I2N(sizeof(int)));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def bignum_mul
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if( INTEGER_P(t1) ) {
      stack_push(bignum_mul(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def fixnum_div
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      stack_push(fixnum_div(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def bignum_div
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( INTEGER_P(t1) ) {
      stack_push(bignum_div(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def equal
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    /* i think we can assume self is fixnum but i don't know where this is called */
    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {

      /* Don't need to convert them to ints since it would have been redundant. */
      /* Its done everywhere else -- going to cast them explicitly just in case */
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      if(j == k) {
        stack_push(Qtrue); 
      } else {
        stack_push(Qfalse);
      }
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def bignum_equal
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if( INTEGER_P(t1) ) {
      stack_push(bignum_equal(state, self, t1));
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def compare
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      if (j < k)
        stack_push(I2N(-1));
      else if (j > k)
        stack_push(I2N(1));
      else
        stack_push(I2N(0));
    }
    CODE
  end
  
  def at
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!FIXNUM_P(t1) || !INDEXED(self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      if(j >= NUM_FIELDS(self)) {
        _ret = FALSE;
      } else {
        stack_push(NTH_FIELD(self, j));
      }
    }
    CODE
  end
  
  def put
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    if(!INDEXED(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      if(j >= NUM_FIELDS(self)) {
        _ret = FALSE;
      } else {
        SET_FIELD(self, j, t2);
        stack_push(t2);
      }
    }
    CODE
  end
  
  def fields
    <<-CODE
    self = stack_pop();
    if(!REFERENCE_P(self)) {
      _ret = FALSE;
    } else {
      stack_push(I2N(NUM_FIELDS(self)));
    }
    CODE
  end
  
  def allocate
    <<-CODE
    self = stack_pop();
    if(!RISA(self, class)) {
      _ret = FALSE;
    } else {
      t1 = class_get_instance_fields(self);
      if(!FIXNUM_P(t1)) {
        _ret = FALSE;
      } else {
        t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1));
        stack_push(t2);
      }
    }
    CODE
  end
  
  def allocate_count
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!RISA(self, class)) {
      _ret = FALSE;
    } else {
      if(!FIXNUM_P(t1)) {
        _ret = FALSE;
      } else {
        t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1));
        stack_push(t2);
      }
    }
    CODE
  end
  
  def allocate_bytes
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!RISA(self, class)) {
      _ret = FALSE;
    } else {
      if(!FIXNUM_P(t1)) {
        _ret = FALSE;
      } else {
        t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1) / 4);
        object_make_byte_storage(state, t2);
        stack_push(t2);
      }
    }
    CODE
  end
  
  def create_block
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    t3 =   Qnil;
    
    if(!FIXNUM_P(t1) || !FIXNUM_P(t2)) {
      _ret = FALSE;
    } else {
      if(!RISA(self, methctx)) {
        if(RISA(self, blokctx)) {
          t3 = blokenv_get_home(blokctx_get_env(self));
        }
      } else {
        t3 = self;
      }
      
      if(t3 == Qnil) {
        printf("Create block failed, %s!!\\n", _inspect(self));
        _ret = FALSE;
      } else {
        methctx_set_ip(self, I2N(c->ip));
        j = FIXNUM_TO_INT(methctx_get_ip(self)) + 5;
        t2 = blokenv_s_under_context(state, t3, j, t1, t2);
        stack_push(t2);
      }
    }
    CODE
  end
  
  def block_given
    <<-CODE
    if(RTEST(c->block)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def block_call
    <<-CODE
    self = stack_pop();
    if(!RISA(self, blokenv)) {
      _ret = FALSE;
    } else {
      t2 = blokenv_create_context(state, self);
      blokctx_set_sender(t2, c->active_context);
      blokctx_set_sp(t2, I2N(c->sp));
      cpu_activate_context(state, c, t2, blokenv_get_home(self));
    }
    CODE
  end
  
  def io_write
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!RISA(self, io) || !RISA(t1, string)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(io_get_descriptor(self));
      buf = string_byte_address(state, t1);
      k = FIXNUM_TO_INT(string_get_bytes(t1));
      k = write(j, buf, k);
      t2 = I2N(k);
      if(k != FIXNUM_TO_INT(t2)) {
        t2 = bignum_new(state, k);
      }
      stack_push(t2);
    }
    CODE
  end
  
  def io_read
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!RISA(self, io) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      t2 = string_new2(state, NULL, FIXNUM_TO_INT(t1));
      j = FIXNUM_TO_INT(io_get_descriptor(self));
      k = read(j, string_byte_address(state, t2), FIXNUM_TO_INT(t1));
      if(k != FIXNUM_TO_INT(t1)) {
        t3 = string_new2(state, NULL, k);
        memcpy(string_byte_address(state, t3), string_byte_address(state, t2), k);
        t2 = t3;
      }
      stack_push(t2);
    }
    CODE
  end
  
  def create_pipe
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    
    if(!RISA(t1, io) || !RISA(t2, io)) {
      _ret = FALSE;
    } else {
      j = pipe(fds);
      if(!j) {
        SET_FIELD(t1, 0, I2N(fds[0]));
        SET_FIELD(t2, 0, I2N(fds[1]));
      }
      stack_push(I2N(j));
    }
    CODE
  end
  
  def io_open
    <<-CODE
    FILE *_fobj;
    char *_path, *_mode;
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    if(!RISA(t1, string) || !RISA(t2, string)) {
      _ret = FALSE;
    } else {
      _path = string_as_string(state, t1);
      _mode = string_as_string(state, t2);
      _fobj = fopen(_path, _mode);
      t3 = NEW_OBJECT(self, 2);
      SET_FIELD(t3, 0, I2N(fileno(_fobj)));
      SET_FIELD(t3, 1, t1);
      free(_path);
      free(_mode);
      stack_push(t3);
    }
    CODE
  end
  
  def io_close
    <<-CODE
    self = stack_pop();
    if(!RISA(self, io)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(io_get_descriptor(self));
      if(j < 0) {
        _ret = FALSE;
      } else {
        if(!close(j)) {
          io_set_descriptor(self, I2N(-1));
          stack_push(Qtrue);
        } else {
          stack_push(Qfalse);
        }
      }
    }
    CODE
  end
  
  def file_unlink
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      char *name;
      name = string_as_string(state, t1);
      if(unlink(name) == 0) {
        stack_push(Qtrue);
      } else {
        /* TODO translate errno into an exception. */
        stack_push(Qfalse);
      }
      free(name);
    }
    CODE
  end
  
=begin
  def socket_open
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    j = socket(FIXNUM_TO_INT(t1), FIXNUM_TO_INT(t2), FIXNUM_TO_INT(t3));
    if(j == -1) {
      stack_push(Qfalse);
    } else {
      t4 = NEW_OBJECT(self, 2);
      SET_FIELD(t3, 0, I2N(j));
    }
    CODE
  end  
=end

  Header << <<-CODE
  CODE
  
  def gettimeofday
    <<-CODE
    t1 = stack_pop(); /* class */
    if(!RISA(t1, class)) {
      _ret = FALSE;
    } else {
      do { /* introduce a new scope */
        struct time_data td;
        struct time_data *tdp;
      
        j = sizeof(struct time_data) / 4;
        self = NEW_OBJECT(t1, j+1);
        object_make_byte_storage(state, self);
        k = gettimeofday(&td.tv, &td.tz);
        tdp = (struct time_data*)BYTES_OF(self);
      
        *tdp = td;
      } while(0);
      stack_push(self);
    }
    CODE
  end

  def time_at
    <<-CODE
      t1 = stack_pop(); // Time class
      t2 = stack_pop(); // Bignum or Fixnum, seconds
      t3 = stack_pop(); // Bignum or Fixnum, usecs
      if(!RISA(t1, class) || !INTEGER_P(t2) || !INTEGER_P(t3)) {
        _ret = FALSE;
      } else {
        do { /* introduce a new scope */
          long tv_sec;
          long tv_usec;
          if(FIXNUM_P(t2)) {
            tv_sec = (long)FIXNUM_TO_INT(t2);
          } else {
            tv_sec = (long)bignum_to_int(state, t2);
          }

          if(FIXNUM_P(t3)) {
            tv_usec = (long)FIXNUM_TO_INT(t3);
          } else {
            tv_usec = (long)bignum_to_int(state, t3);
          }

          struct time_data td;
          struct time_data *tdp;

          j = sizeof(struct time_data) / 4;
          self = NEW_OBJECT(t1, j+1);
          object_make_byte_storage(state, self);
          k = gettimeofday(&td.tv, &td.tz); // fetch the local timezone
          td.tv.tv_sec = tv_sec; // assign specified seconds
          td.tv.tv_usec = tv_usec; // assign specified microseconds          
          tdp = (struct time_data*)BYTES_OF(self);    
          *tdp = td;
        } while(0);
        stack_push(self);
      }
    CODE
  end
  
  def time_seconds
    <<-CODE
    self = stack_pop();
    if(!REFERENCE_P(self)) {
      _ret = FALSE;
    } else {
      struct time_data *tdp;
      tdp = (struct time_data*)BYTES_OF(self);
      stack_push(I2N(tdp->tv.tv_sec));
    }
    CODE
  end
  
  def strftime
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();

    if(!RISA(t1, string) || !REFERENCE_P(self)) {
       _ret = FALSE;
    } else {
      struct tm *time;
      time_t secs;
      char *format = NULL;
      char str[MAX_STRFTIME_OUTPUT+1];
      size_t out;
      struct time_data *tdp;

      format = string_as_string(state, t1);
      tdp = (struct time_data*)BYTES_OF(self);
      secs = tdp->tv.tv_sec;

      time = localtime(&secs);
      out = strftime(str, MAX_STRFTIME_OUTPUT-1, format, time);
      str[MAX_STRFTIME_OUTPUT] = '\\0';
      t2 = string_new2(state, str, out);
      stack_push(t2);
      if(format) {free(format);}
    }
    CODE
  end

  def fixnum_to_s
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      static const char digitmap[] = "0123456789abcdefghijklmnopqrstuvwxyz";
      char buf[100];
      char *b = buf + sizeof(buf);

      j = FIXNUM_TO_INT(t1);
      k = FIXNUM_TO_INT(self);
      /* Algorithm taken from 1.8.4 rb_fix2str */
      if(j < 2 || 36 < j) {
        _ret = FALSE;
      } else if(k == 0) {
        stack_push(string_new(state, "0"));
      } else {
        m = 0;
        if(k < 0) {
          k = -k;
          m = 1;
        }
        *--b = '\\0';
        do {
          *--b = digitmap[(int)(k % j)];
        } while(k /= j);
        if(m) {
          *--b = '-';
        }
        stack_push(string_new(state, b));
      }
    }
    CODE
  end

  def bignum_to_s
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(bignum_to_s(state, self, t1));
    }
    CODE
  end

  def logical_class
    <<-CODE
    self = stack_pop();
    stack_push(object_logical_class(state, self));
    CODE
  end

  def object_id
    <<-CODE
    self = stack_pop();
    stack_push(UI2N(self));
    CODE
  end

  def hash_set
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    t3 =   stack_pop();

    if(!RISA(self, hash) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      hash_add(state, self, FIXNUM_TO_INT(t1), t2, t3);
      stack_push(t3);
    }
    CODE
  end

  def hash_get
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();

    if(!RISA(self, hash) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      t3 = hash_get_undef(state, self, FIXNUM_TO_INT(t1));
      stack_push(t3);
    }
    CODE
  end

  def hash_object
    <<-CODE
    self = stack_pop();
    t1 = I2N(object_hash_int(state, self));
    stack_push(t1);
    CODE
  end
  
  def hash_delete
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!RISA(self, hash) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      t2 = hash_delete(state, self, FIXNUM_TO_INT(t1));
      stack_push(t2);
    }
    CODE
  end

  def symbol_index
    <<-CODE
    self = stack_pop();
    if(!SYMBOL_P(self)) {
      _ret = FALSE;
    } else {
      stack_push(I2N(symbol_to_index(state, self)));
    }
    CODE
  end

  def symbol_lookup
    <<-CODE
    self = stack_pop();
    if(!RISA(self, string)) {
      _ret = FALSE;
    } else {
      stack_push(string_to_sym(state, self));
    }
    CODE
  end

  def dup_into
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    j =    FIXNUM_TO_INT(stack_pop());
    if(!REFERENCE_P(self) || !REFERENCE_P(t1)) {
      _ret = FALSE;
    } else {
      object_copy_fields_from(state, t1, self, j, NUM_FIELDS(t1) - j);
      HEADER(t1)->flags = HEADER(self)->flags;
      stack_push(t1);
    }
    CODE
  end

  def tuple_shifted
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop(); /* distance to shift. */
    if(!FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      t2 = tuple_new(state, NUM_FIELDS(self) + j);
      object_copy_fields_shifted(state, self, t2, j);
      stack_push(t2); 
    }
    CODE
  end

  def get_byte
    <<-CODE
    self = stack_pop();
    t1 = stack_pop(); /* index */
    if(!FIXNUM_P(t1) || !object_stores_bytes_p(state, self)) {
      _ret = FALSE;
    } else {
      unsigned char *indexed;
      j = FIXNUM_TO_INT(t1);
      k = bytearray_bytes(state, self);
      if (j < 0 || j >= k) {
        _ret = FALSE;
      } else {
        indexed = (unsigned char*)bytearray_byte_address(state, self);
        indexed += j;
        stack_push(UI2N(*indexed));
      }
    }
    CODE
  end
    
  def fetch_bytes
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    if(!FIXNUM_P(t1) || !FIXNUM_P(t2) || !object_stores_bytes_p(state, self)) {
      _ret = FALSE;
    } else {
      char *source, *dest;
      int num;
      j = FIXNUM_TO_INT(t1);
      k = FIXNUM_TO_INT(t2);
      m = bytearray_bytes(state, self);

      num = abs(j - k);

      if(j < 0 || k < 0 || j > m || m < num) {
        _ret = FALSE;
      } else {
        t3 = bytearray_new(state, k+1);
        source = (char*)bytearray_byte_address(state, self);
        dest = (char*)bytearray_byte_address(state, t3);
        source += j;
        memcpy(dest, source, k);
        dest[k] = 0;
        stack_push(t3);
      }
    }
    CODE
  end

  def compare_bytes
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();

    if(!object_stores_bytes_p(state, self) || !object_stores_bytes_p(state, t1)) {
      _ret = FALSE;
    } else {
      j = bytearray_bytes(state, self);
      k = bytearray_bytes(state, t1);
      if(j != k) {
        stack_push(Qfalse);
      } else if(j == 0) {
        stack_push(Qtrue);
      } else {
        k = memcmp(bytearray_byte_address(state, self), bytearray_byte_address(state, t1), j);
        if(k > 1) {k = 1;} // Normalize return
        if(k < -1) {k = -1;} // values for <=> method.
        stack_push(I2N(k));
      }
    }
    CODE
  end

  def bytearray_size
    <<-CODE
    self = stack_pop();
    if (!object_stores_bytes_p(state, self)) {
      _ret = FALSE;
    } else {
      j = bytearray_bytes(state, self);
      stack_push(I2N(j));
    }
    CODE
  end
  
  def load_file
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      char *path = string_as_string(state, t1);
      t2 = cpu_unmarshal_file(state, path);
      free(path); 
      stack_push(t2);
    }
    CODE
  end
  
  def activate_as_script
    <<-CODE
    self = stack_pop();
    if(!RISA(self, cmethod)) {
      _ret = FALSE;
    } else {
      cpu_run_script(state, c, self);
      stack_push(Qtrue);
    }
    CODE
  end
  
  def stat_file
    <<-CODE
    struct stat sb = {0};
    self = stack_pop();
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      j = stat(string_as_string(state, t1), &sb);
      if(j != 0) {
        if(errno == ENOENT) {
          stack_push(I2N(1));
        } else if(errno == EACCES) {
          stack_push(I2N(2));
        } else {
          stack_push(Qfalse);
        }
      } else {
        t2 = tuple_new(state, 10);
        tuple_put(state, t2, 0, I2N((int)sb.st_ino));
        tuple_put(state, t2, 1, I2N((int)sb.st_mode));
        if((sb.st_mode & S_IFIFO) == S_IFIFO) {
          t3 = string_to_sym(state, string_new(state, "fifo"));
        } else if((sb.st_mode & S_IFCHR) == S_IFCHR) {
          t3 = string_to_sym(state, string_new(state, "char"));
        } else if((sb.st_mode & S_IFDIR) == S_IFDIR) {
          t3 = string_to_sym(state, string_new(state, "dir"));
        } else if((sb.st_mode & S_IFBLK) == S_IFBLK) {
          t3 = string_to_sym(state, string_new(state, "block"));
        } else if((sb.st_mode & S_IFREG) == S_IFREG) {
          t3 = string_to_sym(state, string_new(state, "regular"));
        } else if((sb.st_mode & S_IFLNK) == S_IFLNK) {
          t3 = string_to_sym(state, string_new(state, "link"));
        } else if((sb.st_mode & S_IFSOCK) == S_IFSOCK) {
          t3 = string_to_sym(state, string_new(state, "socket"));
        } else {
          t3 = string_to_sym(state, string_new(state, "file"));
        }
        tuple_put(state, t2, 2, t3);
        tuple_put(state, t2, 3, I2N((int)sb.st_uid));
        tuple_put(state, t2, 4, I2N((int)sb.st_gid));
        tuple_put(state, t2, 5, I2N((int)sb.st_size));
        tuple_put(state, t2, 6, I2N((int)sb.st_blocks));
        tuple_put(state, t2, 7, UI2N((int)sb.st_atime));
        tuple_put(state, t2, 8, UI2N((int)sb.st_mtime));
        tuple_put(state, t2, 9, UI2N((int)sb.st_ctime));
      
        stack_push(t2);
      }
    }
    CODE
  end
  
  def process_exit
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    if(!FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      exit(FIXNUM_TO_INT(t1));
    }
    CODE
  end
  
  def micro_sleep
    <<-CODE
    struct timespec ts;
    stack_pop();
    t1 = stack_pop();
    if(!FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      ts.tv_sec = j / 1000000;
      ts.tv_nsec = (j % 1000000) * 1000;
      
      if(!nanosleep(&ts, NULL)) {
        t2 = Qfalse;
      } else {
        t2 = Qtrue;
      }
      stack_push(t2);
    }
    CODE
  end
  
  def activate_context
    <<-CODE
    self = stack_pop();
    if(blokctx_s_block_context_p(state, self)) {
      t1 = blokctx_home(state, self);
    } else {
      t1 = self;
    }
    
    cpu_activate_context(state, c, self, t1);
    CODE
  end
  
  def context_sender
    <<-CODE
    self = stack_pop();
    t1 = methctx_get_sender(self);
    
    if(t1 != Qnil) {
      methctx_reference(state, t1);
    }
    
    stack_push(t1);
    CODE
  end
  
  def string_to_sexp
    <<-CODE
    {
      GString *str;
      char *str1, *str2;
      self = stack_pop();
      t1 = stack_pop();
      t2 = stack_pop();
      t3 = stack_pop();
      if(!RISA(t1, string) || !FIXNUM_P(t2)) {
        _ret = FALSE;
      } else {
        str1 = string_as_string(state, self);
        str = g_string_new(str1);
        free(str1);
        str2 = string_as_string(state, t1);
        t1 = syd_compile_string(state,str2, str, FIXNUM_TO_INT(t2), RTEST(t3));
        free(str2);
        stack_push(t1);
      }
    }
    CODE
  end
  
  def file_to_sexp
    <<-CODE
    self = stack_pop();
    t1 = stack_pop(); /* The filename */
    t2 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      GIOChannel *io;
      GError *err;
      char *name;
      
      err = NULL;

      name = string_as_string(state, t1);
      io = g_io_channel_new_file(name, "r", &err);
      if(io == NULL) {
        _ret = FALSE;
      } else {
        g_io_channel_set_encoding(io, NULL, &err);
        t1 = syd_compile_file(state,name, io, 1, RTEST(t2));
        g_io_channel_shutdown(io, TRUE, &err);
        g_io_channel_unref(io);
        stack_push(t1);
      }
      free(name);
    }
    CODE
  end
    
  def terminal_raw
    <<-CODE
    {
      char *env = getenv("_TERM_SETTINGS");
      stack_pop();
      if(env && env[0] != 0) {
        stack_push(Qfalse);
      } else {
        FILE *io;
        char sbuf[1024];
        
        io = popen("stty -g", "r");
        fgets(sbuf, 1023, io);
        setenv("_TERM_SETTINGS", sbuf, 1);
        pclose(io);
        system("stty raw");
        stack_push(Qtrue);
      }
    }
    CODE
  end
  
  def terminal_normal
    <<-CODE
    {
      char *env = getenv("_TERM_SETTINGS");
      stack_pop();
      if(!env || env[0] == 0) {
        stack_push(Qfalse);
      } else {
        char sbuf[1024];
        
        snprintf(sbuf, 1023, "stty %s", env);
        system(sbuf);
        stack_push(Qtrue);
      }
    }
    CODE
  end
  
  def regexp_new
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      stack_push(regexp_new(state, t1, t2));
    }
    CODE
  end
  
  def regexp_match
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      stack_push(regexp_match(state, self, t1));
    }
    CODE
  end
  
  def gc_start
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    if(RTEST(t1)) {
      state->om->collect_now = 0x4;
    } else {
      state->om->collect_now = 0x3;
    }
    stack_push(Qtrue);
    CODE
  end
    
  def get_ivar
    <<-CODE
    self = stack_pop();
    t1 = NTH_FIELD(mo, 3);
    stack_push(object_get_ivar(state, self, t1));
    CODE
  end
  
  def set_ivar
    <<-CODE
    self = stack_pop();
    t1 = NTH_FIELD(mo, 3);
    t2 = stack_pop();
    object_set_ivar(state, self, t1, t2);
    stack_push(t2);
    CODE
  end
  
  def set_index
    <<-CODE
    self = stack_pop();
    if(!INDEXED(self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(NTH_FIELD(mo, 3));
      SET_FIELD(self, j, stack_pop());
      stack_push(NTH_FIELD(self, j));  
      }  
    CODE
  end
  
  def get_index
    <<-CODE
    self = stack_pop();
    if(!INDEXED(self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(NTH_FIELD(mo, 3));
      stack_push(NTH_FIELD(self, j));
    }
    CODE
  end

  def zlib_inflate
    <<-CODE

    stack_pop(); //class method, ignore self
    t1 = stack_pop(); //string to inflate
    if (!RISA(t1, string)) { //parameter must be a string
      _ret = FALSE;
      return 0;
    }
    else {
      unsigned char out_buffer[ZLIB_CHUNK_SIZE];
      unsigned char *input = (unsigned char *)string_as_string(state, t1);
      GString *output = g_string_new(NULL);

      z_stream zs;
      zs.zfree = Z_NULL;
      zs.zalloc = Z_NULL;
      zs.opaque = Z_NULL;
      zs.avail_in = strlen((char *)input) + 1; // Zero terminator is added afterwards, by GString, so we need to account for it. 
      zs.next_in = input;

      int zerr = inflateInit(&zs); // Returns zlib error code
      if (zerr != Z_OK) {
        inflateEnd(&zs);
        _ret = FALSE;
        free(input);
        g_string_free(output, TRUE);
        return 0;
      }

      do {
        zs.avail_out = ZLIB_CHUNK_SIZE;
        zs.next_out = out_buffer;
        zerr = inflate(&zs, Z_SYNC_FLUSH);
        k = ZLIB_CHUNK_SIZE - zs.avail_out; // How much we got.
        switch (zerr) {
          case Z_OK:
            // Fall through
          case Z_STREAM_END:
            g_string_append_len(output, (char *)out_buffer, k);
            break;
          default: // Punt on any other return value.
            inflateEnd(&zs);
            _ret = FALSE;
            free(input);
            g_string_free(output, TRUE);
            return 0;
          }
      } while (zs.avail_out == 0);

      inflateEnd(&zs);
      free(input);
 
      if (zerr != Z_STREAM_END && zerr != Z_OK) {
        _ret = FALSE; 
        return 0;
      }
      stack_push(string_new2(state, output->str, output->len));
      g_string_free(output, TRUE);
    }
    CODE
  end

  def zlib_deflate
    <<-CODE
    stack_pop(); //class method, ignore self
    t1 = stack_pop(); //string to deflate
    if (!RISA(t1, string)) { //parameter must be a string
      _ret = FALSE;
      return 0;
    }
    else {
      unsigned char out_buffer[ZLIB_CHUNK_SIZE];
      unsigned char *input = (unsigned char *)string_as_string(state, t1);
      GString *output = g_string_new(NULL);

      z_stream zs;
      zs.zfree = Z_NULL;
      zs.zalloc = Z_NULL;
      zs.opaque = Z_NULL;

      int zerr = deflateInit(&zs, Z_DEFAULT_COMPRESSION); // Returns zlib error code
      if (zerr != Z_OK) {
        deflateEnd(&zs);
        _ret = FALSE;
        free(input);
        g_string_free(output, TRUE);
        return 0;
      }

      zs.avail_in = strlen((char *)input); // Lower than for zlib_inflate, so that we don't consume the zero-terminator.
      zs.next_in = input;
    do {
      zs.avail_out = ZLIB_CHUNK_SIZE;
      zs.next_out = out_buffer;
      zerr = deflate(&zs, Z_FINISH);
      k = ZLIB_CHUNK_SIZE - zs.avail_out; // How much we got.
      switch (zerr) {
        case Z_OK:
          // Fall through
        case Z_STREAM_END:
          g_string_append_len(output, (char *)out_buffer, k);
          break;
        default: // Punt on any other return value.
          deflateEnd(&zs);
          _ret = FALSE;
          free(input);
          g_string_free(output, TRUE);
          return 0;
        }
    } while (zs.avail_out == 0);

    deflateEnd(&zs);
    free(input);

    if (zerr != Z_STREAM_END && zerr != Z_OK) {
      _ret = FALSE; 
      return 0;
    }
    stack_push(string_new2(state, output->str, output->len));
    g_string_free(output, TRUE);
    }
    CODE
  end
  
  def fixnum_modulo
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j % k;
      t2 = I2N(m);
      if(m != FIXNUM_TO_INT(t2)) {
        t2 = bignum_add(state, bignum_new(state, j), bignum_new(state, k));
      }
      stack_push(t2);
    }
    CODE
  end
  
  def marshal_object
    <<-CODE
    stack_pop(); /* class */
    t1 = stack_pop();
    stack_push(cpu_marshal(state, t1));
    CODE
  end
  
  def unmarshal_object
    <<-CODE
    stack_pop(); /* class */
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      stack_push(cpu_unmarshal(state, string_byte_address(state, t1)));
    }
    CODE
  end
  
  def marshal_to_file
    <<-CODE
    char *_path;
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    if(!RISA(t2, string)) {
      _ret = FALSE;
    } else {
      _path = string_as_string(state, t2);
      stack_push(cpu_marshal_to_file(state, t1, _path));
      free(_path);
    }
    CODE
  end
  
  def unmarshal_from_file
    <<-CODE
    char *_path;
    stack_pop(); /* class */
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      _path = string_as_string(state, t1);
      stack_push(cpu_unmarshal_file(state, _path));
      free(_path);
    }
    CODE
  end
  
  def archive_files
    <<-CODE
    char *path;
    stack_pop(); /* blah */
    t1 = stack_pop();
    if(!RISA(t1, string)) {
      _ret = FALSE;
    } else {
      path = string_as_string(state, t1);
      stack_push(archive_list_files(state, path));
      free(path);
    }
    CODE
  end
  
  def archive_get_file
    <<-CODE
    char *path, *file;
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    if(!RISA(t1, string) || !RISA(t2, string)) {
      _ret = FALSE;
    } else {
      path = string_as_string(state, t1);
      file = string_as_string(state, t2);
      stack_push(archive_get_file(state, path, file));
      free(path);
      free(file);
    }
    CODE
  end
  
  def archive_get_object
    <<-CODE
    char *path, *file;
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    if(!RISA(t1, string) || !RISA(t2, string)) {
      _ret = FALSE;
    } else {
      path = string_as_string(state, t1);
      file = string_as_string(state, t2);
      stack_push(archive_get_object(state, path, file));
      free(path);
      free(file);
    }
    CODE
  end
  
  def archive_add_file
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    if(!RISA(t1, string) || !RISA(t2, string) || !RISA(t3, string)) {
      _ret = FALSE;
    } else {
      t1 = archive_add_file(state, 
                           string_byte_address(state, t1), 
                           string_byte_address(state, t2),
                           string_byte_address(state, t3));
      stack_push(t1);
    }
    CODE
  end
  
  def archive_add_object
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    if(!RISA(t1, string) || !RISA(t2, string)) {
      _ret = FALSE;
    } else {
      t1 = archive_add_object(state, 
                           string_byte_address(state, t1), 
                           string_byte_address(state, t2),
                           t3);
      stack_push(t1);
    }
    CODE
  end
  
  def archive_delete_file
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    if(!RISA(t1, string) || !FIXNUM_P(t2)) {
      _ret = FALSE;
    } else {
      t3 = archive_delete_file(state, string_byte_address(state, t1), FIXNUM_TO_INT(t2));
      stack_push(t3);
    }
    CODE
  end  
  
  def float_to_s
    <<-CODE
    self = stack_pop();
    if( FLOAT_P(self) ) {
      stack_push(float_to_s(state, self));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_add
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FLOAT_P(self) && FLOAT_P(t1) ) {
      stack_push(float_add(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_sub
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FLOAT_P(self) && FLOAT_P(t1) ) {
      stack_push(float_sub(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_mul
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FLOAT_P(self) && FLOAT_P(t1) ) {
      stack_push(float_mul(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_equal
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FLOAT_P(self) && FLOAT_P(t1) ) {
      stack_push(float_equal(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def fixnum_and
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j & k;
      stack_push(I2N(m));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def fixnum_or
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j | k;
      stack_push(I2N(m));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def fixnum_xor
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    if( FIXNUM_P(self) && FIXNUM_P(t1) ) {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j ^ k;
      stack_push(I2N(m));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def fixnum_invert
    <<-CODE
    self = stack_pop();
    if( FIXNUM_P(self) ) {
      j = FIXNUM_TO_INT(self);
      stack_push(I2N(~j));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def fixnum_neg
    <<-CODE
    self = stack_pop();
    if( FIXNUM_P(self) ) {
      j = FIXNUM_TO_INT(self);
      stack_push(I2N(-j));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def fixnum_shift
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    t2   = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1) || !FIXNUM_P(t2)) {
      _ret = FALSE;
    } else {
      long value;
      value = FIXNUM_TO_INT(self);
      int  width;
      width = FIXNUM_TO_INT(t1);
      int  dir;
      dir = FIXNUM_TO_INT(t2);

      /* shift direction -1 == left, 1 == right          */
      /* negative width shifts in the opposite direction */
      if (width < 0) {
        dir   = -dir;
        width = -width;
      }

      if (dir == 1) {
        /* right shift */
        if (width > 0) {
          if (width >= sizeof(value)*8-1) {
            if (value < 0) {
              value = -1;
            } else {
              value = 0;
            }
          } else {
            value >>= width;
          }
        }
        t2 = I2N(value);
        stack_push(t2);
      }

      if (dir == -1) {
        /* left shift */
        /* if (width > sizeof(value)*8-1 || */
        /*   ((unsigned long)value) >> (sizeof(value)*8-1-width) > 0) { */
        /*  t2 = bignum_left_shift(state, self, width); */
        /*  stack_push(t2); */
        /* } else { */
          value <<= width;
          t2 = I2N(value);
          stack_push(t2);
        /* } */
      }
    }
    CODE
  end

  def bignum_to_float
    <<-CODE
    self = stack_pop();

    if( BIGNUM_P(self) ) {
      stack_push(float_new(state, bignum_to_double(state, self)));
    } else {
      _ret = FALSE;
    }
    CODE
  end


  def bignum_and
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( INTEGER_P(t1) ) {
      stack_push(bignum_and(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def bignum_or
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( INTEGER_P(t1) ) {
      stack_push(bignum_or(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def bignum_xor
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();

    if( INTEGER_P(t1) ) {
      stack_push(bignum_xor(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end

  def bignum_neg
    <<-CODE
    self = stack_pop();

    if( INTEGER_P(self) ) {
      stack_push(bignum_neg(state, self));
    } else {
      _ret = FALSE;
    }
    CODE
  end


  def bignum_invert
    <<-CODE
    self = stack_pop();

    if( INTEGER_P(self) ) {
    stack_push(bignum_invert(state, self));

    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_nan_p
    <<-CODE
    self = stack_pop();

    if( FLOAT_P(self) ) {
      stack_push(float_nan_p(state, self));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_infinite_p
    <<-CODE
    self = stack_pop();
    if(!FLOAT_P(self)) {
      _ret = FALSE;
    } else {
      stack_push(float_infinite_p(state, self));
    }
    CODE
  end
  
  def float_div
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!FLOAT_P(self) || !FLOAT_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(float_div(state, self, t1));
    }
    CODE
  end
  
  def float_uminus
    <<-CODE
    self = stack_pop();
    stack_push(float_uminus(state, self));
    CODE
  end
  
  def float_pow
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!FLOAT_P(self) || !FLOAT_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(float_pow(state, self, t1));
    }
    CODE
  end
  
  def float_to_i
    <<-CODE
    self = stack_pop();
    if(!FLOAT_P(self)) {
      _ret = FALSE;
    } else {
      stack_push(float_to_i(state, self));
    }
    CODE
  end
  
  def numeric_coerce
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!INTEGER_P(self) || !INTEGER_P(t1)) {
      _ret = FALSE;
    } else {
      t3 = array_new(state, 2);
      if(BIGNUM_P(self)) {
        if(BIGNUM_P(t1)) {
          array_set(state, t3, 0, t1);
        } else {
          array_set(state, t3, 0, bignum_new(state, FIXNUM_TO_INT(t1)));
        }
        array_set(state, t3, 1, self);
      } else {
        if(BIGNUM_P(t1)) {
          array_set(state, t3, 0, t1);
          array_set(state, t3, 1, bignum_new(state, FIXNUM_TO_INT(self)));
        } else {
          array_set(state, t3, 0, t1);
          array_set(state, t3, 1, self);
        }
      }
      stack_push(t3);
    }
    CODE
  end
  
  def bignum_compare
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!BIGNUM_P(self) || !BIGNUM_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(bignum_compare(state, self, t1));
    }
    CODE
  end
  
  def float_compare
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(!FLOAT_P(self) || !FLOAT_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(float_compare(state, self, t1));
    }
    CODE
  end
  
  def fixnum_to_f
    <<-CODE
    self = stack_pop();
    if(!FLOAT_P(self)) {
      _ret = FALSE;
    } else {
      stack_push(float_new(state, FIXNUM_TO_DOUBLE(self)));
    }
    CODE
  end

end

prim = ShotgunPrimitives.new
prim.generate_select(STDOUT)

