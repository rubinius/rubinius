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
        STDERR.puts "Problem with #{ins}"
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
        STDERR.puts "Problem with #{ins}"
      end
      i += 1
    end
    fd.puts "}"    
  end

  def generate_declarations(fd)
    fd.puts "int _int, j, k;"
    fd.puts "OBJECT _lit, t1, t2, t3;"
  end
    
  def noop
    "stack_pop(); _ret = TRUE;"
  end
  
  def add
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      t2 = I2N(FIXNUM_TO_INT(self) + FIXNUM_TO_INT(t1));
      stack_push(t2);
      _ret = TRUE;
    }
    CODE
  end
  
  def sub
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      t2 = I2N(FIXNUM_TO_INT(self) - FIXNUM_TO_INT(t1));
      stack_push(t2);
      _ret = TRUE;
    }
    CODE
  end
  
  def equal
    <<-CODE
    self = stack_pop();
    t1   = stack_pop();
    if(!FIXNUM_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      /* Don't need to convert them to ints since it would have been redundent. */
      if(self == t1) {
        stack_push(Qtrue); 
      } else {
        stack_push(Qfalse);
      }
      _ret = TRUE;
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
      if(j == k) {
        stack_push(I2N(0)); 
      } else if(j < k) {
        stack_push(I2N(-1));
      } else {
        stack_push(I2N(1));
      }
      _ret = TRUE;
    }
    CODE
  end
  
  def at
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!FIXNUM_P(t1) || !REFERENCE_P(self) || object_stores_bytes_p(state, self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      if(j > NUM_FIELDS(self)) {
        _ret = FALSE;
      } else {
        stack_push(NTH_FIELD(self, j));
        _ret = TRUE;
      }
    }
    CODE
  end
  
  def put
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    if(!FIXNUM_P(t1) || !REFERENCE_P(self) || object_stores_bytes_p(state, self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      if(j > NUM_FIELDS(self)) {
        _ret = FALSE;
      } else {
        SET_FIELD(self, j, t2);
        stack_push(t2);
        _ret = TRUE;
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
      _ret = TRUE;
    }
    CODE
  end
  
  def allocate
    <<-CODE
    self = stack_pop();
    if(!REFERENCE_P(self)) {
      _ret = FALSE;
    } else {
      t1 = class_get_instance_fields(self);
      if(!FIXNUM_P(t1)) {
        _ret = FALSE;
      } else {
        t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1));
        stack_push(t2);
        _ret = TRUE;
      }
    }
    CODE
  end
  
  def allocate_count
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!REFERENCE_P(self)) {
      _ret = FALSE;
    } else {
      if(!FIXNUM_P(t1)) {
        _ret = FALSE;
      } else {
        t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1));
        stack_push(t2);
        _ret = TRUE;
      }
    }
    CODE
  end
  
  def allocate_bytes
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!REFERENCE_P(self)) {
      _ret = FALSE;
    } else {
      if(!FIXNUM_P(t1)) {
        _ret = FALSE;
      } else {
        t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1) / 4);
        object_make_byte_storage(state, t2);
        stack_push(t2);
        _ret = TRUE;
      }
    }
    CODE
  end
  
  def create_block
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    
    if(!REFERENCE_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      methctx_set_ip(self, I2N(c->ip));
      t2 = blokctx_s_under_context(state, self);
      blokctx_set_last_op(t2, t1);
      stack_push(t2);
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
    _ret = TRUE;
    CODE
  end
  
  def block_call
    <<-CODE
    self = stack_pop();
    if(!REFERENCE_P(self)) {
      _ret = FALSE;
    } else {
      blokctx_set_sender(self, c->active_context);
      blokctx_set_ip(self, blokctx_get_start_op(self));
      blokctx_set_sp(self, I2N(c->sp));
      cpu_activate_context(state, c, self, blokctx_get_home(self));
    }
    CODE
  end
  
  def io_write
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!REFERENCE_P(self) || !object_kind_of_p(state, self, state->global->io)) {
      _ret = FALSE;
    } else {
      if(!REFERENCE_P(t1) || !object_kind_of_p(state, t1, state->global->string)) {
        _ret = FALSE;
      } else {
        j = FIXNUM_TO_INT(io_get_descriptor(self));
        buf = string_byte_address(state, t1);
        k = FIXNUM_TO_INT(string_get_bytes(t1));
        k = write(j, buf, k);
        /* TODO: need to return k here. */
        _ret = TRUE;
      }
    }
    CODE
  end
  
  def io_read
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(!REFERENCE_P(self) || !FIXNUM_P(t1)) {
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
      _ret = TRUE;
    }
    CODE
  end
  
  def create_pipe
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    
    if(!object_kind_of_p(state, t1, state->global->io) || 
               !object_kind_of_p(state, t2, state->global->io)) {
      _ret = FALSE;
    } else {
      j = pipe(fds);
      if(!j) {
        SET_FIELD(t1, 0, I2N(fds[0]));
        SET_FIELD(t2, 0, I2N(fds[1]));
      }
      stack_push(I2N(j));
      _ret = TRUE;
    }
    CODE
  end
  
  def io_open
    <<-CODE
    FILE *_fobj;
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    _fobj = fopen(string_as_string(state, t1), string_as_string(state, t2));
    t3 = NEW_OBJECT(self, 2);
    SET_FIELD(t3, 0, I2N(fileno(_fobj)));
    SET_FIELD(t3, 1, t1);
    stack_push(t3);
    CODE
  end
  
  def io_close
    <<-CODE
    self = stack_pop();
    j = FIXNUM_TO_INT(io_get_descriptor(self));
    if(!close(j)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
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
    do {
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
    CODE
  end
  
  def strftime
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    
    if(!object_stores_bytes_p(state, self) || 
        !object_kind_of_p(state, t1, state->global->string)) {
       _ret = FALSE;
    } else {
      struct tm *time;
      time_t secs;
      char *format;
      char *str, *tstr;
      int s_sz;
      size_t out;
      struct time_data *tdp;
      
      format = (char*)string_byte_address(state, t1);
      // A totally made up metric for calculating a target size.
      s_sz = 100 + (FIXNUM_TO_INT(string_get_bytes(t1)) * 4);
      str = (char*)malloc(s_sz);
      tdp = (struct time_data*)BYTES_OF(self);
      secs = tdp->tv.tv_sec;
      
      time = localtime(&secs);
      out = strftime(str, s_sz, format, time);
      // Keep enlarging my buffer until the buffer is larger than
      // strftime says it actually generated fewer characters.
      while(out == s_sz) {
        s_sz += 100;
        tstr = (char*)realloc(str, s_sz);
        if(!tstr) {
          _ret = FALSE;
          free(str);
          break;
        }
        out = strftime(str, s_sz, format, time);
      }
      if(_ret) {
        t2 = string_new2(state, str, out);
        free(str);
        stack_push(t2);
      }
      _ret = TRUE;
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
  
  def logical_class
    <<-CODE
    self = stack_pop();
    stack_push(object_logical_class(state, self));
    CODE
  end
  
  def object_id
    <<-CODE
    self = stack_pop();
    stack_push(I2N((int)self));
    CODE
  end
  
  def hash_set
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    t3 =   stack_pop();
    
    if(!REFERENCE_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      hash_add(state, self, FIXNUM_TO_INT(t1), t2, t3);
      stack_push(t3);
      _ret = TRUE;
    }
    CODE
  end
  
  def hash_get
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    
    if(!REFERENCE_P(self) || !FIXNUM_P(t1)) {
      _ret = FALSE;
    } else {
      t3 = hash_get(state, self, FIXNUM_TO_INT(t1));
      stack_push(t3);
      _ret = TRUE;
    }
    CODE
  end
  
  def hash_object
    <<-CODE
    self = stack_pop();
    t1 = I2N(object_hash_int(state, self));
    stack_push(t1);
    _ret = TRUE;
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
    stack_push(string_to_sym(state, self));
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
  
  def fetch_bytes
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    t2 =   stack_pop();
    if(!FIXNUM_P(t1) || !FIXNUM_P(t2) || !object_stores_bytes_p(state, self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(t1);
      k = FIXNUM_TO_INT(t2);
      m = bytearray_bytes(state, self);
      
      if(j < 0 || k < 0 || m <= (j - k)) {
        _ret = FALSE;
      } else {
        t3 = bytearray_new(state, j);
        object_copy_bytes_into(state, self, t3, j, k);
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
      if(j != bytearray_bytes(state, t1)) {
        stack_push(Qfalse);
      } else if(j == 0) {
        stack_push(Qtrue);
      } else {
        k = memcmp(bytearray_byte_address(state, self), bytearray_byte_address(state, t1), j);
        stack_push(I2N(k));
      }
    }
    CODE
  end
  
  def load_file
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    t2 = cpu_unmarshal_file(state, string_as_string(state, t1));
    stack_push(t2);
    CODE
  end
  
  def activate_as_script
    <<-CODE
    self = stack_pop();
    cpu_run_script(state, c, self);
    stack_push(Qtrue);
    CODE
  end
  
  def stat_file
    <<-CODE
    struct stat sb = {0};
    self = stack_pop();
    t1 = stack_pop();
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
      t2 = tuple_new(state, 7);
      tuple_put(state, t2, 0, I2N((int)sb.st_ino));
      tuple_put(state, t2, 1, I2N((int)sb.st_mode));
      if(sb.st_mode & S_IFIFO == S_IFIFO) {
        t3 = string_to_sym(state, string_new(state, "fifo"));
      } else if(sb.st_mode & S_IFCHR == S_IFCHR) {
        t3 = string_to_sym(state, string_new(state, "char"));
      } else if(sb.st_mode & S_IFDIR == S_IFDIR) {
        t3 = string_to_sym(state, string_new(state, "dir"));
      } else if(sb.st_mode & S_IFBLK == S_IFBLK) {
        t3 = string_to_sym(state, string_new(state, "block"));
      } else if(sb.st_mode & S_IFREG == S_IFREG) {
        t3 = string_to_sym(state, string_new(state, "regular"));
      } else if(sb.st_mode & S_IFLNK == S_IFLNK) {
        t3 = string_to_sym(state, string_new(state, "link"));
      } else if(sb.st_mode & S_IFSOCK == S_IFSOCK) {
        t3 = string_to_sym(state, string_new(state, "link"));
      } else {
        t3 = string_to_sym(state, string_new(state, "file"));
      }
      tuple_put(state, t2, 2, t3);
      tuple_put(state, t2, 3, I2N((int)sb.st_uid));
      tuple_put(state, t2, 4, I2N((int)sb.st_gid));
      tuple_put(state, t2, 5, I2N((int)sb.st_size));
      tuple_put(state, t2, 6, I2N((int)sb.st_blocks));
      
      stack_push(t2);
    }
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
    j = FIXNUM_TO_INT(NTH_FIELD(mo, 3));
    SET_FIELD(self, j, stack_pop());
    stack_push(NTH_FIELD(self, j));    
    CODE
  end
  
  def get_index
    <<-CODE
    self = stack_pop();
    j = FIXNUM_TO_INT(NTH_FIELD(mo, 3));
    stack_push(NTH_FIELD(self, j));    
    CODE
  end
end

prim = ShotgunPrimitives.new
prim.generate_select(STDOUT)