require File.dirname(__FILE__) + '/../../lib/bytecode/primitive_names'
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
    i = Bytecode::Compiler::FirstRuntimePrimitive
    order = Bytecode::Compiler::RuntimePrimitives
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
    
  end

  def generate_declarations(fd)
    fd.puts "int _int, j, k, m;"
    fd.puts "OBJECT _lit, t1, t2, t3;"
    fd.puts "char buffer[1024];"
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
    stack_push(fixnum_div(state, self, t1));
    CODE
  end
  
  def bignum_div(_ = bignum, t1 = bignum)
    <<-CODE
    // Can this ever happen since bignum will always be > zero?
    GUARD( ! bignum_equal(state, t1, bignum_new(state, 0)) )
    stack_push(bignum_div(state, self, t1));
    CODE
  end
  
  def equal(_ = fixnum, t1 = fixnum)
    <<-CODE
    /* No need to shift them to be longs, the comparison is the same. */
    if(self == t1)
      stack_push(Qtrue); 
    else
      stack_push(Qfalse);
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
    k = FIXNUM_TO_INT(t1);
    if(k % 4 == 0) {
      k = k / REFSIZE;
    } else {
      k = (k + REFSIZE) / REFSIZE;
    }
    t2 = NEW_OBJECT(self, k);
    object_make_byte_storage(state, t2);
    stack_push(t2);
    CODE
  end
  
  # FIXME: this is really messy can someone who knows this better refactor it
  def create_block
    <<-CODE
    self = stack_pop(); // Again no type assertions can be made (or can they!?)
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);

    t3 =   Qnil;
    if( !RISA(self, methctx) && RISA(self, blokctx) ) {
      t3 = blokenv_get_home(blokctx_get_env(self));
    } else {
      t3 = self;
    }
    
    // GUARD(t3 == Qnil) would like to use this...
    if(t3 == Qnil) {
      // can this be put somewhere else?
      printf("Create block failed, %s!!\\n", _inspect(self));
      _ret = FALSE;
    } else {
      cpu_flush_sp(c);
      cpu_flush_ip(c);
      j = c->ip + 5;
      t2 = blokenv_s_under_context(state, t3, c->block, j, t1, t2, 0);
      stack_push(t2);
    }
    CODE
  end
  
  def block_given
    <<-CODE
    ARITY(0)
    // pop true off the stack to conform to the "all primitives have a self rule"
    self = stack_pop(); GUARD( TRUE_P(self) )

    if( RTEST(c->block) ) {
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

    cpu_flush_sp(c);
    t2 = blokenv_create_context(state, self, c->active_context, c->sp);    
    cpu_activate_context(state, c, t2, blokenv_get_home(self));
    CODE
  end
  
  def io_write
    <<-CODE
    POP(self, IO);
    POP(t1, STRING);

    j = FIXNUM_TO_INT(io_get_descriptor(self));
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
    j = FIXNUM_TO_INT(io_get_descriptor(self));
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
      io_set_descriptor(t1, I2N(fds[0]));
      io_initialize(state, fds[0]);
      io_set_descriptor(t2, I2N(fds[1]));      
      io_initialize(state, fds[1]);
    }
    stack_push(I2N(j));
    CODE
  end
  
  def io_open
    <<-CODE
    FILE *_fobj;
    char *_path, *_mode;
    self = stack_pop(); /* class */
    POP(t1, STRING);
    POP(t2, STRING);

    _path = string_as_string(state, t1);
    _mode = string_as_string(state, t2);
    _fobj = fopen(_path, _mode);
    t3 = io_new(state, fileno(_fobj));
    io_set_descriptor(t3, t1);
    free(_path);
    free(_mode);
    stack_push(t3);
    CODE
  end
  
  def io_reopen
    <<-CODE
    POP(self, IO);
    POP(t1, IO);
    
    /* MRI does a ton more with reopen, but I don't yet understand why.
       This seems perfectly acceptable currently. */
    
    k = FIXNUM_TO_INT(io_get_descriptor(self));
    j = FIXNUM_TO_INT(io_get_descriptor(t1));
    
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
    j = FIXNUM_TO_INT(io_get_descriptor(self));
    GUARD(j >= 0)

    if( close(j) ) {
      stack_push(Qfalse);
    } else {
      io_set_descriptor(self, I2N(-1));
      stack_push(Qtrue);
    }
    CODE
  end
  
  def file_unlink
    <<-CODE
    stack_pop(); /* class */
    POP(t1, STRING);

    char *name;
    name = string_as_string(state, t1);
    if(unlink(name) == 0) {
      stack_push(Qtrue);
    } else {
      /* TODO translate errno into an exception. */
      stack_push(Qfalse);
    }
    free(name);
    CODE
  end
  
  def gettimeofday
    <<-CODE
    struct timeval tv;

    stack_pop();

    /* don't fill in the 2nd argument here. getting the timezone here
     * this way is not portable and broken anyway.
     */
    gettimeofday(&tv, NULL);

    k = array_new(state, 2);
    array_set(state, k, 0, I2N(tv.tv_sec));
    array_set(state, k, 1, I2N(tv.tv_usec));

    stack_push(k);
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
    tm.tm_zone = string_as_string(state, array_get(state, t1, 10));
#endif

    format = string_as_string(state, t2);

    out = strftime(str, MAX_STRFTIME_OUTPUT-1, format, &tm);

    str[MAX_STRFTIME_OUTPUT] = '\\0';
    t3 = string_new2(state, str, out);
    stack_push(t3);

    if(format) {free(format);}

#ifdef HAVE_STRUCT_TM_TM_ZONE
    if(tm.tm_zone) free(tm.tm_zone);
#endif
    CODE
  end

  def time_switch
    <<-CODE
    time_t seconds;
    struct tm *tm;

    POP(self, REFERENCE);
    POP(t1, NUMERIC);
    t2 = stack_pop();

    if(FIXNUM_P(t1)) {
      seconds = FIXNUM_TO_INT(t1);
    } else {
      seconds = bignum_to_int(state, t1);
    }

    if(t2 == Qtrue) {
      tm = gmtime(&seconds);
    } else {
      tm = localtime(&seconds);
    }

    k = array_new(state, 2);
    array_set(state, k, 0, I2N(tm->tm_sec));
    array_set(state, k, 1, I2N(tm->tm_min));
    array_set(state, k, 2, I2N(tm->tm_hour));
    array_set(state, k, 3, I2N(tm->tm_mday));
    array_set(state, k, 4, I2N(tm->tm_mon));
    array_set(state, k, 5, I2N(tm->tm_year));
    array_set(state, k, 6, I2N(tm->tm_wday));
    array_set(state, k, 7, I2N(tm->tm_yday));
    array_set(state, k, 8, I2N(tm->tm_isdst));

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    array_set(state, k, 9, I2N(tm->tm_gmtoff));
#else
    array_set(state, k, 9, Qnil);
#endif

#ifdef HAVE_STRUCT_TM_TM_ZONE
    array_set(state, k, 10, string_new(state, tm->tm_zone));
#else
    array_set(state, k, 10, Qnil);
#endif

    stack_push(k);
    CODE
  end

  def mktime
    <<-CODE
    time_t seconds;
    struct tm tm;
    char *old_tz, old_tz_buf[128];
    OBJECT t5, t6, t7, t8, t9, ret;

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
    array_set(state, ret, 0, I2N(seconds));
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
    /* self is ANY object because object_logical_class knows all. */
    self = stack_pop();
    stack_push(object_logical_class(state, self));
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

    t3 = hash_get_undef(state, self, FIXNUM_TO_INT(t1));
    stack_push(t3);
    CODE
  end

  def hash_object
    <<-CODE
    /* self is ANY object because object_logical_class knows all. */    
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
    if(FIXNUM_P(t1)) {
      HEADER(self)->hash = (uint32_t)FIXNUM_TO_INT(t1);
    } else {
      HEADER(self)->hash = (uint32_t)bignum_to_int(state, t1);
    }
    stack_push(UI2N(HEADER(self)->hash));
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

    j = FIXNUM_TO_INT(t2);
    object_copy_fields_from(state, t1, self, j, NUM_FIELDS(t1) - j);
    HEADER(self)->flags  = HEADER(t1)->flags;
    // HEADER(self)->flags2 = (HEADER(t1)->flags2 & ZONE_MASK) | GC_ZONE(self);
    stack_push(t1);
    CODE
  end
  
  def fastctx_dup
    <<-CODE
    POP(self, REFERENCE);
    GUARD(ISA(self, state->global->fastctx));
    
    t1 = _om_new_ultra(state->om, state->global->fastctx, (HEADER_SIZE + FASTCTX_FIELDS) * REFSIZE);
    SET_NUM_FIELDS(t1, FASTCTX_FIELDS);

    FLAG_SET(t1, CTXFastFlag);
    FLAG_SET(t1, StoresBytesFlag);
    
    memcpy(FASTCTX(t1), FASTCTX(self), sizeof(struct fast_context));
    stack_push(t1);    
    CODE
  end

  def tuple_shifted
    <<-CODE
    POP(self, TUPLE);
    POP(t1, FIXNUM);

    j = FIXNUM_TO_INT(t1);
    t2 = tuple_new(state, NUM_FIELDS(self) + j);
    object_copy_fields_shifted(state, self, t2, j);
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
    t2 = (*indexed = FIXNUM_TO_INT(t2));
    stack_push(UI2N(t2));
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
    t1 = stack_pop();
    POP(t2, FIXNUM);
    if(!STRING_P(t1)) {
      _ret = FALSE;
    } else {
      char *path = string_as_string(state, t1);
      t2 = cpu_unmarshal_file(state, path, FIXNUM_TO_INT(t2));
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
    if(!STRING_P(t1)) {
      _ret = FALSE;
    } else {
      char *path = string_as_string(state, t1);
      j = stat(path, &sb);
      free(path);

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
      if(!STRING_P(t1) || !FIXNUM_P(t2)) {
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
    if(!STRING_P(t1)) {
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
        struct termios ts;
        char sbuf[1024];
        
        io = popen("stty -g", "r");
        fgets(sbuf, 1023, io);
        setenv("_TERM_SETTINGS", sbuf, 1);
        pclose(io);
        system("stty -icanon -isig -echo min 1");
        tcgetattr(1, &ts);
        t3 = tuple_new2(state, 4, 
          I2N(ts.c_cc[VERASE]), I2N(ts.c_cc[VKILL]),
          I2N(ts.c_cc[VQUIT]),  I2N(ts.c_cc[VINTR])
        );
        stack_push(t3);
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
    if(!STRING_P(t1)) {
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
    if(!STRING_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(regexp_match(state, self, t1));
    }
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
  
  def set_index
    <<-CODE
    self = stack_pop();
    if(!INDEXED(self)) {
      _ret = FALSE;
    } else {
      j = FIXNUM_TO_INT(NTH_FIELD(mo, 4));
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
      j = FIXNUM_TO_INT(NTH_FIELD(mo, 4));
      stack_push(NTH_FIELD(self, j));
    }
    CODE
  end

  def zlib_inflate
    <<-CODE
    stack_pop(); //class method, ignore self
    t1 = stack_pop(); //string to inflate
    if (!STRING_P(t1)) { //parameter must be a string
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
    if (!STRING_P(t1)) { //parameter must be a string
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

      zs.avail_in = strlen((char *)input); // Lower than for zlib_inflate, so that we don't consume the zero-terminator.  [and appease emacs' syntax parser]
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
  
  def fixnum_modulo(_ = fixnum, t1 = fixnum)
    <<-CODE
    GUARD( FIXNUM_TO_INT(t1) != 0 ) // no divide by zero

    t3 = fixnum_divmod(state, self, t1);
    stack_push(array_get(state, t3, 1));
    CODE
  end
  
  def marshal_object
    <<-CODE
    stack_pop(); /* class */
    t1 = stack_pop();
    POP(t2, FIXNUM);
    stack_push(cpu_marshal(state, t1, FIXNUM_TO_INT(t2)));
    CODE
  end
  
  def unmarshal_object
    <<-CODE
    stack_pop(); /* class */
    t1 = stack_pop();
    POP(t2, FIXNUM);
    if(!STRING_P(t1)) {
      _ret = FALSE;
    } else {
      stack_push(cpu_unmarshal(state, string_byte_address(state, t1), FIXNUM_TO_INT(t2)));
    }
    CODE
  end
  
  def marshal_to_file
    <<-CODE
    char *_path;
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    POP(t3, FIXNUM);
    if(!STRING_P(t2)) {
      _ret = FALSE;
    } else {
      _path = string_as_string(state, t2);
      stack_push(cpu_marshal_to_file(state, t1, _path, FIXNUM_TO_INT(t3)));
      free(_path);
    }
    CODE
  end
  
  def unmarshal_from_file
    <<-CODE
    char *_path;
    stack_pop(); /* class */
    t1 = stack_pop();
    POP(t2, FIXNUM);
    if(!STRING_P(t1)) {
      _ret = FALSE;
    } else {
      _path = string_as_string(state, t1);
      stack_push(cpu_unmarshal_file(state, _path, FIXNUM_TO_INT(t2)));
      free(_path);
    }
    CODE
  end
  
  def archive_files
    <<-CODE
    char *path;
    stack_pop(); /* blah */
    t1 = stack_pop();
    if(!STRING_P(t1)) {
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
    if(!STRING_P(t1) || !STRING_P(t2)) {
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
    POP(t3, FIXNUM);
    if(!STRING_P(t1) || !STRING_P(t2)) {
      _ret = FALSE;
    } else {
      path = string_as_string(state, t1);
      file = string_as_string(state, t2);
      stack_push(archive_get_object(state, path, file, FIXNUM_TO_INT(t3)));
      free(path);
      free(file);
    }
    CODE
  end
  
  def archive_add_file
    <<-CODE
    char *path, *file, *data;
    stack_pop();
    POP(t1, STRING);
    POP(t2, STRING);
    POP(t3, STRING);
    
    path = string_as_string(state, t1);
    file = string_as_string(state, t2);
    data = string_as_string(state, t3);
    
    t1 = archive_add_file(state, path, file, data);
    free(path);
    free(file);
    free(data);
    stack_push(t1);
    CODE
  end
  
  def archive_add_object
    <<-CODE
    char *path, *file;
    stack_pop();
    POP(t1, STRING);
    POP(t2, STRING);
    t3 = stack_pop();
    POP(t4, FIXNUM);
    
    path = string_as_string(state, t1);
    file = string_as_string(state, t2);
    
    t1 = archive_add_object(state, path, file, t3, FIXNUM_TO_INT(t4));
    
    free(path);
    free(file);
    
    stack_push(t1);
    CODE
  end
  
  def archive_delete_file
    <<-CODE
    stack_pop();
    t1 = stack_pop();
    t2 = stack_pop();
    if(!STRING_P(t1) || !FIXNUM_P(t2)) {
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
    stack_pop();
    POP(t1, FIXNUM);
    stack_push(bignum_new(state, FIXNUM_TO_INT(t1)));
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

  def bignum_and(_ = bignum, t1 = bignum)
    <<-CODE
    stack_push(bignum_and(state, self, t1));
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
    if(FLOAT_P(self)) {
      stack_push(float_infinite_p(state, self));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_div
    <<-CODE
    POP(self, FLOAT);
    POP(t1, FLOAT);

    stack_push(float_div(state, self, t1));
    CODE
  end
  
  def float_uminus
    <<-CODE
    self = stack_pop();
    if(FLOAT_P(self)) {
      stack_push(float_uminus(state, self));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_pow
    <<-CODE
    self = stack_pop();
    t1 =   stack_pop();
    if(FLOAT_P(self) && FLOAT_P(t1)) {
      stack_push(float_pow(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_to_i
    <<-CODE
    self = stack_pop();
    if(FLOAT_P(self) && float_infinite_p(state, self) == Qfalse && float_nan_p(state, self) == Qfalse) {
      stack_push(float_to_i(state, self));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def numeric_coerce
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(INTEGER_P(self) && INTEGER_P(t1)) {
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
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def bignum_compare(_ = bignum, t1 = bignum)
    <<-CODE
    stack_push(bignum_compare(state, self, t1));
    CODE
  end
  
  def float_compare
    <<-CODE
    self = stack_pop();
    t1 = stack_pop();
    if(FLOAT_P(self) && FLOAT_P(t1)) {
      stack_push(float_compare(state, self, t1));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def fixnum_to_f(_ = fixnum)
    <<-CODE
    stack_push(float_new(state, FIXNUM_TO_DOUBLE(self)));
    CODE
  end
  
  def string_to_f
    <<-CODE
    self = stack_pop();
    if(STRING_P(self)) {
      stack_push(float_new(state, string_to_double(state, self)));
    } else {
      _ret = FALSE;
    }
    CODE
  end
  
  def float_divmod
    <<-CODE
    POP(self, FLOAT);
    POP(t1, FLOAT);
    GUARD( FLOAT_TO_DOUBLE(t1) != 0.0 ) // no divide by zero

    stack_push(float_divmod(state, self, t1));
    CODE
  end
  
  def fixnum_divmod(_ = fixnum, t1 = fixnum)
    <<-CODE
    GUARD( FIXNUM_TO_INT(t1) != 0 ) // no divide by zero
    stack_push(fixnum_divmod(state, self, t1));
    CODE
  end
  
  def float_round
    <<-CODE
    POP(self, FLOAT);

    stack_push(float_round(state, self));
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
  
  def bignum_divmod(_ = bignum, t1 = bignum)
    <<-CODE
    // no divide by zero
    GUARD( ! bignum_equal(state, t1, bignum_new(state, 0)) )

    stack_push(bignum_divmod(state, self, t1));
    CODE
  end
  
  def math_sqrt(_ = reference, t1 = float)
    <<-CODE
      stack_push(math_sqrt(state, t1));
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
    if(RISA(t1, fastctx)) {
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
        case 4:
          stack_push(fc->raiseable);
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
        default:
          stack_push(Qnil);
      }
    } else {
      _ret = FALSE;
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
    
    if(RISA(t1, fastctx)) {
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
        case 4:
          fc->raiseable = t2;
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
          fc->argcount = I2N(t2);
          break;
        case 10:
          fc->name = t2;
          break;
        case 11:
          fc->method_module = t2;
          break;
        default:
          _ret = FALSE;
      }
    } else {
      _ret = FALSE;
    }
    stack_push(t2);
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
    cpu_flush_ip(c);
    cpu_flush_sp(c);
    cpu_save_registers(state, c);
    t1 = nmc_new(state, mo, c->active_context, stack_pop(), method_name, num_args);
    nmc_activate(state, c, t1, FALSE);
    CODE
  end

  def nfunc_call
    <<-CODE
    /* Get rid of the module */
    stack_pop();
    
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
    stack_pop();
    POP(t1, STRING_OR_NIL);
    POP(t2, STRING);
    POP(t3, ARRAY);
    POP(t4, FIXNUM);
    stack_push(ffi_function_create(state, t1, t2, t3, t4));
    CODE
  end
  
  def load_library
    <<-CODE
    stack_pop(); /* self */
    t1 = stack_pop();
    t2 = stack_pop();
    stack_push(subtend_load_library(state, c, t1, t2));
    CODE
  end
  
  def dir_glob
    <<-CODE
    glob_t gd;
    char *pat;
    stack_pop();
    POP(t1, STRING);
    POP(t2, FIXNUM);
    
    /* TODO: use t2. */
    
    pat = string_as_string(state, t1);
    k = glob(pat, 0, NULL, &gd);
    t2 = array_new(state, gd.gl_pathc);
    for(j = 0; j < gd.gl_pathc; j++) {
      array_set(state, t2, j, string_new(state, gd.gl_pathv[j]));
    }
    globfree(&gd);
    free(pat);
    stack_push(t2);
    CODE
  end
  
  def dir_chdir
    <<-CODE
    char *path;
    stack_pop();
    POP(t1, STRING);
    
    path = string_as_string(state, t1);
    if(!chdir(path)) {
      stack_push(Qtrue);
    } else {
      stack_push(Qfalse);
    }
    free(path);
    CODE
  end
  
  def make_weak_ref
    <<-CODE
    stack_pop();
    POP(t1, REFERENCE);
    
    stack_push(object_make_weak_ref(state, t1));
    CODE
  end
  
  def gc_collect_references
    <<-CODE
    stack_pop();
    POP(self, REFERENCE);
    
    stack_push(object_memory_collect_references(state, state->om, self));
    CODE
  end

  def float_sprintf
    <<-CODE
    stack_pop(); /* self */
    POP(t1, STRING);
    POP(t2, FLOAT);
    stack_push(float_sprintf(state, t1, t2));
    CODE
  end
  
  def task_dup
    <<-CODE
    self = stack_pop();
    if(RISA(self, task)) {
      t1 = cpu_task_dup(state, c, self);
    } else {
      t1 = cpu_task_dup(state, c, Qnil);
    }
    
    stack_push(t1);
    CODE
  end
  
  def task_set_current
    <<-CODE
    stack_pop(); /* class */
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
    
    stack_push(cpu_task_associate(state, self, t1));
    CODE
  end
  
  def task_current
    <<-CODE
    stack_pop(); /* class */
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
    
    GUARD(RISA(t1, channel));
    GUARD(RISA(t2, channel));
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
  
  
  def channel_new
    <<-CODE
    stack_pop();
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
    OBJECT ary;
    stack_pop(); /* class */
    ary = stack_pop();
    
    GUARD(RISA(ary, array));
    t1 = array_get_tuple(ary);
    k = FIXNUM_TO_INT(array_get_total(ary));
    
    for(j = 0; j < k; j++) {
      t2 = tuple_at(state, t1, j);
      
      /* Oh, this channel is already ready! return it's value. */
      t3 = channel_get_value(t2);
      if(!NIL_P(t3) && !list_empty_p(t3)) {
        channel_set_value(t2, Qnil);
        stack_push(tuple_new2(state, 2, t2, list_shift(state, t3)));
        return TRUE;
      }
    }
    
    for(j = 0; j < k; j++) {
      t2 = tuple_at(state, t1, j);
      cpu_channel_register(state, c, t2, c->current_thread);      
    }
    
    c->outstanding = ary;
    
    /* We push something on the stack to reserve a place to put the result. */
    stack_push(I2N(343434));
    
    cpu_thread_run_best(state, c);
    CODE
  end
  
  def channel_send_in_microseconds
    <<-CODE
    struct timeval tv;
    stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1, INTEGER);
    
    GUARD(RISA(self, channel));
    
    if(FIXNUM_P(t1)) {
      k = (long)FIXNUM_TO_INT(t1);
    } else {
      k = (long)bignum_to_int(state, t1);
    }
    
    if(k > 1000000) {
      tv.tv_sec = k / 1000000;
      tv.tv_usec = k % 1000000;
    } else {
      tv.tv_sec = 0;
      tv.tv_usec = k;
    }
    
    cpu_event_wake_channel(state, c, self, &tv);
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_readable
    <<-CODE
    stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1,   IO);
    t2 = stack_pop();
    t3 = stack_pop();
    
    GUARD(RISA(self, channel));
    GUARD(STRING_P(t2) || NIL_P(t2));
    GUARD(FIXNUM_P(t3) || NIL_P(t3));
    
    j = FIXNUM_TO_INT(io_get_descriptor(t1));
    cpu_event_wait_readable(state, c, self, j, t2, FIXNUM_TO_INT(t3));
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_writable
    <<-CODE
    stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1,   IO);
    
    GUARD(RISA(self, channel));
    
    j = FIXNUM_TO_INT(io_get_descriptor(t1));
    cpu_event_wait_writable(state, c, self, j);
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_signal
    <<-CODE
    stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1,   FIXNUM);
    GUARD(RISA(self, channel));
    
    cpu_event_wait_signal(state, c, self, FIXNUM_TO_INT(t1));
    stack_push(Qtrue);
    CODE
  end
  
  def channel_send_on_stopped
    <<-CODE
    stack_pop(); /* scheduler */
    POP(self, REFERENCE);
    POP(t1, FIXNUM);
    GUARD(RISA(self, channel));
    
    cpu_event_wait_child(state, c, self, FIXNUM_TO_INT(t1));
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
    /* So when we're restored, there is a ret val. */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);
    cpu_thread_switch(state, c, self);
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
    stack_pop();
    /* Same reason as thread_run */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);
    cpu_thread_run_best(state, c);
    CODE
  end
  
  def thread_current
    <<-CODE
    stack_pop();
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
    stack_pop();
    POP(t1, FIXNUM);
    cpu_sampler_activate(state, FIXNUM_TO_INT(t1));
    stack_push(I2N((int)clock()));
    CODE
  end
  
  def sampler_stop
    <<-CODE
    stack_pop();
    t1 = cpu_sampler_disable(state);
    stack_push(t1);
    CODE
  end
  
  def fork_process
    <<-CODE
    stack_pop(); /* class */
    k = fork();
    if(k == -1) {
      cpu_raise_from_errno(state, c, "Unable to fork");
    } else {
      stack_push(I2N(k));
    }
    CODE
  end
  
  # aka execve().
  def replace_process
    <<-CODE
    int i;
    char *file;
    char **argv;
    
    stack_pop(); /* class */
    POP(t1, STRING);
    POP(t2, ARRAY);
    
    file = string_as_string(state, t1);
    k = FIXNUM_TO_INT(array_get_total(t2)) + 1;
    argv = (char**)calloc(k + 1, sizeof(char*));
    argv[0] = file;
    for(j = 1; j < k; j++) {
      t3 = array_get(state, t2, j - 1);
      if(!ISA(t3, state->global->string)) {
        for(i = 0; i < j; i++) {
          free(argv[i]);
        }
        free(argv);
        return FALSE;
      }
      
      argv[j] = string_as_string(state, t3);
    }
    
    argv[k] = NULL;
    
    k = execvp(file, argv);
    
    /* If you're here, there was an error. */
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

    stack_pop();
    POP(t1, STRING);

    t2 = Qnil;

    key = string_as_string(state, t1);
    if (key) {
      char *value = getenv(key);

      if (value) {
        t2 = string_new(state, value);
      }

      free(key);
    }

    stack_push(t2);
    CODE
  end

  def env_set
    <<-CODE
    char *key, *value;

    stack_pop();
    POP(t1, STRING);
    t2 = stack_pop();

    key = string_as_string(state, t1);
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
        value = string_as_string(state, t2);
        if(value) {
          setenv(key, value, 1);
          stack_push(t2);
          free(value);
        } else {
          stack_push(Qnil);
        }
      }

      free(key);
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
end

prim = ShotgunPrimitives.new
prim.generate_select(STDOUT)

