require File.dirname(__FILE__) + '/primitive_names'
require File.dirname(__FILE__) + '/primitive_generator'

class ShotgunPrimitives
  include PrimitiveGenerator

  def generate_select(fd, op="prim")
    i = 1
    order = Bytecode::Compiler::Primitives

    fd.puts "switch(#{op}) {"
    fd.puts "   // NOOP is 0 and signifies a method with no primitive"
    order.each do |ins|
      unless ins
        i += 1
        next
      end
      meth = method(ins)
      code = send(ins)

      if code
        fd.puts "   case #{i}: { // #{ins}"
        if meth.arity < 0 # relinquish control to the new PrimitiveGenerator
          fd.puts PrimitiveGenerator.generate(meth)
        else # just put code
          fd.puts code
        end
        fd.puts "    break;\n   }"
      else
        STDERR.puts "Problem with CPU primitive: #{ins}"
      end

      i += 1
    end

    fd.puts "default:"
    fd.puts 'printf("Error: Primitive index out of range for this VM\n");'
    fd.puts "sassert(0);"
    fd.puts "}"
    fd.puts

    File.open("runtime_primitives.gen","w") do |f|
      total = Bytecode::Compiler::FirstRuntimePrimitive
      order = Bytecode::Compiler::RuntimePrimitives
      i = 0
      f.puts "switch(#{op} - #{total}) {"

      order.each do |ins|
        code = send(ins) rescue nil
        if code
          f.puts "   case #{i}: { // #{ins}"
          f.puts code
          f.puts "   break; \n   }"
        else
          STDERR.puts "Problem with runtime primitive: #{ins}"
        end
        i += 1
      end
      
      f.puts "default:"
      f.puts 'printf("Error: Primitive index out of range for this VM\n");'
      f.puts "sassert(0);"
      f.puts "}"
      f.puts
    end
    
    File.open("primitive_indexes.h", "w") do |f|
      i = 1
      Bytecode::Compiler::Primitives.each do |name|
        unless name
          i += 1
          next
        end
        f.puts "#define CPU_PRIMITIVE_#{name.to_s.upcase} #{i}"
        i += 1
      end
      
    end
    
    File.open("primitive_util.h", "w") do |f|
      size = Bytecode::Compiler::Primitives.size
      f.puts "struct prim2index { const char *name; int index; };"
      f.puts
      f.puts "static int calc_primitive_index(STATE, OBJECT str) {"
      f.puts "  static struct prim2index pi[] = {"
      
      i = 1
      Bytecode::Compiler::Primitives.each do |name|
        unless name
          i += 1
          next
        end
        f.puts %Q!  { "#{name}", #{i} },!
        i += 1
      end
      
      f.puts "  { NULL, 0 } };"
      f.puts <<-CODE
        int i;
        char *target = string_byte_address(state, str);
        for(i = 0; pi[i].name; i++) {
          if(!strcmp(target, pi[i].name)) return pi[i].index;
        }
        
        return -1;
      }
      CODE
    end
    
  end

  def add(_ = fixnum, t1 = fixnum)
    <<-CODE
    stack_push(fixnum_add(state, self, t1));
    CODE
  end
  
  def bignum_add(_ = bignum, t1 = bignum)
    <<-CODE
    stack_push(bignum_add(state, self, t1));
    CODE
  end
  
  def sub(_ = fixnum, t1 = fixnum)
    <<-CODE
    stack_push(fixnum_sub(state, self, t1));
    CODE
  end
  
  def bignum_sub
    <<-CODE
    POP(self, BIGNUM);
    POP(t1, INTEGER);

    stack_push(bignum_sub(state, self, t1));
    CODE
  end
  
  def fixnum_mul(_ = fixnum, t1 = fixnum)
    <<-CODE
    stack_push(fixnum_mul(state, self, t1));
    CODE
  end
  
  def fixnum_size(_ = fixnum)
    <<-CODE
    stack_push(I2N(sizeof(int)));
    CODE
  end

  def bignum_mul
    <<-CODE
    POP(self, BIGNUM);
    POP(t1, INTEGER);

    stack_push(bignum_mul(state, self, t1));
    CODE
  end
  
  def fixnum_div(_ = fixnum, t1 = fixnum)
    <<-CODE
    GUARD( FIXNUM_TO_INT(t1) != 0 ) // no divide by zero

    t3 = fixnum_divmod(state, self, t1);
    stack_push(array_get(state, t3, 0));
    CODE
  end
  
  def bignum_div(_ = bignum, t1 = bignum)
    <<-CODE
    // Can this ever happen since bignum will always be > zero?
    GUARD(!bignum_is_zero(state, t1));    
    stack_push(bignum_div(state, self, t1));
    CODE
  end

  def bignum_mod(_ = bignum, t1 = integer)
    <<-CODE
    stack_push(bignum_mod(state, self, t1));
    CODE
  end
  
  def equal(_ = fixnum, t1 = fixnum)
    <<-CODE
    /* No need to shift them to be longs, the comparison is the same. */
    if(self == t1) {
      stack_push(Qtrue); 
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def object_equal
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    
    if(self == t1) {
      stack_push(Qtrue); 
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def bignum_equal(_ = bignum, t1 = bignum)
    <<-CODE
    stack_push(bignum_equal(state, self, t1));
    CODE
  end
  
  def compare(_ = fixnum, t1 = fixnum)
    <<-CODE
    
    /* we can deduce == quickly and easily, so do it first. */
    
    if(self == t1) {
      stack_push(I2N(0));
    } else {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);

      if (j < k) {
        stack_push(I2N(-1));
      }
      else if (j > k) {
        stack_push(I2N(1));
      } else {
        /* We shouldn't be here! */
        stack_push(I2N(0));
      }
    }
    CODE
  end
  
  def fixnum_lt
    <<-CODE
    POP(self, FIXNUM);
    POP(t1,   FIXNUM);
    j = FIXNUM_TO_INT(self);
    k = FIXNUM_TO_INT(t1);
    
    stack_push(j < k ? Qtrue : Qfalse);
    CODE
  end
  
  def fixnum_le
    <<-CODE
    POP(self, FIXNUM);
    POP(t1,   FIXNUM);
    j = FIXNUM_TO_INT(self);
    k = FIXNUM_TO_INT(t1);
    
    stack_push(j <= k ? Qtrue : Qfalse);
    CODE
  end
  
  def fixnum_gt
    <<-CODE
    POP(self, FIXNUM);
    POP(t1,   FIXNUM);
    j = FIXNUM_TO_INT(self);
    k = FIXNUM_TO_INT(t1);
    
    stack_push(j > k ? Qtrue : Qfalse);
    CODE
  end
  
  def fixnum_ge
    <<-CODE
    POP(self, FIXNUM);
    POP(t1,   FIXNUM);
    j = FIXNUM_TO_INT(self);
    k = FIXNUM_TO_INT(t1);
    
    stack_push(j >= k ? Qtrue : Qfalse);
    CODE
  end
  
  
  def at
    <<-CODE
    self = stack_pop(); GUARD( INDEXED(self) )
    t1 = stack_pop(); GUARD( FIXNUM_P(t1) )
    j = FIXNUM_TO_INT(t1); GUARD( j >= 0 && j < NUM_FIELDS(self) )

    stack_push(NTH_FIELD(self, j));
    CODE
  end
  
  def put
    <<-CODE
    self = stack_pop(); GUARD( INDEXED(self) )
    t1 = stack_pop(); GUARD( FIXNUM_P(t1) )
    t2 = stack_pop(); // We do not care about the type
    j = FIXNUM_TO_INT(t1); GUARD( j >= 0 && j < NUM_FIELDS(self) )

    SET_FIELD(self, j, t2);
    stack_push(t2);
    CODE
  end
  
  def fields
    <<-CODE
    POP(self, REFERENCE);

    stack_push(I2N(NUM_FIELDS(self)));
    CODE
  end
  
  def allocate
    <<-CODE
    self = stack_pop(); GUARD( RISA(self, class) )
    t1 = class_get_instance_fields(self); GUARD( FIXNUM_P(t1) )

    t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1));
    stack_push(t2);
    CODE
  end
  
  def allocate_count
    <<-CODE
    self = stack_pop(); GUARD( RISA(self, class) )
    POP(t1, FIXNUM);

    t2 = NEW_OBJECT(self, FIXNUM_TO_INT(t1));
    stack_push(t2);
    CODE
  end
  
  def allocate_bytes
    <<-CODE
    self = stack_pop(); GUARD( RISA(self, class) )
    POP(t1, FIXNUM);
    t2 = bytearray_new(state, FIXNUM_TO_INT(t1));
    t2->klass = self;
    stack_push(t2);
    CODE
  end

  def io_seek
    <<-CODE
      off_t position;
      POP(self, IO);
      POP(t1, INTEGER); /* offset */
      POP(t2, FIXNUM); /* whence */

      j = io_to_fd(self);

      if (FIXNUM_P(t1)) {
        position = lseek(j, FIXNUM_TO_INT(t1), FIXNUM_TO_INT(t2));
      } else {
        position = lseek(j, bignum_to_ll(state, t1), FIXNUM_TO_INT(t2));
      }

      if (position == -1) {
        cpu_raise_from_errno(state, c, "Unable to seek");
        return TRUE;
      } else {
        stack_push(I2N(position));
      }
    CODE
  end

  def block_given
    <<-CODE
    ARITY(0)
    // pop true off the stack to conform to the "all primitives have a self rule"
    self = stack_pop(); GUARD( TRUE_P(self) )

    if( RTEST(cpu_current_block(state, c)) ) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end
  
  def block_call
    <<-CODE
    self = stack_pop(); 
    GUARD( RISA(self, blokenv) );
    
    c->blockargs = num_args;
    
    t3 = tuple_new(state, num_args);
    for(j = 0; j < num_args; j++) {
      t1 = stack_pop();
      tuple_put(state, t3, j, t1);
    }
    stack_push(t3);

    cpu_flush_sp(c);
    t2 = cpu_create_block_context(state, c, self, c->sp);
    cpu_activate_context(state, c, t2, blokenv_get_home(self), 1);
    CODE
  end
  
  def io_write
    <<-CODE
    POP(self, IO);
    POP(t1, STRING);

    j = io_to_fd(self);
    buf = string_byte_address(state, t1);
    k = FIXNUM_TO_INT(string_get_bytes(t1));
    k = write(j, buf, k);
    t2 = I2N(k);
    if(k != FIXNUM_TO_INT(t2)) {
      t2 = bignum_new(state, k);
    }
    stack_push(t2);
    CODE
  end
  
  def io_read
    <<-CODE
    POP(self, IO);
    POP(t1, FIXNUM);

    t2 = string_new2(state, NULL, FIXNUM_TO_INT(t1));
    j = io_to_fd(self);
    k = read(j, string_byte_address(state, t2), FIXNUM_TO_INT(t1));
    if(k == 0) {
      t2 = Qnil;
    } else if(k != FIXNUM_TO_INT(t1)) {
      t3 = string_new2(state, NULL, k);
      memcpy(string_byte_address(state, t3), string_byte_address(state, t2), k);
      t2 = t3;
    }
    stack_push(t2);
    CODE
  end
  
  def create_pipe
    <<-CODE
    self = stack_pop(); /* class */
    POP(t1, IO);
    POP(t2, IO);
    
    j = pipe(fds);
    if(!j) {
      io_wrap(state, t1, fds[0], "r");
      io_wrap(state, t2, fds[1], "r");
    }
    stack_push(I2N(j));
    CODE
  end
  
  def io_open
    <<-CODE
    int fd, mode, perm;
    char *_path;

    self = stack_pop(); /* class */
    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    
    _path = string_byte_address(state, t1);
    mode = FIXNUM_TO_INT(t2);
    perm = FIXNUM_TO_INT(t3);

    fd = open(_path, mode, perm);

    stack_push(I2N(fd));
    CODE
  end
  
  def io_reopen
    <<-CODE
    POP(self, IO);
    POP(t1, IO);
    
    /* MRI does a ton more with reopen, but I don't yet understand why.
       This seems perfectly acceptable currently. */
    
    k = io_to_fd(self);
    j = io_to_fd(t1);

    /* Probably needs an fflush here. */
    if(dup2(j, k) == -1) {
      cpu_raise_from_errno(state, c, "Unable to reopen IO object");
    } else {
      stack_push(Qtrue);
    }
    
    CODE
  end
  
  def io_close
    <<-CODE
    POP(self, IO);
    j = io_to_fd(self);
    GUARD(j >= 0);

    if( close(j) ) {
      stack_push(Qfalse);
    } else {
      cpu_event_clear(state, j);
      io_set_descriptor(self, I2N(-1));
      stack_push(Qtrue);
    }
    CODE
  end
  
  def io_operation
    <<-CODE
    POP(self, IO);
    POP(t1, FIXNUM);
    
    j = io_to_fd(self);
    GUARD(j >= 0);
    
    k = FIXNUM_TO_INT(t1);
    switch(k) {
      case 0:
        if(isatty(j)) {
          stack_push(Qtrue);
        } else {
          stack_push(Qfalse);
        }
        break;
      case 1:
        stack_push(string_new(state, ttyname(j)));
        break;
      default:
       stack_push(Qnil);
    }
    CODE
  end
  
  def file_unlink
    <<-CODE
    (void)stack_pop(); /* class */
    POP(t1, STRING);

    char *name;
    name = string_byte_address(state, t1);
    if(unlink(name) == 0) {
      stack_push(Qtrue);
    } else {
      /* TODO translate errno into an exception. */
      stack_push(Qfalse);
    }
    CODE
  end
  
  def gettimeofday
    <<-CODE
    struct timeval tv;

    (void)stack_pop();

    /* don't fill in the 2nd argument here. getting the timezone here
     * this way is not portable and broken anyway.
     */
    gettimeofday(&tv, NULL);

    self = array_new(state, 2);
    array_set(state, self, 0, LL2I(tv.tv_sec));
    array_set(state, self, 1, LL2I(tv.tv_usec));

    stack_push(self);
    CODE
  end

  def strftime
    <<-CODE
    POP(self, REFERENCE);
    POP(t1, ARRAY);
    POP(t2, STRING);

    struct tm tm;
    char *format = NULL;
    char str[MAX_STRFTIME_OUTPUT+1];
    size_t out;

    tm.tm_sec = FIXNUM_TO_INT(array_get(state, t1, 0));
    tm.tm_min = FIXNUM_TO_INT(array_get(state, t1, 1));
    tm.tm_hour = FIXNUM_TO_INT(array_get(state, t1, 2));
    tm.tm_mday = FIXNUM_TO_INT(array_get(state, t1, 3));
    tm.tm_mon = FIXNUM_TO_INT(array_get(state, t1, 4));
    tm.tm_year = FIXNUM_TO_INT(array_get(state, t1, 5));
    tm.tm_wday = FIXNUM_TO_INT(array_get(state, t1, 6));
    tm.tm_yday = FIXNUM_TO_INT(array_get(state, t1, 7));
    tm.tm_isdst = FIXNUM_TO_INT(array_get(state, t1, 8));

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    tm.tm_gmtoff = FIXNUM_TO_INT(array_get(state, t1, 9));
#endif

#ifdef HAVE_STRUCT_TM_TM_ZONE
    tm.tm_zone = string_byte_address(state, array_get(state, t1, 10));
#endif

    format = string_byte_address(state, t2);

    out = strftime(str, MAX_STRFTIME_OUTPUT-1, format, &tm);

    str[MAX_STRFTIME_OUTPUT] = '\\0';
    t3 = string_new2(state, str, out);
    stack_push(t3);
    CODE
  end

  def time_switch
    <<-CODE
    time_t seconds;
    struct tm *tm;

    POP(self, REFERENCE);
    POP(t1, INTEGER);
    t2 = stack_pop();

    if(FIXNUM_P(t1)) {
      seconds = FIXNUM_TO_INT(t1);
    } else {
      seconds = bignum_to_ll(state, t1);
    }

    if(t2 == Qtrue) {
      tm = gmtime(&seconds);
    } else {
      tm = localtime(&seconds);
    }

    t3 = array_new(state, 2);
    array_set(state, t3, 0, I2N(tm->tm_sec));
    array_set(state, t3, 1, I2N(tm->tm_min));
    array_set(state, t3, 2, I2N(tm->tm_hour));
    array_set(state, t3, 3, I2N(tm->tm_mday));
    array_set(state, t3, 4, I2N(tm->tm_mon));
    array_set(state, t3, 5, I2N(tm->tm_year));
    array_set(state, t3, 6, I2N(tm->tm_wday));
    array_set(state, t3, 7, I2N(tm->tm_yday));
    array_set(state, t3, 8, I2N(tm->tm_isdst));

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    array_set(state, t3, 9, I2N(tm->tm_gmtoff));
#else
    array_set(state, t3, 9, Qnil);
#endif

#ifdef HAVE_STRUCT_TM_TM_ZONE
    array_set(state, t3, 10, string_new(state, tm->tm_zone));
#else
    array_set(state, t3, 10, Qnil);
#endif

    stack_push(t3);
    CODE
  end

  def mktime
    <<-CODE
    time_t seconds;
    struct tm tm;
    char *old_tz, old_tz_buf[128];
    OBJECT t5, t6, t7, t8, t9, ret;
    
    old_tz = NULL;
    
    POP(self, REFERENCE);
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    POP(t4, FIXNUM);
    POP(t5, FIXNUM);
    POP(t6, FIXNUM);
    POP(t7, FIXNUM);
    POP(t8, FIXNUM);
    t9 = stack_pop();

    tm.tm_sec = FIXNUM_TO_INT(t1);
    tm.tm_min = FIXNUM_TO_INT(t2);
    tm.tm_hour = FIXNUM_TO_INT(t3);
    tm.tm_mday = FIXNUM_TO_INT(t4);
    tm.tm_mon = FIXNUM_TO_INT(t5) - 1;
    tm.tm_year = FIXNUM_TO_INT(t6) - 1900;

    /* In theory, we'd set the tm_isdst field to FIXNUM_TO_INT(t8).
     * But since that will break on at least FreeBSD,
     * and I don't see the point of filling in that flag at all,
     * we're telling the system here to figure the DST stuff
     * out itself.
     */
    tm.tm_isdst = -1;

    if(t9 == Qtrue) {
      old_tz = getenv("TZ");

      /* We need to save old_tz to our own buffer here, because e.g.
       * FreeBSD's setenv() will manipulate that string directly.
       */
      if(old_tz) {
        strncpy(old_tz_buf, old_tz, sizeof(old_tz_buf));
        old_tz_buf[sizeof(old_tz_buf) - 1] = 0;
      }

      setenv("TZ", "", 1);
    }

    seconds = mktime(&tm);

    if(t9 == Qtrue) {
      if(old_tz) {
        setenv("TZ", old_tz_buf, 1);
      } else {
        unsetenv("TZ");
      }
    }

    ret = array_new(state, 2);
    array_set(state, ret, 0, LL2I(seconds));
    array_set(state, ret, 1, t7);

    stack_push(ret);
    CODE
  end

  def fixnum_to_s(_ = fixnum, t1 = fixnum)
    <<-CODE
    static const char digitmap[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char buf[100];
    char *b = buf + sizeof(buf);

    j = FIXNUM_TO_INT(t1);
    k = FIXNUM_TO_INT(self);
    GUARD( j >= 2 && j <= 36 )

    /* Algorithm taken from 1.8.4 rb_fix2str */
    if(k == 0) {
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
    CODE
  end

  def bignum_to_s
    <<-CODE
    POP(self, BIGNUM);
    POP(t1, FIXNUM);

    stack_push(bignum_to_s(state, self, t1));
    CODE
  end

  def logical_class
    <<-CODE
    /* self is ANY object because object_class knows all. */
    self = stack_pop();
    stack_push(object_class(state, self));
    CODE
  end

  def object_id
    <<-CODE
    self = stack_pop();
    stack_push(UI2N(object_get_id(state, self)));
    CODE
  end

  def hash_set
    <<-CODE
    POP(self, HASH);
    POP(t1, FIXNUM);
    t2 = stack_pop(); // some type of object
    t3 = stack_pop(); // some type of object can we do an object guard?

    hash_add(state, self, FIXNUM_TO_INT(t1), t2, t3);
    stack_push(t3);
    CODE
  end

  def hash_get
    <<-CODE
    POP(self, HASH);
    POP(t1, FIXNUM);
    t2 = stack_pop();
    t3 = hash_find_entry(state, self, FIXNUM_TO_INT(t1));
    // t3 = hash_get_undef(state, self, FIXNUM_TO_INT(t1));
    stack_push(t3);
    CODE
  end

  def hash_object
    <<-CODE
    /* self is ANY object because object_class knows all. */    
    self = stack_pop();
    t1 = UI2N(object_hash_int(state, self));
    stack_push(t1);
    CODE
  end
  
  def hash_delete
    <<-CODE
    POP(self, HASH);
    POP(t1, FIXNUM);

    t2 = hash_delete(state, self, FIXNUM_TO_INT(t1));
    stack_push(t2);
    CODE
  end
  
  def hash_value_set
    <<-CODE
    POP(self, REFERENCE);
    POP(t1, NUMERIC);
    abort();
    CODE
  end

  def symbol_index
    <<-CODE
    POP(self, SYMBOL);

    stack_push(I2N(symbol_to_index(state, self)));
    CODE
  end

  def symbol_lookup
    <<-CODE
    POP(self, STRING);

    stack_push(string_to_sym(state, self));
    CODE
  end

  def dup_into
    <<-CODE
    POP(self, REFERENCE);
    POP(t1, REFERENCE);
    POP(t2, FIXNUM);

    int start = FIXNUM_TO_INT(t2);
    for(k = start, j = 0; 
        k < NUM_FIELDS(t1) && j < NUM_FIELDS(self); 
        k++, j++) {
      SET_FIELD(self, j, NTH_FIELD(t1, k));
    }
    // object_copy_fields_from(state, t1, self, j, NUM_FIELDS(self));
    // gc_zone selfgc = self->gc_zone;
    object_copy_nongc_flags(self, t1);
    // self->flags2 = (t1->flags2 & ZONE_MASK) | GC_ZONE(self);
    stack_push(t1);
    CODE
  end
    
  def bytes_dup_into
    <<-CODE
    POP(self, REFERENCE);
    POP(t1,   REFERENCE);

    GUARD(_object_stores_bytes(self));
    GUARD(_object_stores_bytes(t1));
    
    k = bytearray_bytes(state, self);
    j = bytearray_bytes(state, t1);
    
    if(j < k) { k = j; }
      
    memcpy(bytearray_byte_address(state, t1), 
           bytearray_byte_address(state, self), k);
           
    stack_push(t1);
    CODE
  end

  def object_dup
    <<-CODE
    t1 = stack_pop();
    if(REFERENCE_P(t1)) {
      j = NUM_FIELDS(t1);
      t2 = NEW_OBJECT(object_class(state, t1), j);
      if(t1->StoresBytes) {
        memcpy(object_byte_start(state, t2), 
               object_byte_start(state, t1), SIZE_OF_BODY(t1));
        t2->StoresBytes = 1;
      } else {
        for(k = 0; k < j; k++) {
          SET_FIELD(t2, k, NTH_FIELD(t1, k));
        }
      }
      stack_push(t2);
      object_copy_ivars(state, t1, t2);
      cpu_perform_hook(state, c, t2, state->global->sym_init_copy, t1);
    } else {
      stack_push(t1);
    }
    CODE
  end
  
  def object_clone
    <<-CODE
    POP(t1, REFERENCE);
    j = NUM_FIELDS(t1);
    t2 = NEW_OBJECT(object_class(state, t1), j);
    if(t1->StoresBytes) {
      memcpy(object_byte_start(state, t2), 
             object_byte_start(state, t1), SIZE_OF_BODY(t1));
      t2->StoresBytes = 1;
    } else {
      for(k = 0; k < j; k++) {
        SET_FIELD(t2, k, NTH_FIELD(t1, k));
      }
    }
    stack_push(t2);
    object_copy_ivars(state, t1, t2);
    object_copy_metaclass(state, t1, t2);
    cpu_perform_hook(state, c, t2, state->global->sym_init_copy, t1);
    
    CODE
  end
  
  
  def fastctx_dup
    <<-CODE
    POP(self, REFERENCE);
    GUARD(ISA(self, state->global->fastctx));
    
    stack_push(methctx_dup(state, self));
    CODE
  end

  def tuple_shifted
    <<-CODE
    POP(self, TUPLE);
    POP(t1, FIXNUM);

    j = FIXNUM_TO_INT(t1);
    if (!j)
      t2 = self;
    else {
      t2 = tuple_new(state, NUM_FIELDS(self) + j);
      object_copy_fields_shifted(state, self, t2, j);
    }

    stack_push(t2);
    CODE
  end

  def get_byte
    <<-CODE
    self = stack_pop(); GUARD( object_stores_bytes_p(state, self) )
    POP(t1, FIXNUM); /* index */

    unsigned char *indexed;
    j = FIXNUM_TO_INT(t1);
    k = bytearray_bytes(state, self);

    GUARD( j >= 0 && j < k )
    indexed = (unsigned char*)bytearray_byte_address(state, self);
    indexed += j;
    stack_push(UI2N(*indexed));
    CODE
  end

  def set_byte
    <<-CODE
    self = stack_pop(); GUARD( object_stores_bytes_p(state, self) )
    POP(t1, FIXNUM); /* index */
    POP(t2, FIXNUM); /* value */

    unsigned char *indexed;
    j = FIXNUM_TO_INT(t1);
    k = bytearray_bytes(state, self);

    GUARD( j >= 0 && j < k )
    indexed = (unsigned char*)bytearray_byte_address(state, self);
    indexed += j;
    t2 = UI2N(*indexed = FIXNUM_TO_INT(t2));
    stack_push(t2);
    CODE
  end
    
  def fetch_bytes
    <<-CODE
    self = stack_pop(); GUARD( object_stores_bytes_p(state, self) )
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);

    char *source, *dest;
    int num;
    j = FIXNUM_TO_INT(t1);
    k = FIXNUM_TO_INT(t2);
    m = bytearray_bytes(state, self);

    num = abs(j - k);

    GUARD( m >= num )
    GUARD( k >= 0 )
    GUARD( j >= 0 && j <= m )
    t3 = bytearray_new(state, k+1);
    source = (char*)bytearray_byte_address(state, self);
    dest = (char*)bytearray_byte_address(state, t3);
    source += j;
    memcpy(dest, source, k);
    dest[k] = 0;
    stack_push(t3);
    CODE
  end
  
  def move_bytes
    <<-CODE
    char *data, *source, *dest;
    int total, offset, start, count;
    
    self = stack_pop(); 
    GUARD(object_stores_bytes_p(state, self));
    
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    
    start  = FIXNUM_TO_INT(t1);
    count  = FIXNUM_TO_INT(t2);
    offset = FIXNUM_TO_INT(t3);
    
    total = bytearray_bytes(state, self);
        
    GUARD(start + count + offset < total);
    GUARD(offset >= 0);
    GUARD(count >= 0);
    
    data = (char*)bytearray_byte_address(state, self);
    source = data;
    dest = data;
    
    source += start;
    dest += offset;
        
    memmove((void*)dest, (void*)source, count);
    
    stack_push(t2);    
    CODE
  end

  def compare_bytes
    <<-CODE
    self = stack_pop(); GUARD( object_stores_bytes_p(state, self) )
    t1 = stack_pop(); GUARD( object_stores_bytes_p(state, t1) )

    j = bytearray_bytes(state, self);
    k = bytearray_bytes(state, t1);
    if(j < k) {
      stack_push(I2N(-1));
    } else if(j > k) {
      stack_push(I2N(1));
    } else if(j == 0) {
      stack_push(I2N(0));
    } else {
      k = memcmp(bytearray_byte_address(state, self), bytearray_byte_address(state, t1), j);
      if(k > 1) {k = 1;} // Normalize return
      if(k < -1) {k = -1;} // values for <=> method.
      stack_push(I2N(k));
    }
    CODE
  end

  def bytearray_size
    <<-CODE
    self = stack_pop(); GUARD( object_stores_bytes_p(state, self) )

    j = bytearray_bytes(state, self);
    stack_push(I2N(j));
    CODE
  end
  
  def load_file
    <<-CODE
    self = stack_pop();
    POP(t1, STRING);
    POP(t2, FIXNUM);

    char *path = string_byte_address(state, t1);
    t2 = cpu_unmarshal_file(state, path, FIXNUM_TO_INT(t2));
    stack_push(t2);
    CODE
  end
  
  def activate_as_script
    <<-CODE
    self = stack_pop();
    GUARD(RISA(self, cmethod));

    cpu_run_script(state, c, self);
    stack_push(Qtrue);
    CODE
  end
  
  def stat_file
    <<-CODE
    struct stat sb = {0};
    self = stack_pop();
    POP(t1, STRING);
    t2 = stack_pop();

    char *path = string_byte_address(state, t1);
    if (RTEST(t2)) {
      j = stat(path, &sb);
    } else {
      j = lstat(path, &sb);
    }

    if(j != 0) {
      stack_push(Qfalse);
    } else {
      t2 = NEW_OBJECT(self, 12);
      tuple_put(state, t2, 0, I2N((int)sb.st_ino));
      tuple_put(state, t2, 1, I2N((int)sb.st_mode));

      switch(sb.st_mode & S_IFMT) {
        case S_IFIFO:     // named pipe
          t3 = string_to_sym(state, string_new(state, "fifo"));
          break;
        case S_IFCHR:     // character special
          t3 = string_to_sym(state, string_new(state, "char"));
                break;
        case S_IFDIR:     // directory
          t3 = string_to_sym(state, string_new(state, "dir"));
                break;
        case S_IFBLK:     // block special
          t3 = string_to_sym(state, string_new(state, "block"));
                break;
        case S_IFREG:     // regular file
          t3 = string_to_sym(state, string_new(state, "file"));
                break;
        case S_IFLNK:     // symbolic link
          t3 = string_to_sym(state, string_new(state, "link"));
                break;
        case S_IFSOCK:    // socket
          t3 = string_to_sym(state, string_new(state, "socket"));
                break;
        #ifdef S_IFWHT
        case S_IFWHT:     // whiteout
          t3 = string_to_sym(state, string_new(state, "whiteout"));
                break;
        #endif
        default:
          t3 = string_to_sym(state, string_new(state, "file"));
      }
      
      tuple_put(state, t2, 2, t3);
      tuple_put(state, t2, 3, I2N((int)sb.st_uid));
      tuple_put(state, t2, 4, I2N((int)sb.st_gid));
      tuple_put(state, t2, 5, I2N((int)sb.st_size));
      tuple_put(state, t2, 6, I2N((int)sb.st_blocks));
      tuple_put(state, t2, 7, LL2I((long)sb.st_atime));
      tuple_put(state, t2, 8, LL2I((long)sb.st_mtime));
      tuple_put(state, t2, 9, LL2I((long)sb.st_ctime));
      tuple_put(state, t2, 10, t1);
      tuple_put(state, t2, 11, UI2N((unsigned long)sb.st_blksize));
    
      stack_push(t2);
    }
    CODE
  end
  
  def process_exit
    <<-CODE
    (void)stack_pop();
    POP(t1, FIXNUM);

    if(state->gc_stats) {
      printf("[GC M %6dK total]\\n", state->om->ms->allocated_bytes / 1024);
    }
    if(current_machine->sub) {
      environment_exit_machine(); 
    } else {
      exit(FIXNUM_TO_INT(t1));
    }
    CODE
  end
  
  def micro_sleep
    <<-CODE
    struct timespec ts;
    (void)stack_pop();
    POP(t1, FIXNUM);

    j = FIXNUM_TO_INT(t1);
    ts.tv_sec = j / 1000000;
    ts.tv_nsec = (j % 1000000) * 1000;
      
    if(!nanosleep(&ts, NULL)) {
      t2 = Qfalse;
    } else {
      t2 = Qtrue;
    }

    stack_push(t2);
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
    
    cpu_activate_context(state, c, self, t1, 0);
    CODE
  end
  
  def context_sender
    <<-CODE
    self = stack_pop();
    t1 = FASTCTX(self)->sender;
    
    if(t1 != Qnil) {
      methctx_reference(state, t1);
    }
    
    stack_push(t1);
    CODE
  end
  
  def string_to_sexp
    <<-CODE
    {
      bstring contents;
      const char *name;

      self = stack_pop();
      POP(t1, STRING);
      POP(t2, FIXNUM);
      t3 = stack_pop();

      contents = cstr2bstr(string_byte_address(state, self));
      name = string_byte_address(state, t1);
      t1 = syd_compile_string(state, name, contents, FIXNUM_TO_INT(t2), RTEST(t3));
      bdestroy(contents);
      stack_push(t1);
    }
    CODE
  end
  
  def file_to_sexp
    <<-CODE
    FILE *file;
    char *name;

    self = stack_pop();
    POP(t1, STRING); /* The filename */
    t2 = stack_pop();

    name = string_byte_address(state, t1);
    file = fopen(name, "r");

    if(!file) {
      _ret = FALSE;
    } else {
      t1 = syd_compile_file(state, name, file, 1, RTEST(t2));
      fclose(file);
      stack_push(t1);
    }
    CODE
  end

  def terminal_raw
    <<-CODE
    {
      struct termios ts;
      int err;

      (void)stack_pop();

      if (NULL == state->termios) {
        if (!isatty(STDOUT_FILENO)) {
          stack_push(Qfalse);
          break;
        }

        /* HACK: this memory is never freed */
        state->termios = ALLOC(struct termios);

        if (NULL == state->termios) {
          stack_push(Qfalse);
          break;
        }

        err = tcgetattr(STDOUT_FILENO, state->termios);

        if (err == -1) { /* TODO: handle errno */
          XFREE(state->termios);
          stack_push(Qfalse);
          break;
        }
      }

      err = tcgetattr(STDOUT_FILENO, &ts);

      if (err == -1) { /* TODO: handle errno */
        stack_push(Qfalse);
        break;
      }

      ts.c_lflag &= ~ICANON; /* -icanon */
      ts.c_lflag &= ~ISIG;   /* -isig */
      ts.c_lflag &= ~ECHO;   /* -echo */
      ts.c_cc[VMIN] = 1;     /* min 1 */

      err = tcsetattr(STDOUT_FILENO, TCSANOW, &ts);

      if (err == -1) { /* TODO: handle errno */
        stack_push(Qfalse);
        break;
      }

      t3 = tuple_new2(state, 4,
        I2N(ts.c_cc[VERASE]), I2N(ts.c_cc[VKILL]),
        I2N(ts.c_cc[VQUIT]),  I2N(ts.c_cc[VINTR])
      );
      stack_push(t3);
    }
    CODE
  end

  def terminal_normal
    <<-CODE
    {
      (void)stack_pop();
      
      if (NULL == state->termios) {
        stack_push(Qfalse);
      } else if (isatty(STDOUT_FILENO)) {
        int err;

        err = tcsetattr(STDOUT_FILENO, TCSANOW, state->termios);

        if (err == -1) {
          stack_push(Qfalse);
        } else {
          stack_push(Qtrue);
        }
      } else {
        stack_push(Qfalse);
      }
    }
    CODE
  end

  def regexp_new
    <<-CODE
    self = stack_pop();
    POP(t1, STRING);
    t2 = stack_pop();
    t3 = regexp_new(state, t1, t2);
    if(STRING_P(t3)) {
      cpu_raise_exception(state, c, cpu_new_exception(state, c, state->global->exc_rex, string_byte_address(state, t3)));
    }
    t3->klass = self;   /* Subclasses */
    stack_push(t3);
    CODE
  end

  def regexp_match
    <<-CODE
    self = stack_pop();
    POP(t1, STRING);
    stack_push(regexp_match(state, self, t1));
    CODE
  end

  def regexp_match_start
    <<-CODE
    self = stack_pop();
    POP(t1, STRING);
    POP(t2, FIXNUM);
    stack_push(regexp_match_start(state, self, t1, t2));
    CODE
  end

  def regexp_match_region
    <<-CODE
    self = stack_pop();
    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    t4 = stack_pop();
    OBJECT t5 = regexp_match_region(state, self, t1, t2, t3, t4);
    stack_push(t5);
    CODE
  end

  def regexp_scan
    <<-CODE
    self = stack_pop();
    POP(t1, STRING);
    stack_push(regexp_scan(state, self, t1));
    CODE
  end  

  def regexp_options
    <<-CODE
    self = stack_pop();
    stack_push(regexp_options(state, self));
    CODE
  end
  
  def gc_start
    <<-CODE
    (void)stack_pop();
    t1 = stack_pop();
    if(RTEST(t1)) {
      state->om->collect_now = OMCollectYoung;
    } else {
      state->om->collect_now = OMCollectYoung | OMCollectMature;
    }
    stack_push(Qtrue);
    CODE
  end
    
  def get_ivar
    <<-CODE
    self = stack_pop();
    t1 = NTH_FIELD(mo, 4);
    stack_push(object_get_ivar(state, self, t1));
    CODE
  end
  
  def set_ivar
    <<-CODE
    self = stack_pop();
    t1 = NTH_FIELD(mo, 4);
    t2 = stack_pop();
    object_set_ivar(state, self, t1, t2);
    stack_push(t2);
    CODE
  end

  # field 4: method to call
  # field 5: object to call method on
  # field 6: whether or not we need to retain 'self'
  def dispatch_as_method
    <<-CODE
      t1 = NTH_FIELD(mo, 4);
      t2 = NTH_FIELD(mo, 5);
      t3 = NTH_FIELD(mo, 6);
      if(Qtrue == t3) {
        num_args++;
      } else {
       (void)stack_pop();
      }
      cpu_unified_send(state, c, t2, t1, num_args, Qnil);
    CODE
  end
  
  def set_index
    <<-CODE
    self = stack_pop();
    GUARD(INDEXED(self));

    j = FIXNUM_TO_INT(NTH_FIELD(mo, 4));
    SET_FIELD(self, j, stack_pop());
    stack_push(NTH_FIELD(self, j));  
    CODE
  end
  
  def get_index
    <<-CODE
    self = stack_pop();
    GUARD(INDEXED(self));

    j = FIXNUM_TO_INT(NTH_FIELD(mo, 4));
    stack_push(NTH_FIELD(self, j));
    CODE
  end

  def fixnum_modulo(_ = fixnum, t1 = fixnum)
    <<-CODE
    GUARD( FIXNUM_TO_INT(t1) != 0 ) // no divide by zero

    t3 = fixnum_divmod(state, self, t1);
    stack_push(array_get(state, t3, 1));
    CODE
  end
  
  def marshal_object
    <<-CODE
    (void)stack_pop(); /* class */
    t1 = stack_pop();
    POP(t2, FIXNUM);
    stack_push(cpu_marshal(state, t1, FIXNUM_TO_INT(t2)));
    CODE
  end
  
  def unmarshal_object
    <<-CODE
    (void)stack_pop(); /* class */
    POP(t1, STRING);
    POP(t2, FIXNUM);

    stack_push(cpu_unmarshal(state, (uint8_t*)string_byte_address(state, t1), FIXNUM_TO_INT(string_get_bytes(t1)), FIXNUM_TO_INT(t2)));
    CODE
  end
  
  def marshal_to_file
    <<-CODE
    char *_path;
    (void)stack_pop();
    t1 = stack_pop();
    POP(t2, STRING);
    POP(t3, FIXNUM);

    _path = string_byte_address(state, t2);
    stack_push(cpu_marshal_to_file(state, t1, _path, FIXNUM_TO_INT(t3)));
    CODE
  end
  
  def unmarshal_from_file
    <<-CODE
    char *_path;
    (void)stack_pop(); /* class */
    POP(t1, STRING);
    POP(t2, FIXNUM);

    _path = string_byte_address(state, t1);
    stack_push(cpu_unmarshal_file(state, _path, FIXNUM_TO_INT(t2)));
    CODE
  end
  
  def archive_files
    <<-CODE
    char *path;
    (void)stack_pop(); /* blah */
    POP(t1, STRING);

    path = string_byte_address(state, t1);
    stack_push(archive_list_files(state, path));
    CODE
  end
  
  def archive_get_file
    <<-CODE
    char *path, *file;
    (void)stack_pop();
    POP(t1, STRING);
    POP(t2, STRING);

    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    stack_push(archive_get_file(state, path, file));
    CODE
  end
  
  def archive_get_object
    <<-CODE
    char *path, *file;
    (void)stack_pop();
    POP(t1, STRING);
    POP(t2, STRING);
    POP(t3, FIXNUM);

    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    stack_push(archive_get_object(state, path, file, FIXNUM_TO_INT(t3)));
    CODE
  end
  
  def archive_add_file
    <<-CODE
    char *path, *file, *data;
    (void)stack_pop();
    POP(t1, STRING);
    POP(t2, STRING);
    POP(t3, STRING);
    
    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    data = string_byte_address(state, t3);
    
    t1 = archive_add_file(state, path, file, data);
    stack_push(t1);
    CODE
  end
  
  def archive_add_object
    <<-CODE
    char *path, *file;
    (void)stack_pop();
    POP(t1, STRING);
    POP(t2, STRING);
    t3 = stack_pop();
    POP(t4, FIXNUM);
    
    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    
    t1 = archive_add_object(state, path, file, t3, FIXNUM_TO_INT(t4));
    stack_push(t1);
    CODE
  end
  
  def archive_delete_file
    <<-CODE
    (void)stack_pop();
    POP(t1, STRING);
    POP(t2, FIXNUM);

    t3 = archive_delete_file(state, string_byte_address(state, t1), FIXNUM_TO_INT(t2));
    stack_push(t3);
    CODE
  end  
  
  # FIXME: get rid of bignum awareness and use coerce
  # i.e. t1 = fixnum
  def fixnum_and(_ = fixnum, t1 = integer)
    <<-CODE
    if( FIXNUM_P(t1) ) {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j & k;
      stack_push(I2N(m));
    } else {
      stack_push(bignum_and(state, bignum_new(state, FIXNUM_TO_INT(self)), t1));
    }
    CODE
  end

  # FIXME: get rid of bignum awareness and use coerce
  # i.e. t1 = fixnum
  def fixnum_or(_ = fixnum, t1 = integer)
    <<-CODE
    if(FIXNUM_P(t1)) {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j | k;
      stack_push(I2N(m));
    } else {
      stack_push(bignum_or(state, bignum_new(state, FIXNUM_TO_INT(self)), t1));
    }
    CODE
  end

  # FIXME: get rid of bignum awareness and use coerce
  # i.e. t1 = fixnum
  def fixnum_xor(_ = fixnum, t1 = integer)
    <<-CODE
    if(FIXNUM_P(t1)) {
      j = FIXNUM_TO_INT(self);
      k = FIXNUM_TO_INT(t1);
      m = j ^ k;
      stack_push(I2N(m));
    } else {
      stack_push(bignum_xor(state, bignum_new(state, FIXNUM_TO_INT(self)), t1));
    }
    CODE
  end

  def fixnum_invert(_ = fixnum)
    <<-CODE
      j = FIXNUM_TO_INT(self);
      stack_push(I2N(~j));
    CODE
  end

  def fixnum_neg(_ = fixnum)
    <<-CODE
      j = FIXNUM_TO_INT(self);
      stack_push(I2N(-j));
    CODE
  end

  def fixnum_right_shift(_ = fixnum, t1 = fixnum)
    <<-CODE
    long value;
    int  width;
    value = FIXNUM_TO_INT(self);
    width = FIXNUM_TO_INT(t1);

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

    CODE
  end

  def fixnum_left_shift(_ = fixnum, t1 = fixnum)
    <<-CODE

    long value;
    int  width;
    value = FIXNUM_TO_INT(self);
    width = FIXNUM_TO_INT(t1);

    value <<= width;
    t2 = I2N(value);
    stack_push(t2);

    CODE
  end

  def bignum_new
    <<-CODE
    (void)stack_pop();
    POP(t1, FIXNUM);
    stack_push(bignum_new(state, FIXNUM_TO_INT(t1)));
    CODE
  end
  
  def bignum_to_float
    <<-CODE
    POP(self, BIGNUM);

    stack_push(float_new(state, bignum_to_double(state, self)));
    CODE
  end

  def bignum_and(_ = bignum, t1 = integer)
    <<-CODE
    stack_push(bignum_and(state, self, t1));
    CODE
  end

  def bignum_or
    <<-CODE
    self = stack_pop();
    POP(t1, INTEGER);

    stack_push(bignum_or(state, self, t1));
    CODE
  end

  def bignum_xor
    <<-CODE
    self = stack_pop();
    POP(t1, INTEGER);

    stack_push(bignum_xor(state, self, t1));
    CODE
  end

  def bignum_neg
    <<-CODE
    POP(self, INTEGER);

    stack_push(bignum_neg(state, self));
    CODE
  end

  def bignum_invert
    <<-CODE
    POP(self, INTEGER);

    stack_push(bignum_invert(state, self));
    CODE
  end
  
  def numeric_coerce
    <<-CODE
    POP(self, INTEGER);
    POP(t1, INTEGER);

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
    CODE
  end
  
  def bignum_compare(_ = bignum, t1 = bignum)
    <<-CODE
    stack_push(bignum_compare(state, self, t1));
    CODE
  end
  
  def fixnum_to_f(_ = fixnum)
    <<-CODE
    stack_push(float_new(state, FIXNUM_TO_DOUBLE(self)));
    CODE
  end
  
  def string_to_f
    <<-CODE
    POP(self, STRING);

    stack_push(float_new(state, string_to_double(state, self)));
    CODE
  end
  
  def fixnum_divmod(_ = fixnum, t1 = fixnum)
    <<-CODE
    GUARD( FIXNUM_TO_INT(t1) != 0 ) // no divide by zero
    stack_push(fixnum_divmod(state, self, t1));
    CODE
  end
  
  def bignum_left_shift
    <<-CODE
    POP(self, BIGNUM);
    POP(t1, FIXNUM);

    stack_push(bignum_left_shift(state, self, t1));
    CODE
  end

  def bignum_right_shift
    <<-CODE
    POP(self, BIGNUM);
    POP(t1, FIXNUM);

    stack_push(bignum_right_shift(state, self, t1));
    CODE
  end

  def find_method
    <<-CODE
    self = stack_pop();
    t1 = stack_pop(); // must be sym already
    t1 = exported_cpu_find_method(state, c, self, t1, &t2);
    t3 = tuple_new(state, 2);
    tuple_put(state, t3, 0, t2);
    tuple_put(state, t3, 1, t1);
    stack_push(t3);
    CODE
  end
  
  def bignum_divmod
    <<-CODE
    self = stack_pop();
    GUARD(RISA(self, bignum));
    
    t1  =  stack_pop();
    GUARD(RISA(t1,   bignum));    
    
    // no divide by zero
    GUARD(!bignum_is_zero(state, t1));

    stack_push(bignum_divmod(state, self, t1));
    CODE
  end
  
  def object_taint
    <<-CODE
    self = stack_pop();
    object_set_tainted(state, self);
    stack_push(self);
    CODE
  end
  
  def object_tainted_p
    <<-CODE
    self = stack_pop();
    stack_push(object_tainted_p(state, self) ? Qtrue : Qfalse);
    CODE
  end
  
  def object_untaint
    <<-CODE
    self = stack_pop();
    object_set_untainted(state, self);
    stack_push(self);
    CODE
  end
  
  def object_freeze
    <<-CODE
    self = stack_pop();
    object_set_frozen(state, self);
    stack_push(self);
    CODE
  end
  
  def object_frozen_p
    <<-CODE
    self = stack_pop();
    stack_push(object_frozen_p(state, self) ? Qtrue : Qfalse);
    CODE
  end
  
  def fastctx_get_field
    <<-CODE
    int i;
    struct fast_context *fc;
    t1 = stack_pop();
    GUARD(RISA(t1, fastctx));

    fc = FASTCTX(t1);
    i = FIXNUM_TO_INT(stack_pop());
    switch(i) {
      case 0:
        if(!NIL_P(fc->sender)) {
          methctx_reference(state, fc->sender);
        }
        stack_push(fc->sender);
        break;
      case 1:
        stack_push(I2N(fc->ip));
        break;
      case 2:
        stack_push(I2N(fc->sp));
        break;
      case 3:
        stack_push(fc->block);
        break;
      case 5:
        stack_push(fc->method);
        break;
      case 6:
        stack_push(fc->literals);
        break;
      case 7:
        stack_push(fc->self);
        break;
      case 8:
        stack_push(fc->locals);
        break;
      case 9:
        stack_push(I2N(fc->argcount));
        break;
      case 10:
        stack_push(fc->name);
        break;
      case 11:
        stack_push(fc->method_module);
        break;
      case 12:
        stack_push(I2N(fc->flags));
        break;
      case 13:
        stack_push(I2N(fc->fp));
        break;
      default:
        stack_push(Qnil);
    }
    CODE
  end
  
  def fastctx_set_field
    <<-CODE
    int i;
    struct fast_context *fc;
    t1 = stack_pop();
    i = FIXNUM_TO_INT(stack_pop());
    t2 = stack_pop();
    
    GUARD(RISA(t1, fastctx));

    fc = FASTCTX(t1);
    switch(i) {
      case 0:
        fc->sender = t2;
        break;
      case 1:
        GUARD(FIXNUM_P(t2));
        fc->ip = FIXNUM_TO_INT(t2);
        break;
      case 2:
        GUARD(FIXNUM_P(t2));
        fc->sp = FIXNUM_TO_INT(t2);
        break;
      case 3:
        fc->block = t2;
        break;
      case 5:
        fc->method = t2;
        break;
      case 6:
        fc->literals = t2;
        break;
      case 7:
        fc->self = t2;
        break;
      case 8:
        fc->locals = t2;
        break;
      case 9:
        GUARD(FIXNUM_P(t2));
        fc->argcount = FIXNUM_TO_INT(t2);
        break;
      case 10:
        fc->name = t2;
        break;
      case 11:
        fc->method_module = t2;
        break;
      case 12:
        if(NIL_P(t2)) {
          fc->flags = 0;
        } else if(FIXNUM_P(t2)) {
          fc->flags |= FIXNUM_TO_INT(t2);
        }
        break;
      default:
        _ret = FALSE;
    }

    stack_push(t2);
    CODE
  end

  def fastctx_reload_method
    <<-CODE
    struct fast_context *fc;
    t1 = stack_pop();

    GUARD(RISA(t1, fastctx));

    fc = FASTCTX(t1);
    if(fc->method->obj_type == CMethodType) {
      fc->data = BYTEARRAY_ADDRESS(cmethod_get_compiled(fc->method));
    }

    stack_push(Qtrue);
    CODE
  end

  def fastctx_set_iseq
    <<-CODE
    struct fast_context *fc;
    t1 = stack_pop();
    t2 = stack_pop();

    GUARD(RISA(t1, fastctx));
    GUARD(RISA(t2, bytearray));

    fc = FASTCTX(t1);
    if(fc->method->obj_type == CMethodType) {
      cpu_compile_instructions(state, t2);
      fc->data = BYTEARRAY_ADDRESS(t2);
    }

    stack_push(Qtrue);
    CODE
  end

  def vm_stats
    <<-CODE
#ifdef TRACK_STATS
    t1 = tuple_new(state, 7);
    tuple_put(state, t1, 0, I2N(state->cache_hits));
    tuple_put(state, t1, 1, I2N(state->cache_misses));
    tuple_put(state, t1, 2, I2N(state->cache_used));
    tuple_put(state, t1, 3, I2N(state->cache_collisions));
    tuple_put(state, t1, 4, I2N(state->cache_inline_hit));
    tuple_put(state, t1, 5, I2N(state->cache_inline_stale));
    tuple_put(state, t1, 6, I2N(state->cache_inline_const_hit));
    stack_push(t1);
#else
    stack_push(Qfalse);
#endif
    CODE
  end
  
  def nmethod_call
    <<-CODE
    t1 = stack_pop();
    cpu_flush_ip(c);
    cpu_flush_sp(c);
    cpu_save_registers(state, c, num_args);
    t1 = nmc_new(state, mo, c->active_context, t1, method_name, num_args);
    nmc_activate(state, c, t1, Qnil, FALSE);
    CODE
  end

  def nfunc_call
    <<-CODE
    /* Get rid of the module */
    (void)stack_pop();
    /* The definition of beauty. Simplicity. To call a native function, there is no work 
       to be done. The stub contains all the serialization code. 
       
       That being said, this might get more complicated when callbacks are supported. */
    ffi_call(state, c, nfunc_get_data(mo));
    CODE
  end
  
  def nfunc_call_object
    <<-CODE
    /* The definition of beauty. Simplicity. To call a native function, there is no work 
       to be done. The stub contains all the serialization code. 
       
       That being said, this might get more complicated when callbacks are supported. */
    ffi_call(state, c, nfunc_get_data(stack_pop()));
    CODE
  end
  
  def nfunc_add
    <<-CODE
    (void)stack_pop();
    POP(t1, STRING_OR_NIL);
    POP(t2, STRING);
    POP(t3, ARRAY);
    POP(t4, FIXNUM);
    stack_push(ffi_function_create(state, t1, t2, t3, t4));
    CODE
  end
  
  def load_library
    <<-CODE
    (void)stack_pop(); /* self */
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(subtend_load_library(state, c, t1, t2));
    CODE
  end
  
  def dir_glob
    <<-CODE
    glob_t gd;
    char *pat;
    int flags = GLOB_NOSORT | GLOB_BRACE;

    (void)stack_pop();
    POP(t1, STRING);
    POP(t2, FIXNUM);
    
    /* TODO: use t2. */
    
    pat = string_byte_address(state, t1);
    k = glob(pat, flags, NULL, &gd);
    t2 = array_new(state, gd.gl_pathc);
    for(j = 0; j < gd.gl_pathc; j++) {
      array_set(state, t2, j, string_new(state, gd.gl_pathv[j]));
    }
    globfree(&gd);
    stack_push(t2);
    CODE
  end
  
  def dir_chdir
    <<-CODE
    char *path;
    (void)stack_pop();
    POP(t1, STRING);
    
    path = string_byte_address(state, t1);
    if(!chdir(path)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def yield_gdb
    <<-CODE
      (void)stack_pop();
      self = stack_pop();
      *((char*)4) = 1; /* cause a SIGBUS */
      stack_push(Qtrue);
    CODE
  end
  
  def make_weak_ref
    <<-CODE
    (void)stack_pop();
    POP(t1, REFERENCE);
    
    stack_push(object_make_weak_ref(state, t1));
    CODE
  end
  
  def gc_collect_references
    <<-CODE
    (void)stack_pop();
    POP(self, REFERENCE);
    
    stack_push(object_memory_collect_references(state, state->om, self));
    CODE
  end

  def task_dup
    <<-CODE
    self = stack_pop();
    
    /* This is a little contrived so the dup'd task has
       it stack setup properly. */
    
    stack_push(Qnil);
    
    if(RISA(self, task)) {
      t1 = cpu_task_dup(state, c, self);
    } else {
      t1 = cpu_task_dup(state, c, Qnil);
    }
    
    cpu_stack_set_top(state, c, t1);    
    CODE
  end
  
  def task_set_current
    <<-CODE
    (void)stack_pop(); /* class */
    self = stack_pop();
    
    GUARD( RISA(self, task) );
    
    stack_push(Qnil);    
    cpu_task_select(state, c, self);
    CODE
  end
  
  def task_associate
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    
    GUARD( RISA(self, task) );
    GUARD( RISA(t1,   blokenv) );
    
    stack_push(cpu_task_associate(state, c, self, t1));
    CODE
  end
  
  def task_current
    <<-CODE
    (void)stack_pop(); /* class */
    stack_push(c->current_task);
    CODE
  end
  
  def task_at
    <<-CODE
    struct cpu_task *task;
    self = stack_pop();
    t1 =   stack_pop();
    
    GUARD( RISA(self, task) );
    GUARD( FIXNUM_P(t1) );
    
    task = (struct cpu_task*)BYTES_OF(self);
    k = FIXNUM_TO_INT(t1);
    
    switch(k) {
    case 0:
      t2 = task->main;
      break;
    case 1:
      t2 = task->active_context;
      if(REFERENCE_P(t2)) methctx_reference(state, t2);
      break;
    default:
      t2 = Qnil;
    }
    
    stack_push(t2);
    CODE
  end
  
  def task_set_debugging
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();

    GUARD(t1 == Qnil || RISA(t1, channel));
    GUARD(t2 == Qnil || RISA(t2, channel));
    GUARD(RISA(self, task));

    if(self == c->current_task) {
      c->debug_channel = t1;
      c->control_channel = t2;
    } else {
      cpu_task_set_debugging(state, self, t1, t2);
    }
    stack_push(Qtrue);
    CODE
  end

  def task_debug_channel
    <<-CODE
    struct cpu_task *task;
    self = stack_pop();
    
    GUARD(RISA(self, task));
    
    task = (struct cpu_task*)BYTES_OF(self);

    stack_push(task->debug_channel);
    CODE
  end
  
  def task_control_channel
    <<-CODE
    struct cpu_task *task;
    self = stack_pop();
    
    GUARD(RISA(self, task));
    
    task = (struct cpu_task*)BYTES_OF(self);

    stack_push(task->control_channel);
    CODE
  end
  
  def task_stack_size
    <<-CODE
    struct cpu_task *task;
    self = stack_pop(); /* self */

    GUARD(RISA(self, task));

    task = (struct cpu_task*)BYTES_OF(self);
    t1 = I2N(task->sp_ptr - task->stack_top);

    stack_push(t1);
    CODE
  end

  def task_get_stack_value
    <<-CODE
    struct cpu_task *task;
    self = stack_pop(); /* self */
    t1 = stack_pop();

    GUARD(RISA(self, task));
    GUARD(FIXNUM_P(t1));

    task = (struct cpu_task*)BYTES_OF(self);
    int idx = FIXNUM_TO_INT(t1);
    GUARD(idx >=0 && idx < (task->sp_ptr - task->stack_top));

    t2 = *(task->sp_ptr - idx);

    stack_push(t2);
    CODE
  end

  def task_raise
    <<-CODE
    self = stack_pop();
    GUARD( RISA(self, task) );
    
    t1 = stack_pop();
    
    /* The return value */
    stack_push(Qnil);
    
    /* This is conditional because task_select can decide that it's not
       not possible to select this task, in which case it handles letting
       the user know this on it's own. */
        
    if(cpu_task_select(state, c, self)) {
      cpu_raise_exception(state, c, t1);
    }
    CODE
  end
  
  def thread_raise
    <<-CODE
    self = stack_pop();
    GUARD( RISA(self, thread) );
    
    t1 = stack_pop();

    if(!cpu_thread_alive_p(state, self)) {
      stack_push(Qfalse);
    } else {

      /* The return value */
      stack_push(Qtrue);

      cpu_thread_schedule(state, c->current_thread);    
      cpu_thread_force_run(state, c, self);

      methctx_reference(state, c->active_context);
      exception_set_context(t1, c->active_context);

      cpu_raise_exception(state, c, t1);
    }
    CODE
  end
  
  def channel_new
    <<-CODE
    (void)stack_pop();
    stack_push(cpu_channel_new(state));
    CODE
  end
  
  def channel_send
    <<-CODE
    self = stack_pop();
    
    GUARD(RISA(self, channel));
    
    t1 = stack_pop();
    stack_push(cpu_channel_send(state, c, self, t1));
    CODE
  end
  
  def channel_receive
    <<-CODE
    self = stack_pop();
    
    GUARD(RISA(self, channel));
    
    cpu_channel_receive(state, c, self, c->current_thread);
    /* Don't touch the stack as we may be in a different task at this
       point. The original task's stack is updated when the channel
       is written to and the task restored. */
    CODE
  end
  
  def channel_receive_many
    <<-CODE
    (void)stack_pop();
    stack_push(Qnil);
    CODE
  end
  
  def channel_send_in_microseconds
    <<-CODE
    double seconds;
    (void)stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1, INTEGER);
    
    GUARD(RISA(self, channel));
    
    if(FIXNUM_P(t1)) {
      k = (long)FIXNUM_TO_INT(t1);
    } else {
      k = (long)bignum_to_int(state, t1);
    }
    
    seconds = k / 1000000.0;
    
    cpu_event_wake_channel(state, c, self, seconds);
    stack_push(Qtrue);
    CODE
  end

  def channel_send_in_seconds
    <<-CODE
    double seconds;

    (void)stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1, FLOAT);

    GUARD(RISA(self, channel));

    seconds = FLOAT_TO_DOUBLE(t1);

    cpu_event_wake_channel(state, c, self, seconds);
    stack_push(Qtrue);
    CODE
  end

  def channel_send_on_readable
    <<-CODE
    (void)stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    
    GUARD(RISA(self, channel));
    GUARD(STRING_P(t2) || NIL_P(t2));
    GUARD(FIXNUM_P(t3) || NIL_P(t3));
    if(IO_P(t1)) {
      j = io_to_fd(t1);
    } else if(FIXNUM_P(t1)) {
      j = FIXNUM_TO_INT(t1);
    } else {
      GUARD(0);
    }
    cpu_event_wait_readable(state, c, self, j, t2, FIXNUM_TO_INT(t3));
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_writable
    <<-CODE
    (void)stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1,   IO);
    
    GUARD(RISA(self, channel));
    
    j = io_to_fd(t1);
    cpu_event_wait_writable(state, c, self, j);
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_signal
    <<-CODE
    (void)stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1,   FIXNUM);
    GUARD(RISA(self, channel));
    
    cpu_event_wait_signal(state, c, self, FIXNUM_TO_INT(t1));
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_stopped
    <<-CODE
    (void)stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);
    GUARD(RISA(self, channel));
    
    cpu_event_wait_child(state, c, self, FIXNUM_TO_INT(t1), FIXNUM_TO_INT(t2));
    stack_push(Qtrue);
    CODE
  end
  
  def thread_new
    <<-CODE
    self = stack_pop();
    stack_push(cpu_thread_new(state, c));
    CODE
  end
  
  def thread_run
    <<-CODE
    self = stack_pop();
    GUARD(cpu_thread_alive_p(state, self));
    
    /* So when we're restored, there is a ret val. */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);    
    cpu_thread_force_run(state, c, self);
    CODE
  end
  
  def thread_schedule
    <<-CODE
    self = stack_pop();
    cpu_thread_schedule(state, self);
    stack_push(Qnil);
    CODE
  end
  
  def thread_yield
    <<-CODE
    (void)stack_pop();
    /* Same reason as thread_run */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);
    THDEBUG("%d: thread yield.\\n", getpid());
    cpu_thread_run_best(state, c);
    CODE
  end
  
  def thread_dequeue
    <<-CODE
    (void)stack_pop();
    THDEBUG("%d: dequeue thread.\\n", getpid());
    cpu_thread_exited(state, c);
    CODE
  end
  
  def thread_current
    <<-CODE
    (void)stack_pop();
    stack_push(c->current_thread);
    CODE
  end
  
  def object_become
    <<-CODE
    void state_object_become(STATE, cpu c, OBJECT from, OBJECT to);
    
    POP(self, REFERENCE);
    POP(t1,   REFERENCE);
    state_object_become(state, c, self, t1);
    stack_push(self);
    CODE
  end
  
  def old_object_become
    <<-CODE
    /* A very poor mans become. Copies all the data of t1 into self, so that 
       self 'becomes' t1. This works perfectly for 'normal' objects that just
       use ivars because the copy of t1 shares the ivar's hash, so they have the
       same ivars. If you use #become! on a object that uses slots, they're just
       copied, so you should use the newly become'd object rather than the original.
       In other words, there is strangness with this implementation. */
       
    POP(self, REFERENCE);
    POP(t1,   REFERENCE);
    GUARD(NUM_FIELDS(t1) <= NUM_FIELDS(self));
    k = NUM_FIELDS(self);
    memcpy((void*)self, (void*)t1, SIZE_IN_BYTES(t1));
    if(NUM_FIELDS(self) < k) {
      for(j = 0; j < (k - NUM_FIELDS(self)); j++) {
        SET_FIELD(self, NUM_FIELDS(self) + j, Qnil);
      }
    }
    stack_push(Qnil);
    CODE
  end
  
  def sampler_activate
    <<-CODE
    (void)stack_pop();
    POP(t1, FIXNUM);
    cpu_sampler_activate(state, FIXNUM_TO_INT(t1));
    stack_push(LL2I((long)clock()));
    CODE
  end
  
  def sampler_stop
    <<-CODE
    (void)stack_pop();
    t1 = cpu_sampler_disable(state);
    stack_push(t1);
    CODE
  end
  
  def fork_process
    <<-CODE
    (void)stack_pop(); /* class */
    k = fork();
    if(k == -1) {
      cpu_raise_from_errno(state, c, "Unable to fork");
    } else {
      if (k == 0) {
        environment_fork();
      }
      stack_push(I2N(k));
    }
    CODE
  end
  
  # aka execve().
  def replace_process
    <<-CODE
    int i;
    char *tmp, *file;
    char **argv;
    
    (void)stack_pop(); /* class */
    POP(t1, STRING);
    POP(t2, ARRAY);
    
    k = FIXNUM_TO_INT(array_get_total(t2));
    argv = ALLOC_N(char*, k + 1);
    for(j = 0; j < k; j++) {
      t3 = array_get(state, t2, j);
      if(!ISA(t3, state->global->string)) {
        for(i = 0; i < j; i++) {
          XFREE(argv[i]);
        }
        XFREE(argv);
        return FALSE;
      }
      
      tmp = string_byte_address(state, t3);
      argv[j] = tmp ? strdup(tmp) : NULL;
    }
    
    argv[k] = NULL;
    
    tmp = string_byte_address(state, t1);
    file = tmp ? strdup(tmp) : NULL;
    
    cpu_task_disable_preemption(state);
    k = execvp(file, argv);

    /* If you're here, there was an error. */    
    cpu_task_configure_preemption(state);
    cpu_raise_from_errno(state, c, "Unable to execute");    
    
    CODE
  end

  def ivar_get
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    stack_push(object_get_ivar(state, self, t1));
    CODE
  end
  
  def ivar_set
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    object_set_ivar(state, self, t1, t2);
    stack_push(t2);
    CODE
  end
  
  def ivars_get
    <<-CODE
    self = stack_pop();
    stack_push(object_get_ivars(state, self));
    CODE
  end
  
  def str_crypt
    <<-CODE
    POP(self, STRING);
    POP(t1, STRING);
    stack_push(string_new(state, crypt(string_byte_address(state, self),
        string_byte_address(state, t1))));
    CODE
  end
    
  def env_get
    <<-CODE
    char *key;

    (void)stack_pop();
    POP(t1, STRING);

    t2 = Qnil;

    key = string_byte_address(state, t1);
    if (key) {
      char *value = getenv(key);

      if (value) {
        t2 = string_new(state, value);
      }
    }

    stack_push(t2);
    CODE
  end

  def env_set
    <<-CODE
    char *key, *value;

    (void)stack_pop();
    POP(t1, STRING);
    t2 = stack_pop();

    key = string_byte_address(state, t1);
    if(key) {
      /* if t2 is nil, we need to delete the variable
       * and return its value.
       */
      if(NIL_P(t2)) {
        value = getenv(key);
        if(value) {
          stack_push(string_new(state, value));
          unsetenv(key);
        } else {
          stack_push(Qnil);
        }
      } else {
        GUARD(STRING_P(t2));
        value = string_byte_address(state, t2);
        if(value) {
          setenv(key, value, 1);
          stack_push(t2);
        } else {
          stack_push(Qnil);
        }
      }
    }
    CODE
  end

  def env_as_hash
    <<-CODE
    char *cur, **total = environ;
  
    OBJECT hash = hash_new(state);

    do {
      cur = *total++;
      if(!cur) break;

      char *name = cur;
      int i = 0;

      while(*cur && *cur != '=') {
        i++; cur++;
      }

      OBJECT key = string_new2(state, name, i);
      OBJECT val = string_new(state, cur+1);

      hash_set(state, hash, key, val);
    } while(cur);

    stack_push(hash);
    CODE
  end

  def bignum_size(_ = bignum)
    <<-CODE
    stack_push(bignum_size(state, self));
    CODE
  end

  def iseq_compile
    <<-CODE
    self = stack_pop();
    cpu_compile_method(state, self);
    stack_push(Qtrue);
    CODE
  end

  def reset_method_cache
    <<-CODE
    (void)stack_pop(); /* self */
    t1 = stack_pop();
    cpu_clear_cache_for_method(state, c, t1, TRUE);
    CODE
  end
  
  def bignum_from_float
    <<-CODE
    (void)stack_pop(); /* we don't care */
    POP(t1, FLOAT);
    stack_push(bignum_from_double(state, FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  def save_encloser_path
    <<-CODE
    cpu_set_encloser_path(state, c, state->global->object);
    stack_push(Qnil);
    CODE
  end

  def restore_encloser_path
    <<-CODE
    cpu_push_encloser(state, c);
    stack_push(Qnil);
    CODE
  end
  
  def array_aref
    <<-CODE
    GUARD(num_args == 1);
    self = stack_pop();
    t1 = stack_top();
    GUARD(FIXNUM_P(t1));

    j = FIXNUM_TO_INT(array_get_total(self));

    k = FIXNUM_TO_INT(t1);

    if(k < 0) k += j;
    
    if(k < 0 || k >= j) {
      stack_set_top(Qnil);
    } else {
      k += FIXNUM_TO_INT(array_get_start(self));
      t3 = array_get_tuple(self);
      GUARD(k < NUM_FIELDS(t3));
      
      stack_set_top(tuple_at(state, t3, k));
    }
    CODE
  end
  
  def array_aset
    <<-CODE
    GUARD(num_args == 2);
    self = stack_pop();
    t1 = stack_pop();
    GUARD(FIXNUM_P(t1));

    j = FIXNUM_TO_INT(array_get_total(self));    
    k = FIXNUM_TO_INT(t1);
  
    if(k < 0) k += j;
      
    if(k >= j - 1) {
      array_set_total(self, I2N(k + 1));
    }

    k += FIXNUM_TO_INT(array_get_start(self));
    t3 = array_get_tuple(self);
    GUARD(k < NUM_FIELDS(t3));
    
    tuple_put(state, t3, k, stack_top());
    CODE
  end
  
  def string_append
    <<-CODE
    GUARD(num_args == 1);
    self = stack_pop();
    t1 = stack_top();
    GUARD(STRING_P(t1));
    
    string_append(state, self, t1);
    cpu_stack_set_top(state, c, self);
    CODE
  end
  
  def string_dup
    <<-CODE
    cpu_stack_set_top(state, c, string_dup(state, stack_top()));
    CODE
  end
  
  def string_equal
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    GUARD(STRING_P(t1));
    
    if(self == t1) {
      stack_push(Qtrue);
    } else {
      t2 = string_get_bytes(self);
      
      if(t2 != string_get_bytes(t1)) {
        stack_push(Qfalse);
      } else {
        j =  FIXNUM_TO_INT(t2);
        t2 = string_get_data(self);
        t3 = string_get_data(t1);

        k = memcmp(BYTEARRAY_ADDRESS(t2), BYTEARRAY_ADDRESS(t3), j);
        stack_push(k == 0 ? Qtrue : Qfalse);
      }
    }
    CODE
  end
  
  def object_send
    <<-CODE
    self = stack_pop();
    GUARD(num_args >= 1);
    t1 = stack_pop();
    if(!SYMBOL_P(t1)) {
      if(STRING_P(t1)) {
        t1 = string_to_sym(state, t1);
      } else {
        GUARD(0);
      }
    }

    /* Send is allowed to call private methods. */
    c->call_flags = 1;
    
    cpu_unified_send(state, c, self, t1, num_args - 1, block);
    CODE
  end

  def machine_new
    <<-CODE
    int *pipes;
    int argc;
    char **argv;
    OBJECT ret, ary, str;
    int i;
    machine m;
    environment e = environment_current();

    (void)stack_pop(); /* class */
    POP(ary, ARRAY);

    argc = FIXNUM_TO_INT(array_get_total(ary));
    argv = ALLOC_N(char*, argc);
    for(i = 0; i < argc; i++) {
      str = array_get(state, ary, i);
      if(STRING_P(str)) {
        argv[i] = strdup(string_byte_address(state, str));
      } else {
        argv[i] = strdup("");
      }
    }
    
    m = machine_new(e);
    pipes = machine_setup_thread(m, argc, argv);

    m->parent_id = current_machine->id;

    ret = tuple_new(state, 4);
    tuple_put(state, ret, 0, I2N(m->id));
    tuple_put(state, ret, 1, io_new(state, pipes[0], "w"));
    tuple_put(state, ret, 2, io_new(state, pipes[1], "r"));
    tuple_put(state, ret, 3, io_new(state, pipes[2], "r"));
    stack_push(ret);

    environment_start_thread(e, m);

    CODE
  end

  def machine_join
    <<-CODE
    (void)stack_pop(); /* class */
    POP(t1, FIXNUM);
    if(environment_join_machine(environment_current(), FIXNUM_TO_INT(t1))) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    CODE
  end

  def machine_get_message
    <<-CODE
    (void)stack_pop(); /* class */
    stack_push(environment_get_message(environment_current(), current_machine->id));
    CODE
  end

  def machine_send_message
    <<-CODE
    (void)stack_pop(); /* class */
    POP(t1, FIXNUM);
    t2 = stack_pop();

    environment_send_message(environment_current(), FIXNUM_TO_INT(t1), t2);

    stack_push(Qtrue);
    CODE
  end

end

prim = ShotgunPrimitives.new
prim.generate_select(STDOUT)
