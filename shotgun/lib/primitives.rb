require File.dirname(__FILE__) + '/primitive_names'

class ShotgunPrimitives

  OldMap = {
    :set_ivar => 1024,
    :get_ivar => 1025,
    :set_index => 1026,
    :get_index => 1027,
    :dispatch_as_method => 1028
  }

  def generate_select(fd, op="prim")
    i = 1
    order = Bytecode::Compiler::Primitives

    File.open("primitive_implementation.gen", "w") do |f|
      order.each do |ins|
        f.puts "int cpu_primitive_#{ins}(STATE, cpu c, const struct message *msg) {"
        f.puts send(ins)
        f.puts "  DONE();\n}"
      end

      f.puts "\nprim_func cpu_lookup_primitive(int index) {"
      f.puts "  static prim_func funcs[] = { NULL, "
      
      order.each do |ins|
        f.puts "    cpu_primitive_#{ins},"
      end

      f.puts "  };"
      f.puts "  return funcs[index];"
      f.puts "}"
      
      f.puts "OBJECT cpu_populate_prim_names(STATE) {"
      f.puts "OBJECT hash = hash_new(state);"
      i = 1
      Bytecode::Compiler::Primitives.each do |name|
        f.puts "hash_set(state, hash, I2N(#{i}), SYM(\"#{name}\"));"
        f.puts "hash_set(state, hash, SYM(\"#{name}\"), I2N(#{i}));"
        i += 1
      end
      f.puts "return hash;}"
    end

    i = 1
    fd.puts "switch(#{op}) {"
    fd.puts "   // NOOP is 0 and signifies a method with no primitive"
    order.each do |ins|
      fd.puts "case #{i}: { // #{ins}"
      if old = OldMap[ins]
        fd.puts "case #{old}:"
      end
      fd.puts "  cpu_patch_primitive(state, msg, cpu_primitive_#{ins}, #{i});"
      fd.puts "  _ret = cpu_primitive_#{ins}(state, c, msg);"
      fd.puts "  break;\n}"

      i += 1
    end

    fd.puts "default:"
    fd.puts '  printf("Error: Primitive index out of range for this VM (%d)\n", prim);'
    fd.puts "  sassert(0);"
    fd.puts "}"
    fd.puts

    File.open("primitive_indexes.h", "w") do |f|
      i = 1
      Bytecode::Compiler::Primitives.each do |name|
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

        printf("Unknown primitive %s\\n", target);
        
        return -1;
      }
      CODE

    end
    
  end

  def add
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      RET(fixnum_add(state, msg->recv, t1));
    } else if(BIGNUM_P(t1)) {
      RET(bignum_add(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) + FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def bignum_add
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_add(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(float_new(state, a + FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def sub
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      RET(fixnum_sub(state, msg->recv, t1));
    } else if(BIGNUM_P(t1)) {
      RET(bignum_sub(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) - FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def bignum_sub
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_sub(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(float_new(state, a - FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def fixnum_mul
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      RET(fixnum_mul(state, msg->recv, t1));
    } else if(BIGNUM_P(t1)) {
      RET(bignum_mul(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) * FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def fixnum_size
    <<-CODE
    RET(I2N(sizeof(native_int)));
    CODE
  end

  def bignum_mul
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_mul(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(float_new(state, a * FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def fixnum_div
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      long mod;
      GUARD(N2I(t1) != 0) // no divide by zero
      RET(I2N(fixnum_div(state, msg->recv, t1, &mod)));
    } else if(BIGNUM_P(t1)) {
      GUARD(!bignum_is_zero(state, t1));
      RET(bignum_div(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) / FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end
  
  def bignum_div
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1)) {
      GUARD(!bignum_is_zero(state, t1));
      RET(bignum_div(state, msg->recv, t1));
    } else if(FIXNUM_P(t1)) {
      GUARD(N2I(t1) != 0) // no divide by zero
      RET(bignum_div(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(float_new(state, a / FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  def bignum_mod
    <<-CODE
    OBJECT t1;

    GUARD(BIGNUM_P(msg->recv));
    POP(t1, INTEGER);

    RET(bignum_mod(state, msg->recv, t1));
    CODE
  end
  
  def equal
    <<-CODE
    OBJECT t1;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    /* No need to shift them to be longs, the comparison is the same. */
    if(msg->recv == t1) {
      RET(Qtrue); 
    } else {
      RET(Qfalse);
    }
    CODE
  end
  
  def object_equal
    <<-CODE
    OBJECT t1;
    t1 = stack_pop();
    
    if(msg->recv == t1) {
      RET(Qtrue); 
    } else {
      RET(Qfalse);
    }
    CODE
  end
  
  def bignum_equal
    <<-CODE
    OBJECT t1;

    GUARD(BIGNUM_P(msg->recv));
    POP(t1, BIGNUM);

    RET(bignum_equal(state, msg->recv, t1));
    CODE
  end
  
  def compare
    <<-CODE
    OBJECT t1;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);
    
    /* we can deduce == quickly and easily, so do it first. */
    
    if(msg->recv == t1) {
      RET(I2N(0));
    } else {
      native_int j = N2I(msg->recv);
      native_int k = N2I(t1);

      if (j < k) {
        RET(I2N(-1));
      } else if (j > k) {
        RET(I2N(1));
      } else {
        /* We shouldn't be here! */
        RET(I2N(0));
      }
    }
    CODE
  end
  
  def fixnum_lt
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();
    native_int j = N2I(msg->recv);

    if(FIXNUM_P(t2)) {
      native_int k = N2I(t2);
      RET(j < k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      OBJECT t1 = bignum_new(state, j);
      RET(bignum_lt(state, t1, t2));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) < FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end
  
  def fixnum_le
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();
    native_int j = N2I(msg->recv);

    if(FIXNUM_P(t2)) {
      native_int k = N2I(t2);
      RET(j <= k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      OBJECT t1 = bignum_new(state, j);
      RET(bignum_le(state, t1, t2));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) <= FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end
  
  def fixnum_gt
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();
    native_int j = N2I(msg->recv);

    if(FIXNUM_P(t2)) {
      native_int k = N2I(t2);
      RET(j > k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      OBJECT t1 = bignum_new(state, j);
      RET(bignum_gt(state, t1, t2));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) > FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end
  
  def fixnum_ge
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();
    native_int j = N2I(msg->recv);

    if(FIXNUM_P(t2)) {
      native_int k = N2I(t2);
      RET(j >= k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      OBJECT t1 = bignum_new(state, j);
      RET(bignum_ge(state, t1, t2));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) >= FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end
  
  
  def at
    <<-CODE
    OBJECT t1;

    GUARD(INDEXED(msg->recv));
    POP(t1, FIXNUM);
    
    native_int j = N2I(t1);
    
    GUARD(j >= 0 && j < NUM_FIELDS(msg->recv));

    RET(NTH_FIELD(msg->recv, j));
    CODE
  end
  
  def put
    <<-CODE
    OBJECT t1, t2;

    GUARD(INDEXED(msg->recv));
    POP(t1, FIXNUM);

    t2 = stack_pop(); // We do not care about the type
    native_int j = N2I(t1);
    GUARD(j >= 0 && j < NUM_FIELDS(msg->recv));

    SET_FIELD(msg->recv, j, t2);
    RET(t2);
    CODE
  end
  
  def fields
    <<-CODE
    GUARD(REFERENCE_P(msg->recv));

    RET(I2N(NUM_FIELDS(msg->recv)));
    CODE
  end
  
  def allocate
    <<-CODE
    OBJECT t1, t2;

    GUARD(CLASS_P(msg->recv));
    
    t1 = class_get_instance_fields(msg->recv);

    t2 = NEW_OBJECT(msg->recv, N2I(t1));
    RET(t2);
    CODE
  end
  
  def allocate_count
    <<-CODE
    OBJECT t1, t2;
    
    GUARD(CLASS_P(msg->recv));
    POP(t1, FIXNUM);

    t2 = NEW_OBJECT(msg->recv, N2I(t1));
    RET(t2);
    CODE
  end
  
  def allocate_bytes
    <<-CODE
    OBJECT t1, t2;

    GUARD(CLASS_P(msg->recv));
    
    POP(t1, FIXNUM);
    
    t2 = bytearray_new(state, N2I(t1));
    t2->klass = msg->recv;
   
    RET(t2);
    CODE
  end

  def io_seek
    <<-CODE
      OBJECT t1, t2;
      native_int j;
      off_t position;
      
      GUARD(IO_P(msg->recv));
      
      POP(t1, INTEGER); /* offset */
      POP(t2, FIXNUM); /* whence */

      j = io_to_fd(msg->recv);

      if (FIXNUM_P(t1)) {
        position = lseek(j, N2I(t1), N2I(t2));
      } else {
        position = lseek(j, bignum_to_ll(state, t1), N2I(t2));
      }

      if (position == -1) {
        RAISE_FROM_ERRNO("Unable to seek");
        return TRUE;
      } else {
        RET(I2N(position));
      }
    CODE
  end

  def block_given
    <<-CODE
    ARITY(0)
    // pop true off the stack to conform to the "all primitives have a msg->recv rule"
    GUARD( TRUE_P(msg->recv) )

    if( RTEST(cpu_current_block(state, c)) ) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end
  
  def block_call
    <<-CODE
    GUARD(BLOCKENV_P(msg->recv));
    
    blokenv_call(state, c, msg->recv, msg->args);
    CODE
  end
  
  def io_write
    <<-CODE
    OBJECT t1, t2;

    GUARD(IO_P(msg->recv));
    POP(t1, STRING);

    native_int j = io_to_fd(msg->recv);
    char *buf = string_byte_address(state, t1);
    native_int k = N2I(string_get_bytes(t1));
    
    k = write(j, buf, k);
    t2 = I2N(k);
    if(k != N2I(t2)) {
      t2 = bignum_new(state, k);
    }
    
    RET(t2);
    CODE
  end
  
  def io_read
    <<-CODE
    OBJECT t1, t2, t3;
    native_int j, k;

    GUARD(IO_P(msg->recv));

    POP(t1, FIXNUM);

    t2 = string_new2(state, NULL, N2I(t1));
    j = io_to_fd(msg->recv);
    k = read(j, string_byte_address(state, t2), N2I(t1));
    if(k == 0) {
      t2 = Qnil;
    } else if(k != N2I(t1)) {
      t3 = string_new2(state, NULL, k);
      memcpy(string_byte_address(state, t3), string_byte_address(state, t2), k);
      t2 = t3;
    }
    
    RET(t2);
    CODE
  end
  
  def create_pipe
    <<-CODE
    OBJECT t1, t2;
    int j;
    int fds[2];

    POP(t1, IO);
    POP(t2, IO);
    
    j = pipe(fds);
    if(!j) {
      io_wrap(state, t1, fds[0], "r");
      io_wrap(state, t2, fds[1], "r");
    }

    RET(I2N(j));
    CODE
  end
  
  def io_open
    <<-CODE
    int fd, mode, perm;
    char *_path;
    OBJECT t1, t2, t3;

    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    
    _path = string_byte_address(state, t1);
    mode = N2I(t2);
    perm = N2I(t3);

    fd = open(_path, mode, perm);

    RET(I2N(fd));
    CODE
  end
  
  def io_reopen
    <<-CODE
    OBJECT t1;
    native_int j, k;

    GUARD(IO_P(msg->recv));
    POP(t1, IO);
    
    /* MRI does a ton more with reopen, but I don't yet understand why.
       This seems perfectly acceptable currently. */
    
    k = io_to_fd(msg->recv);
    j = io_to_fd(t1);

    /* Probably needs an fflush here. */
    if(dup2(j, k) == -1) {
      RAISE_FROM_ERRNO("Unable to reopen IO object");
    } else {
      RET(Qtrue);
    }
    
    CODE
  end
  
  def io_close
    <<-CODE
    native_int j;
    
    GUARD(IO_P(msg->recv));
    
    j = io_to_fd(msg->recv);
    GUARD(j >= 0);

    if(close(j)) {
      RET(Qfalse);
    } else {
      cpu_event_clear(state, j);
      io_set_descriptor(msg->recv, I2N(-1));
      RET(Qtrue);
    }
    CODE
  end
  
  def io_operation
    <<-CODE
    OBJECT t1;
    native_int k, j;

    GUARD(IO_P(msg->recv));
    POP(t1, FIXNUM);
    
    j = io_to_fd(msg->recv);
    GUARD(j >= 0);
    
    k = N2I(t1);
    switch(k) {
      case 0:
        if(isatty(j)) {
          RET(Qtrue);
        } else {
          RET(Qfalse);
        }
        break;
      case 1:
        RET(string_new(state, ttyname(j)));
        break;
      default:
        RET(Qnil);
    }
    CODE
  end
  
  def file_unlink
    <<-CODE
    OBJECT t1;
    char *name;
    
    POP(t1, STRING);

    name = string_byte_address(state, t1);
    if(unlink(name) == 0) {
      RET(Qtrue);
    } else {
      /* TODO translate errno into an exception. */
      RET(Qfalse);
    }
    CODE
  end
  
  def gettimeofday
    <<-CODE
    OBJECT t1;
    struct timeval tv;

    /* don't fill in the 2nd argument here. getting the timezone here
     * this way is not portable and broken anyway.
     */
    gettimeofday(&tv, NULL);

    /* update Time::TIMEVAL_FIELDS when changing order of fields */
    t1 = array_new(state, 2);
    array_set(state, t1, 0, ML2N(tv.tv_sec));
    array_set(state, t1, 1, ML2N(tv.tv_usec));

    RET(t1);
    CODE
  end

  def strftime
    <<-CODE
    OBJECT t1, t2, t3;

    GUARD(REFERENCE_P(msg->recv));
    POP(t1, ARRAY);
    POP(t2, STRING);

    struct tm tm;
    char *format = NULL;
    char str[MAX_STRFTIME_OUTPUT+1];
    size_t out;

    tm.tm_sec = N2I(array_get(state, t1, 0));
    tm.tm_min = N2I(array_get(state, t1, 1));
    tm.tm_hour = N2I(array_get(state, t1, 2));
    tm.tm_mday = N2I(array_get(state, t1, 3));
    tm.tm_mon = N2I(array_get(state, t1, 4));
    tm.tm_year = N2I(array_get(state, t1, 5));
    tm.tm_wday = N2I(array_get(state, t1, 6));
    tm.tm_yday = N2I(array_get(state, t1, 7));
    tm.tm_isdst = N2I(array_get(state, t1, 8));

#ifdef HAVE_STRUCT_TM_TM_GMTOFF
    tm.tm_gmtoff = N2I(array_get(state, t1, 9));
#endif

#ifdef HAVE_STRUCT_TM_TM_ZONE
    tm.tm_zone = string_byte_address(state, array_get(state, t1, 10));
#endif

    format = string_byte_address(state, t2);

    out = strftime(str, MAX_STRFTIME_OUTPUT-1, format, &tm);

    str[MAX_STRFTIME_OUTPUT] = '\\0';
    t3 = string_new2(state, str, out);
    RET(t3);
    CODE
  end

  def time_switch
    <<-CODE
    time_t seconds;
    struct tm *tm;
    OBJECT t1, t2, t3;

    GUARD(REFERENCE_P(msg->recv));

    POP(t1, INTEGER);
    t2 = stack_pop();

    if(FIXNUM_P(t1)) {
      seconds = N2I(t1);
    } else {
      seconds = bignum_to_ll(state, t1);
    }

    if(t2 == Qtrue) {
      tm = gmtime(&seconds);
    } else {
      tm = localtime(&seconds);
    }

    /* update Time::TM_FIELDS when changing order of fields */
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

    RET(t3);
    CODE
  end

  def mktime
    <<-CODE
    time_t seconds;
    struct tm tm;
    char *old_tz, old_tz_buf[128];
    OBJECT t1, t2, t3, t4, t5, t6, t7, t8, t9, ret;
    
    old_tz = NULL;
   
    GUARD(REFERENCE_P(msg->recv));
    
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    POP(t4, FIXNUM);
    POP(t5, FIXNUM);
    POP(t6, FIXNUM);
    POP(t7, FIXNUM);
    POP(t8, FIXNUM);
    t9 = stack_pop();

    tm.tm_sec = N2I(t1);
    GUARD(tm.tm_sec >= 0 && tm.tm_sec <= 60);
    
    tm.tm_min = N2I(t2);
    GUARD(tm.tm_min >= 0 && tm.tm_min <= 60);
    
    tm.tm_hour = N2I(t3);
    GUARD(tm.tm_hour >= 0 && tm.tm_hour <= 24);
    
    tm.tm_mday = N2I(t4);
    GUARD(tm.tm_mday >= 1 && tm.tm_mday <= 31);
    
    tm.tm_mon = N2I(t5) - 1;
    GUARD(tm.tm_mon >= 0 && tm.tm_mon <= 11);
    
    tm.tm_year = N2I(t6) - 1900;

    /* In theory, we'd set the tm_isdst field to N2I(t8).
     * But since that will break on at least FreeBSD,
     * and I don't see the point of filling in that flag at all,
     * we're telling the system here to figure the DST stuff
     * out itmsg->recv.
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
    array_set(state, ret, 0, ML2N(seconds));
    array_set(state, ret, 1, t7);

    RET(ret);
    CODE
  end

  def fixnum_to_s
    <<-CODE
    OBJECT t1;
    static const char digitmap[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    char buf[100];
    char *b = buf + sizeof(buf);
    native_int j, k, m;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    j = N2I(t1);
    k = N2I(msg->recv);
    GUARD( j >= 2 && j <= 36 );

    /* Algorithm taken from 1.8.4 rb_fix2str */
    if(k == 0) {
      RET(string_new(state, "0"));
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
      RET(string_new(state, b));
    }
    CODE
  end

  def bignum_to_s
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    POP(t1, FIXNUM);

    RET(bignum_to_s(state, msg->recv, t1));
    CODE
  end

  def logical_class
    <<-CODE
    /* msg->recv is ANY object because object_class knows all. */
    RET(object_class(state, msg->recv));
    CODE
  end

  def object_id
    <<-CODE
    RET(UI2N(object_get_id(state, msg->recv)));
    CODE
  end

  def hash_set
    <<-CODE
    OBJECT t1, t2, t3;
    GUARD(HASH_P(msg->recv));

    POP(t1, FIXNUM);
    t2 = stack_pop(); // some type of object
    t3 = stack_pop(); // some type of object can we do an object guard?

    hash_add(state, msg->recv, N2I(t1), t2, t3);
    RET(t3);
    CODE
  end

  def hash_get
    <<-CODE
    OBJECT t1, t2, t3;
    GUARD(HASH_P(msg->recv));
    
    POP(t1, FIXNUM);
    t2 = stack_pop();
    t3 = hash_find_entry(state, msg->recv, N2I(t1));
    RET(t3);
    CODE
  end

  def hash_object
    <<-CODE
    OBJECT t1;
    /* msg->recv is ANY object because object_class knows all. */    
    t1 = UI2N(object_hash_int(state, msg->recv));
    RET(t1);
    CODE
  end
  
  def hash_delete
    <<-CODE
    OBJECT t1, t2;
    GUARD(HASH_P(msg->recv));
    
    POP(t1, FIXNUM);

    t2 = hash_delete(state, msg->recv, N2I(t1));
    RET(t2);
    CODE
  end
  
  def hash_value_set
    <<-CODE
    OBJECT t1;
    GUARD(REFERENCE_P(msg->recv));
    
    POP(t1, NUMERIC);
    abort();
    CODE
  end

  def symbol_index
    <<-CODE
    GUARD(SYMBOL_P(msg->recv));

    RET(I2N(symbol_to_index(state, msg->recv)));
    CODE
  end

  def symbol_lookup
    <<-CODE
    GUARD(STRING_P(msg->recv));
    GUARD(N2I(string_get_bytes(msg->recv)) > 0);
    RET(string_to_sym(state, msg->recv));
    CODE
  end

  def dup_into
    <<-CODE
    OBJECT t1, t2, t3;
    native_int k, j;

    GUARD(REFERENCE_P(msg->recv));
    
    POP(t1, REFERENCE);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);

    native_int start = N2I(t2);
    native_int dest = N2I(t3);
    for(k = start, j = dest; 
        k < NUM_FIELDS(t1) && j < NUM_FIELDS(msg->recv); 
        k++, j++) {
      SET_FIELD(msg->recv, j, NTH_FIELD(t1, k));
    }
    
    object_copy_nongc_flags(msg->recv, t1);
    
    RET(t1);
    CODE
  end
    
  def bytes_dup_into
    <<-CODE
    OBJECT t1;
    native_int k, j;

    GUARD(REFERENCE_P(msg->recv));
    
    POP(t1,   REFERENCE);

    GUARD(_object_stores_bytes(msg->recv));
    GUARD(_object_stores_bytes(t1));
    
    k = bytearray_bytes(state, msg->recv);
    j = bytearray_bytes(state, t1);
    
    if(j < k) { k = j; }
      
    memcpy(bytearray_byte_address(state, t1), 
           bytearray_byte_address(state, msg->recv), k);
           
    RET(t1);
    CODE
  end

  def object_dup
    <<-CODE
    OBJECT t2;
    native_int j, k;
    
    if(REFERENCE_P(msg->recv)) {
      j = NUM_FIELDS(msg->recv);
      t2 = NEW_OBJECT(object_class(state, msg->recv), j);
      if(msg->recv->StoresBytes) {
        memcpy(object_byte_start(state, t2), 
               object_byte_start(state, msg->recv), SIZE_OF_BODY(msg->recv));
        t2->StoresBytes = 1;
      } else {
        for(k = 0; k < j; k++) {
          SET_FIELD(t2, k, NTH_FIELD(msg->recv, k));
        }
      }
      
      stack_push(t2);
      object_copy_ivars(state, msg->recv, t2);
      cpu_perform_hook(state, c, t2, state->global->sym_init_copy, msg->recv);
      DONE();
    } else {
      RET(msg->recv);
    }
    CODE
  end
  
  def object_clone
    <<-CODE
    native_int k, j;
    OBJECT t2;

    GUARD(REFERENCE_P(msg->recv));
    
    j = NUM_FIELDS(msg->recv);
    t2 = NEW_OBJECT(object_class(state, msg->recv), j);
    if(msg->recv->StoresBytes) {
      memcpy(object_byte_start(state, t2), 
             object_byte_start(state, msg->recv), SIZE_OF_BODY(msg->recv));
      t2->StoresBytes = 1;
    } else {
      for(k = 0; k < j; k++) {
        SET_FIELD(t2, k, NTH_FIELD(msg->recv, k));
      }
    }
    
    stack_push(t2);
    object_copy_ivars(state, msg->recv, t2);
    object_copy_metaclass(state, msg->recv, t2);
    cpu_perform_hook(state, c, t2, state->global->sym_init_copy, msg->recv);
    DONE();
    CODE
  end
  
  
  def fastctx_dup
    <<-CODE
    GUARD(RISA(msg->recv, fastctx));
    
    RET(methctx_dup(state, msg->recv));
    CODE
  end

  def tuple_shifted
    <<-CODE
    OBJECT t1, t2;
    native_int j;

    GUARD(TUPLE_P(msg->recv));
    POP(t1, FIXNUM);

    j = N2I(t1);
    if(!j) {
      t2 = msg->recv;
    } else {
      t2 = tuple_new(state, NUM_FIELDS(msg->recv) + j);
      object_copy_fields_shifted(state, msg->recv, t2, j);
    }

    RET(t2);
    CODE
  end

  def get_byte
    <<-CODE
    OBJECT t1;
    native_int j, k;

    GUARD( object_stores_bytes_p(state, msg->recv) )
    POP(t1, FIXNUM); /* index */

    unsigned char *indexed;
    j = N2I(t1);
    k = bytearray_bytes(state, msg->recv);

    GUARD( j >= 0 && j < k );
    indexed = (unsigned char*)bytearray_byte_address(state, msg->recv);
    indexed += j;

    RET(UI2N(*indexed));
    CODE
  end

  def set_byte
    <<-CODE
    OBJECT t1, t2;
    native_int j, k;

    GUARD( object_stores_bytes_p(state, msg->recv) )
    POP(t1, FIXNUM); /* index */
    POP(t2, FIXNUM); /* value */

    unsigned char *indexed;
    j = N2I(t1);
    k = bytearray_bytes(state, msg->recv);

    GUARD( j >= 0 && j < k );
    
    indexed = (unsigned char*)bytearray_byte_address(state, msg->recv);
    indexed += j;
    t2 = UI2N(*indexed = N2I(t2));
    
    RET(t2);
    CODE
  end
    
  def fetch_bytes
    <<-CODE
    OBJECT t1, t2, t3;
    native_int j, k, m;

    GUARD( object_stores_bytes_p(state, msg->recv) )
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);

    char *source, *dest;
    native_int num;
    j = N2I(t1);
    k = N2I(t2);
    m = bytearray_bytes(state, msg->recv);

    num = abs(j - k);

    GUARD( m >= num );
    GUARD( k >= 0 );
    GUARD( j >= 0 && j <= m );

    t3 = bytearray_new(state, k+1);
    source = (char*)bytearray_byte_address(state, msg->recv);
    dest = (char*)bytearray_byte_address(state, t3);
    source += j;
    memcpy(dest, source, k);
    dest[k] = 0;

    RET(t3);
    CODE
  end
  
  def move_bytes
    <<-CODE
    OBJECT t1, t2, t3;
    char *data, *source, *dest;
    native_int total, offset, start, count;
    
    GUARD(object_stores_bytes_p(state, msg->recv));
    
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    
    start  = N2I(t1);
    count  = N2I(t2);
    offset = N2I(t3);
    
    total = bytearray_bytes(state, msg->recv);
        
    GUARD(start + count + offset < total);
    GUARD(offset >= 0);
    GUARD(count >= 0);
    
    data = (char*)bytearray_byte_address(state, msg->recv);
    source = data;
    dest = data;
    
    source += start;
    dest += offset;
        
    memmove((void*)dest, (void*)source, count);
    
    RET(t2);    
    CODE
  end

  def compare_bytes
    <<-CODE
    OBJECT t1, t2, t3;
    native_int j, k, m;
    native_int len, a, b, n, cmp;
    
    GUARD(object_stores_bytes_p(state, msg->recv));
    t1 = stack_pop();
    GUARD(object_stores_bytes_p(state, t1));
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    a = N2I(t2);   /* max bytes to compare in self */
    b = N2I(t3);   /* max bytes to compare in t1 */
    
    j = bytearray_bytes(state, msg->recv);
    k = bytearray_bytes(state, t1);
    m = j < a ? j : a;
    n = k < b ? k : b;
    
    /* regardless of the user's request, 
     * don't compare more bytes than there are
     */
    len = m < n ? m : n;
    
    cmp = memcmp(bytearray_byte_address(state, msg->recv), 
                 bytearray_byte_address(state, t1), len);
                 
    /* check against m and n, to allow a, b to be non-symmetric with j, k */
    if(cmp == 0) {
      if(m < n) {
        RET(I2N(-1));
      } else if(m > n) {
        RET(I2N(1));
      } else {
        RET(I2N(0));
      }
    } else {
      RET(cmp < 0 ? I2N(-1) : I2N(1));
    }
    CODE
  end

  def bytearray_size
    <<-CODE
    native_int j;
    GUARD( object_stores_bytes_p(state, msg->recv) )

    j = bytearray_bytes(state, msg->recv);
    
    RET(I2N(j));
    CODE
  end
  
  def load_file
    <<-CODE
    OBJECT t1, t2;

    POP(t1, STRING);
    POP(t2, FIXNUM);

    char *path = string_byte_address(state, t1);
    t2 = cpu_unmarshal_file(state, path, N2I(t2));
    
    RET(t2);
    CODE
  end
  
  def activate_as_script
    <<-CODE
    GUARD(CMETHOD_P(msg->recv));

    cpu_run_script(state, c, msg->recv);
    DONE();
    CODE
  end
  
  # TODO: Remove stat_file after rebuilding stables
  def stat_file
    <<-CODE
    OBJECT t1, t2, t3;
    native_int j;
    struct stat sb = {0};

    GUARD(CLASS_P(msg->recv));

    POP(t1, STRING);
    t2 = stack_pop();

    char *path = string_byte_address(state, t1);
    if (RTEST(t2)) {
      j = stat(path, &sb);
    } else {
      j = lstat(path, &sb);
    }

    if(j != 0) {
      RET(Qfalse);
    } else {
      t2 = NEW_OBJECT(msg->recv, 15);
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
      tuple_put(state, t2, 3, I2N((native_int)sb.st_uid));
      tuple_put(state, t2, 4, I2N((native_int)sb.st_gid));
      tuple_put(state, t2, 5, I2N((native_int)sb.st_size));
      tuple_put(state, t2, 6, I2N((native_int)sb.st_blocks));
      tuple_put(state, t2, 7, ML2N((long long)sb.st_atime));
      tuple_put(state, t2, 8, ML2N((long long)sb.st_mtime));
      tuple_put(state, t2, 9, ML2N((long long)sb.st_ctime));
      tuple_put(state, t2, 10, t1);
      tuple_put(state, t2, 11, UI2N((unsigned long)sb.st_blksize));
      tuple_put(state, t2, 12, UI2N((unsigned long)sb.st_dev));
      tuple_put(state, t2, 12, UI2N((unsigned long)sb.st_dev));
      
      OBJECT dev_major = Qnil, dev_minor = Qnil;

      #ifdef major
        dev_major = ML2N(major(sb.st_dev));
      #endif

      #ifdef minor
        dev_minor = ML2N(minor(sb.st_dev));
      #endif

      tuple_put(state, t2, 13, dev_major);
      tuple_put(state, t2, 14, dev_minor);

      RET(t2);
    }
    CODE
  end
  
  def process_exit
    <<-CODE
    OBJECT t1;

    POP(t1, FIXNUM);

#ifdef TIME_LOOKUP
    cpu_show_lookup_time(state);
#endif

    if(state->gc_stats) {
      printf("[GC M %6dK total]\\n", state->om->ms->allocated_bytes / 1024);
    }

    if(current_machine->sub) {
      environment_exit_machine(); 
    } else {
      exit(N2I(t1));
    }
    CODE
  end
  
  def micro_sleep
    <<-CODE
    OBJECT t1, t2;
    native_int j;
    struct timespec ts;
    POP(t1, FIXNUM);

    j = N2I(t1);
    ts.tv_sec = j / 1000000;
    ts.tv_nsec = (j % 1000000) * 1000;
      
    if(!nanosleep(&ts, NULL)) {
      t2 = Qfalse;
    } else {
      t2 = Qtrue;
    }

    RET(t2);
    CODE
  end
  
  def activate_context
    <<-CODE
    OBJECT t1;
    sassert(0);
    if(blokctx_s_block_context_p(state, msg->recv)) {
      t1 = blokctx_home(state, msg->recv);
    } else {
      t1 = msg->recv;
    }
    
    cpu_activate_context(state, c, msg->recv, t1, 0);
    CODE
  end
  
  def context_sender
    <<-CODE
    OBJECT t1;
    GUARD(CTX_P(msg->recv));

    t1 = FASTCTX(msg->recv)->sender;
    
    if(t1 != Qnil) {
      methctx_reference(state, t1);
    }
    
    RET(t1);
    CODE
  end
  
  def string_to_sexp
    <<-CODE
    bstring contents;
    const char *name;
    OBJECT t1, t2, t3;

    GUARD(STRING_P(msg->recv));

    POP(t1, STRING);
    POP(t2, FIXNUM);
    t3 = stack_pop();

    contents = cstr2bstr(string_byte_address(state, msg->recv));
    name = string_byte_address(state, t1);
    t1 = syd_compile_string(state, name, contents, N2I(t2), RTEST(t3));
    bdestroy(contents);
    
    RET(t1);
    CODE
  end
  
  def file_to_sexp
    <<-CODE
    FILE *file;
    char *name;
    OBJECT t1, t2;

    POP(t1, STRING); /* The filename */
    t2 = stack_pop();

    name = string_byte_address(state, t1);
    file = fopen(name, "r");

    if(!file) {
      FAIL();
    } else {
      t1 = syd_compile_file(state, name, file, 1, RTEST(t2));
      fclose(file);
      RET(t1);
    }
    CODE
  end

  def terminal_raw
    <<-CODE
    struct termios ts;
    int err;
    OBJECT t3;

    if(NULL == state->termios) {
      if(!isatty(STDOUT_FILENO)) {
        RET(Qfalse);
      }

      /* HACK: this memory is never freed */
      state->termios = ALLOC(struct termios);

      if(NULL == state->termios) {
        RET(Qfalse);
      }

      err = tcgetattr(STDOUT_FILENO, state->termios);

      if(err == -1) { /* TODO: handle errno */
        XFREE(state->termios);
        RET(Qfalse);
      }
    }

    err = tcgetattr(STDOUT_FILENO, &ts);
    
    if(err == -1) { /* TODO: handle errno */
      RET(Qfalse);
    }
    
    ts.c_lflag &= ~ICANON; /* -icanon */
    ts.c_lflag &= ~ISIG;   /* -isig */
    ts.c_lflag &= ~ECHO;   /* -echo */
    ts.c_cc[VMIN] = 1;     /* min 1 */
    
    err = tcsetattr(STDOUT_FILENO, TCSANOW, &ts);
    
    if(err == -1) { /* TODO: handle errno */
      RET(Qfalse);
    }

    t3 = tuple_new2(state, 4,
      I2N(ts.c_cc[VERASE]), I2N(ts.c_cc[VKILL]),
      I2N(ts.c_cc[VQUIT]),  I2N(ts.c_cc[VINTR])
    );

    RET(t3);
    CODE
  end

  def terminal_normal
    <<-CODE
    if(NULL == state->termios) {
      RET(Qfalse);
    } else if (isatty(STDOUT_FILENO)) {
      int err;

      err = tcsetattr(STDOUT_FILENO, TCSANOW, state->termios);

      if (err == -1) {
        RET(Qfalse);
      } else {
        RET(Qtrue);
      }
    } else {
      RET(Qfalse);
    }
    CODE
  end

  def regexp_new
    <<-CODE
    OBJECT t1, t2, t3;
    char err_buf[1024];
    
    GUARD(CLASS_P(msg->recv));
    POP(t1, STRING);
    t2 = stack_pop();
    t3 = regexp_new(state, t1, t2, err_buf);
    
    if(NIL_P(t3)) {
      RAISE("RegexpError", err_buf);
    } else {
      t3->klass = msg->recv;   /* Subclasses */
    }
    
    RET(t3);
    CODE
  end

  def regexp_match
    <<-CODE
    OBJECT t1;
    GUARD(REGEXP_P(msg->recv));

    POP(t1, STRING);
    
    RET(regexp_match(state, msg->recv, t1));
    CODE
  end

  def regexp_match_start
    <<-CODE
    OBJECT t1, t2;
    GUARD(REGEXP_P(msg->recv));

    POP(t1, STRING);
    POP(t2, FIXNUM);
    
    RET(regexp_match_start(state, msg->recv, t1, t2));
    CODE
  end

  def regexp_match_region
    <<-CODE
    OBJECT t1, t2, t3, t4, t5;
    GUARD(REGEXP_P(msg->recv));

    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    t4 = stack_pop();
    
    t5 = regexp_match_region(state, msg->recv, t1, t2, t3, t4);
    
    RET(t5);
    CODE
  end

  def regexp_scan
    <<-CODE
    OBJECT t1;
    GUARD(REGEXP_P(msg->recv));
    
    POP(t1, STRING);

    RET(regexp_scan(state, msg->recv, t1));
    CODE
  end  

  def regexp_options
    <<-CODE
    GUARD(REGEXP_P(msg->recv));
    
    RET(regexp_options(state, msg->recv));
    CODE
  end
  
  def gc_start
    <<-CODE
    OBJECT t1 = stack_pop();
    if(RTEST(t1)) {
      state->om->collect_now = OMCollectYoung;
    } else {
      state->om->collect_now = OMCollectYoung | OMCollectMature;
    }
    RET(Qtrue);
    CODE
  end
    
  def get_ivar
    <<-CODE
    OBJECT t1 = NTH_FIELD(msg->method, 4);
    RET(object_get_ivar(state, msg->recv, t1));
    CODE
  end
  
  def set_ivar
    <<-CODE
    OBJECT t1, t2;
    
    t1 = NTH_FIELD(msg->method, 4);
    t2 = stack_pop();
    object_set_ivar(state, msg->recv, t1, t2);

    RET(t2);
    CODE
  end

  # field 4: method to call
  # field 5: object to call method on
  # field 6: whether or not we need to retain 'msg->recv'
  def dispatch_as_method
    <<-CODE
    OBJECT t1, t2, t3; 
    int args = msg->args;
    
    t1 = NTH_FIELD(msg->method, 4);
    t2 = NTH_FIELD(msg->method, 5);
    t3 = NTH_FIELD(msg->method, 6);
    
    if(Qtrue == t3) {
      stack_push(msg->recv);
      args++;
    }

    cpu_send(state, c, t2, t1, args, Qnil);
    CODE
  end
  
  def set_index
    <<-CODE
    native_int j;
    GUARD(INDEXED(msg->recv));

    j = N2I(NTH_FIELD(msg->method, 4));
    SET_FIELD(msg->recv, j, stack_pop());
    
    RET(NTH_FIELD(msg->recv, j));  
    CODE
  end
  
  def get_index
    <<-CODE
    native_int j;
    GUARD(INDEXED(msg->recv));

    j = N2I(NTH_FIELD(msg->method, 4));
    RET(NTH_FIELD(msg->recv, j));
    CODE
  end

  def fixnum_modulo
    <<-CODE
    OBJECT t1;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    GUARD(N2I(t1) != 0) // no divide by zero
    long mod;
    fixnum_div(state, msg->recv, t1, &mod);
    RET(I2N(mod));
    CODE
  end
  
  def marshal_object
    <<-CODE
    OBJECT t1, t2;
    t1 = stack_pop();
    POP(t2, FIXNUM);

    RET(cpu_marshal(state, t1, N2I(t2)));
    CODE
  end
  
  def unmarshal_object
    <<-CODE
    OBJECT t1, t2, t3;
    POP(t1, STRING);
    POP(t2, FIXNUM);
    t3 = cpu_unmarshal(state, 
                       (uint8_t*)string_byte_address(state, t1), 
                       N2I(string_get_bytes(t1)), 
                       N2I(t2));
    RET(t3);
    CODE
  end
  
  def marshal_to_file
    <<-CODE
    OBJECT t1, t2, t3;

    char *_path;
    t1 = stack_pop();
    POP(t2, STRING);
    POP(t3, FIXNUM);

    _path = string_byte_address(state, t2);
    RET(cpu_marshal_to_file(state, t1, _path, N2I(t3)));
    CODE
  end
  
  def unmarshal_from_file
    <<-CODE
    char *_path;
    OBJECT t1, t2;

    POP(t1, STRING);
    POP(t2, FIXNUM);

    _path = string_byte_address(state, t1);
    RET(cpu_unmarshal_file(state, _path, N2I(t2)));
    CODE
  end
  
  def archive_files
    <<-CODE
    char *path;
    OBJECT t1;
    POP(t1, STRING);

    path = string_byte_address(state, t1);
    RET(archive_list_files(state, path));
    CODE
  end
  
  def archive_get_file
    <<-CODE
    char *path, *file;
    OBJECT t1, t2;
    
    POP(t1, STRING);
    POP(t2, STRING);

    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    
    RET(archive_get_file(state, path, file));
    CODE
  end
  
  def archive_get_object
    <<-CODE
    char *path, *file;
    OBJECT t1, t2, t3;
    
    POP(t1, STRING);
    POP(t2, STRING);
    POP(t3, FIXNUM);

    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    
    RET(archive_get_object(state, path, file, N2I(t3)));
    CODE
  end
  
  def archive_add_file
    <<-CODE
    char *path, *file, *data;
    OBJECT t1, t2, t3;
    
    POP(t1, STRING);
    POP(t2, STRING);
    POP(t3, STRING);
    
    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    data = string_byte_address(state, t3);
    
    t1 = archive_add_file(state, path, file, data);
    RET(t1);
    CODE
  end
  
  def archive_add_object
    <<-CODE
    char *path, *file;
    OBJECT t1, t2, t3, t4;
    
    POP(t1, STRING);
    POP(t2, STRING);
    t3 = stack_pop();
    POP(t4, FIXNUM);
    
    path = string_byte_address(state, t1);
    file = string_byte_address(state, t2);
    
    t1 = archive_add_object(state, path, file, t3, N2I(t4));
    RET(t1);
    CODE
  end
  
  def archive_delete_file
    <<-CODE
    OBJECT t1, t2, t3;

    POP(t1, STRING);
    POP(t2, FIXNUM);

    t3 = archive_delete_file(state, string_byte_address(state, t1), N2I(t2));
    RET(t3);
    CODE
  end  
  
  # FIXME: get rid of bignum awareness and use coerce
  # i.e. t1 = fixnum
  def fixnum_and
    <<-CODE
    OBJECT t1;
    native_int j, k, m;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, INTEGER);

    if(FIXNUM_P(t1)) {
      j = N2I(msg->recv);
      k = N2I(t1);
      m = j & k;
      RET(I2N(m));
    } else {
      RET(bignum_and(state, bignum_new(state, N2I(msg->recv)), t1));
    }
    CODE
  end

  # FIXME: get rid of bignum awareness and use coerce
  # i.e. t1 = fixnum
  def fixnum_or
    <<-CODE
    OBJECT t1;
    native_int j, k, m;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, INTEGER);

    if(FIXNUM_P(t1)) {
      j = N2I(msg->recv);
      k = N2I(t1);
      m = j | k;
      RET(I2N(m));
    } else {
      RET(bignum_or(state, bignum_new(state, N2I(msg->recv)), t1));
    }
    CODE
  end

  # FIXME: get rid of bignum awareness and use coerce
  # i.e. t1 = fixnum
  def fixnum_xor
    <<-CODE
    OBJECT t1;
    native_int j, k, m;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, INTEGER);

    if(FIXNUM_P(t1)) {
      j = N2I(msg->recv);
      k = N2I(t1);
      m = j ^ k;
      RET(I2N(m));
    } else {
      RET(bignum_xor(state, bignum_new(state, N2I(msg->recv)), t1));
    }
    CODE
  end

  def fixnum_invert
    <<-CODE
    native_int j;
    GUARD(FIXNUM_P(msg->recv));

    j = N2I(msg->recv);
    RET(I2N(~j));
    CODE
  end

  def fixnum_neg
    <<-CODE
    native_int j;
    GUARD(FIXNUM_P(msg->recv));
    
    j = N2I(msg->recv);
    RET(I2N(-j));
    CODE
  end

  def fixnum_right_shift
    <<-CODE
    OBJECT t1, t2;
    native_int value, width;
    
    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    value = N2I(msg->recv);
    width = N2I(t1);

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
    RET(t2);
    CODE
  end

  def fixnum_left_shift
    <<-CODE
    OBJECT t1, t2;
    native_int value, width;
    
    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    value = N2I(msg->recv);
    width = N2I(t1);

    value <<= width;
    t2 = I2N(value);
    RET(t2);
    CODE
  end

  def bignum_new
    <<-CODE
    OBJECT t1;
    POP(t1, FIXNUM);
    RET(bignum_new(state, N2I(t1)));
    CODE
  end
  
  def bignum_to_float
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));

    RET(float_new(state, bignum_to_double(state, msg->recv)));
    CODE
  end

  def bignum_and
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    POP(t1, INTEGER);

    RET(bignum_and(state, msg->recv, t1));
    CODE
  end

  def bignum_or
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    
    POP(t1, INTEGER);

    RET(bignum_or(state, msg->recv, t1));
    CODE
  end

  def bignum_xor
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    
    POP(t1, INTEGER);

    RET(bignum_xor(state, msg->recv, t1));
    CODE
  end

  def bignum_neg
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));

    RET(bignum_neg(state, msg->recv));
    CODE
  end

  def bignum_invert
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));

    RET(bignum_invert(state, msg->recv));
    CODE
  end
  
  def numeric_coerce
    <<-CODE
    OBJECT t1, t3;

    GUARD(INTEGER_P(msg->recv));
    POP(t1, INTEGER);

    t3 = array_new(state, 2);
    if(BIGNUM_P(msg->recv)) {
      if(BIGNUM_P(t1)) {
        array_set(state, t3, 0, t1);
      } else {
        array_set(state, t3, 0, bignum_new(state, N2I(t1)));
      }
      array_set(state, t3, 1, msg->recv);
    } else {
      if(BIGNUM_P(t1)) {
        array_set(state, t3, 0, t1);
        array_set(state, t3, 1, bignum_new(state, N2I(msg->recv)));
      } else {
        array_set(state, t3, 0, t1);
        array_set(state, t3, 1, msg->recv);
      }
    }
    RET(t3);
    CODE
  end
  
  def bignum_compare
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    POP(t1, BIGNUM);

    RET(bignum_compare(state, msg->recv, t1));
    CODE
  end
  
  def fixnum_to_f
    <<-CODE
    GUARD(FIXNUM_P(msg->recv));

    RET(float_new(state, FIXNUM_TO_DOUBLE(msg->recv)));
    CODE
  end
  
  def string_to_f
    <<-CODE
    GUARD(STRING_P(msg->recv));

    RET(float_new(state, string_to_double(state, msg->recv)));
    CODE
  end
  
  def fixnum_divmod
    <<-CODE
    OBJECT t1;
    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    GUARD( N2I(t1) != 0 ) // no divide by zero
    RET(fixnum_divmod(state, msg->recv, t1));
    CODE
  end
  
  def bignum_left_shift
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    
    POP(t1, FIXNUM);

    RET(bignum_left_shift(state, msg->recv, t1));
    CODE
  end

  def bignum_right_shift
    <<-CODE
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    
    POP(t1, FIXNUM);

    RET(bignum_right_shift(state, msg->recv, t1));
    CODE
  end

  def find_method
    <<-CODE
    OBJECT t1, t2, t3;
    POP(t1, SYMBOL);
    t1 = exported_cpu_find_method(state, c, msg->recv, t1, &t2);
    t3 = tuple_new(state, 2);
    
    tuple_put(state, t3, 0, t2);
    tuple_put(state, t3, 1, t1);
    
    RET(t3);
    CODE
  end
  
  def bignum_divmod
    <<-CODE
    OBJECT t1;

    GUARD(BIGNUM_P(msg->recv));
    
    POP(t1, BIGNUM); 
    
    // no divide by zero
    GUARD(!bignum_is_zero(state, t1));

    RET(bignum_divmod(state, msg->recv, t1));
    CODE
  end
  
  def object_taint
    <<-CODE
    GUARD(REFERENCE_P(msg->recv));
    object_set_tainted(state, msg->recv);
    RET(msg->recv);
    CODE
  end
  
  def object_tainted_p
    <<-CODE
    GUARD(REFERENCE_P(msg->recv));
    RET(object_tainted_p(state, msg->recv) ? Qtrue : Qfalse);
    CODE
  end
  
  def object_untaint
    <<-CODE
    GUARD(REFERENCE_P(msg->recv));
    object_set_untainted(state, msg->recv);
    RET(msg->recv);
    CODE
  end
  
  def object_freeze
    <<-CODE
    GUARD(REFERENCE_P(msg->recv));
    object_set_frozen(state, msg->recv);
    RET(msg->recv);
    CODE
  end
  
  def object_frozen_p
    <<-CODE
    GUARD(REFERENCE_P(msg->recv));
    RET(object_frozen_p(state, msg->recv) ? Qtrue : Qfalse);
    CODE
  end
  
  def fastctx_get_field
    <<-CODE
    native_int i;
    struct fast_context *fc;
    OBJECT t1;

    GUARD(CTX_P(msg->recv));
    POP(t1, FIXNUM);

    fc = FASTCTX(msg->recv);
    i = N2I(t1);

    switch(i) {
      case 0:
        if(!NIL_P(fc->sender)) {
          methctx_reference(state, fc->sender);
        }
        RET(fc->sender);
        break;
      case 1:
        RET(I2N(fc->ip));
        break;
      case 2:
        RET(I2N(fc->sp));
        break;
      case 3:
        RET(fc->block);
        break;
      case 5:
        RET(fc->method);
        break;
      case 6:
        RET(fc->literals);
        break;
      case 7:
        RET(fc->self);
        break;
      case 8:
        RET(fc->locals);
        break;
      case 9:
        RET(I2N(fc->argcount));
        break;
      case 10:
        RET(fc->name);
        break;
      case 11:
        RET(fc->method_module);
        break;
      case 12:
        RET(I2N(fc->flags));
        break;
      case 13:
        RET(I2N(fc->fp));
        break;
      default:
        RET(Qnil);
    }
    CODE
  end
  
  def fastctx_set_field
    <<-CODE
    native_int i;
    struct fast_context *fc;
    OBJECT t2;

    GUARD(CTX_P(msg->recv));
    
    POP(t2, FIXNUM);
    i = N2I(t2);
    
    t2 = stack_pop();
    fc = FASTCTX(msg->recv);
    
    switch(i) {
      case 0:
        GUARD(CTX_P(t2));
        fc->sender = t2;
        break;
      case 1:
        GUARD(FIXNUM_P(t2));
        fc->ip = N2I(t2);
        break;
      case 2:
        GUARD(FIXNUM_P(t2));
        fc->sp = N2I(t2);
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
        fc->argcount = N2I(t2);
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
          fc->flags |= N2I(t2);
        }
        break;
      default:
        FAIL();
    }

    RET(t2);
    CODE
  end

  def fastctx_reload_method
    <<-CODE
    struct fast_context *fc;
    GUARD(CTX_P(msg->recv));

    fc = FASTCTX(msg->recv);
    if(fc->method->obj_type == CMethodType) {
      fc->data = BYTEARRAY_ADDRESS(cmethod_get_compiled(fc->method));
    }

    RET(Qtrue);
    CODE
  end

  def fastctx_set_iseq
    <<-CODE
    struct fast_context *fc;
    OBJECT t1;

    GUARD(CTX_P(msg->recv));

    POP(t1, BYTEARRAY);

    fc = FASTCTX(msg->recv);
    if(fc->method->obj_type == CMethodType) {
      cpu_compile_method(state, fc->method);
      fc->data = BYTEARRAY_ADDRESS(t1);
    }

    RET(Qtrue);
    CODE
  end

  def vm_stats
    <<-CODE
#ifdef TRACK_STATS
    OBJECT t1 = tuple_new(state, 7);
    tuple_put(state, t1, 0, I2N(state->cache_hits));
    tuple_put(state, t1, 1, I2N(state->cache_misses));
    tuple_put(state, t1, 2, I2N(state->cache_used));
    tuple_put(state, t1, 3, I2N(state->cache_collisions));
    tuple_put(state, t1, 4, I2N(state->cache_inline_hit));
    tuple_put(state, t1, 5, I2N(state->cache_inline_stale));
    tuple_put(state, t1, 6, I2N(state->cache_inline_const_hit));
    RET(t1);
#else
    RET(Qfalse);
#endif
    CODE
  end
  
  def nmethod_call
    <<-CODE
    OBJECT t1;
    cpu_flush_ip(c);
    cpu_flush_sp(c);
    cpu_save_registers(state, c, msg->args);
    t1 = nmc_new(state, msg->method, c->active_context, msg->recv, msg->name, msg->args);
    nmc_activate(state, c, t1, Qnil, FALSE);
    CODE
  end

  def nfunc_call
    <<-CODE
    /* The definition of beauty. Simplicity. To call a native function, there is no work 
       to be done. The stub contains all the serialization code. 
       
       That being said, this might get more complicated when callbacks are supported. */
    cpu_patch_ffi(state, msg);
    ffi_call(state, c, nfunc_get_data(msg->method));
    CODE
  end
  
  def nfunc_call_object
    <<-CODE
    /* The definition of beauty. Simplicity. To call a native function, there is no work 
       to be done. The stub contains all the serialization code. 
       
       That being said, this might get more complicated when callbacks are supported. */
    ffi_call(state, c, nfunc_get_data(msg->recv));
    CODE
  end
  
  def nfunc_add
    <<-CODE
    OBJECT t1, t2, t3, t4;

    POP(t1, STRING_OR_NIL);
    POP(t2, STRING);
    POP(t3, ARRAY);
    POP(t4, FIXNUM);
    RET(ffi_function_create(state, t1, t2, t3, t4));
    CODE
  end
  
  def load_library
    <<-CODE
    OBJECT t1, t2;
    
    POP(t1, STRING);
    POP(t2, STRING);
    RET(subtend_load_library(state, c, t1, t2));
    CODE
  end
  
  def dir_glob
    <<-CODE
    glob_t gd;
    char *pat;
    int flags = GLOB_NOSORT | GLOB_BRACE;
    OBJECT t1, t2;
    native_int j, k;

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
    RET(t2);
    CODE
  end
  
  def dir_chdir
    <<-CODE
    char *path;
    OBJECT t1;

    POP(t1, STRING);
    
    path = string_byte_address(state, t1);
    if(!chdir(path)) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end

  def yield_gdb
    <<-CODE
    OBJECT t1 = stack_pop();
    *((OBJECT*)4) = t1; /* cause a SIGBUS */
    RET(Qtrue);
    CODE
  end
  
  def make_weak_ref
    <<-CODE
    OBJECT t1;
    POP(t1, REFERENCE);
    
    RET(object_make_weak_ref(state, t1));
    CODE
  end
  
  def gc_collect_references
    <<-CODE
    OBJECT t1;
    POP(t1, REFERENCE);
    
    RET(object_memory_collect_references(state, state->om, t1));
    CODE
  end

  def task_dup
    <<-CODE
    OBJECT t1;
    /* This is a little contrived so the dup'd task has
       it stack setup properly. */
    
    stack_push(Qnil);
    
    if(TASK_P(msg->recv)) {
      t1 = cpu_task_dup(state, c, msg->recv);
    } else {
      t1 = cpu_task_dup(state, c, Qnil);
    }
    
    cpu_stack_set_top(state, c, t1);
    CODE
  end
  
  def task_set_current
    <<-CODE
    OBJECT t1;
    POP(t1, TASK);
    
    stack_push(Qnil);    
    cpu_task_select(state, c, t1);
    CODE
  end
  
  def task_associate
    <<-CODE
    OBJECT t1;

    GUARD(TASK_P(msg->recv));
    POP(t1, BLOCKENV);
    
    RET(cpu_task_associate(state, c, msg->recv, t1));
    CODE
  end
  
  def task_current
    <<-CODE
    RET(c->current_task);
    CODE
  end
  
  def task_at
    <<-CODE
    struct cpu_task *task;
    OBJECT t1, t2;
    native_int k;

    GUARD(TASK_P(msg->recv));
    POP(t1, FIXNUM);
    
    task = (struct cpu_task*)BYTES_OF(msg->recv);
    k = N2I(t1);
    
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
    
    RET(t2);
    CODE
  end
  
  def task_set_debugging
    <<-CODE
    OBJECT t1, t2;
    GUARD(TASK_P(msg->recv));

    t1 = stack_pop();
    t2 = stack_pop();

    GUARD(t1 == Qnil || CHANNEL_P(t1));
    GUARD(t2 == Qnil || CHANNEL_P(t2));

    if(msg->recv == c->current_task) {
      c->debug_channel = t1;
      c->control_channel = t2;
    } else {
      cpu_task_set_debugging(state, msg->recv, t1, t2);
    }
    RET(Qtrue);
    CODE
  end

  def task_debug_channel
    <<-CODE
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));
    
    task = (struct cpu_task*)BYTES_OF(msg->recv);

    RET(task->debug_channel);
    CODE
  end
  
  def task_control_channel
    <<-CODE
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));
    
    task = (struct cpu_task*)BYTES_OF(msg->recv);

    RET(task->control_channel);
    CODE
  end

  def task_get_debug_context_change
    <<-CODE
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));
    
    task = (struct cpu_task*)BYTES_OF(msg->recv);
    if(TASK_FLAG_P(task, TASK_DEBUG_ON_CTXT_CHANGE)) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end
  
  def task_set_debug_context_change
    <<-CODE
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));
    OBJECT t1 = stack_pop();
    
    task = (struct cpu_task*)BYTES_OF(msg->recv);
    if(RTEST(t1)) {
      TASK_SET_FLAG(task, TASK_DEBUG_ON_CTXT_CHANGE);
      if(task->active) {
        TASK_SET_FLAG(c, TASK_DEBUG_ON_CTXT_CHANGE);
      }
      RET(Qtrue);
    } else {
      if(TASK_FLAG_P(task, TASK_DEBUG_ON_CTXT_CHANGE)) {
        TASK_CLEAR_FLAG(task, TASK_DEBUG_ON_CTXT_CHANGE);
        if(task->active) {
          TASK_CLEAR_FLAG(c, TASK_DEBUG_ON_CTXT_CHANGE);
        }
      }
      RET(Qfalse);
    }
    CODE
  end
  
  def task_stack_size
    <<-CODE
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));

    task = (struct cpu_task*)BYTES_OF(msg->recv);
    OBJECT t1 = I2N(task->sp_ptr - task->stack_top);

    RET(t1);
    CODE
  end

  def task_get_stack_value
    <<-CODE
    struct cpu_task *task;
    OBJECT t1, t2;
    
    GUARD(TASK_P(msg->recv));
    POP(t1, FIXNUM);

    task = (struct cpu_task*)BYTES_OF(msg->recv);
    int idx = N2I(t1);
    GUARD(idx >=0 && idx < (task->sp_ptr - task->stack_top));

    t2 = *(task->sp_ptr - idx);

    RET(t2);
    CODE
  end

  def task_raise
    <<-CODE
    OBJECT t1;
    GUARD(TASK_P(msg->recv));
    
    t1 = stack_pop();
    
    /* The return value */
    stack_push(Qnil);
    
    /* This is conditional because task_select can decide that it's not
       not possible to select this task, in which case it handles letting
       the user know this on it's own. */
        
    if(cpu_task_select(state, c, msg->recv)) {
      cpu_raise_exception(state, c, t1);
    }
    CODE
  end
  
  def thread_raise
    <<-CODE
    OBJECT t1;
    GUARD(THREAD_P(msg->recv));
    
    t1 = stack_pop();

    if(!cpu_thread_alive_p(state, msg->recv)) {
      RET(Qfalse);
    } else {

      /* The return value */
      stack_push(Qtrue);

      cpu_thread_schedule(state, c->current_thread);    
      cpu_thread_force_run(state, c, msg->recv);

      methctx_reference(state, c->active_context);
      exception_set_context(t1, c->active_context);

      cpu_raise_exception(state, c, t1);
    }
    CODE
  end
  
  def channel_new
    <<-CODE
    RET(cpu_channel_new(state));
    CODE
  end
  
  def channel_send
    <<-CODE
    OBJECT t1;
    GUARD(CHANNEL_P(msg->recv));
    
    t1 = stack_pop();
    RET(cpu_channel_send(state, c, msg->recv, t1));
    CODE
  end
  
  def channel_receive
    <<-CODE
    GUARD(CHANNEL_P(msg->recv));
    
    cpu_channel_receive(state, c, msg->recv, c->current_thread);
    /* Don't touch the stack as we may be in a different task at this
       point. The original task's stack is updated when the channel
       is written to and the task restored. */
    CODE
  end
  
  def channel_send_in_microseconds
    <<-CODE
    double seconds;
    OBJECT t2, t1;
    native_int k;

    POP(t2, CHANNEL);
    POP(t1, INTEGER);
    
    if(FIXNUM_P(t1)) {
      k = (long)N2I(t1);
    } else {
      k = (long)bignum_to_int(state, t1);
    }
    
    seconds = k / 1000000.0;
    
    RET(cpu_event_wake_channel(state, c, t2, seconds));
    CODE
  end

  def channel_send_in_seconds
    <<-CODE
    double seconds;
    OBJECT t2, t1;

    POP(t2, CHANNEL);
    POP(t1, FLOAT);

    seconds = FLOAT_TO_DOUBLE(t1);

    RET(cpu_event_wake_channel(state, c, t2, seconds));
    CODE
  end

  def channel_send_on_readable
    <<-CODE
    OBJECT t1, t2, t3, t4;
    native_int j;

    POP(t4, CHANNEL);
    t1 = stack_pop();
    t2 = stack_pop();
    t3 = stack_pop();
    
    GUARD(STRING_P(t2) || NIL_P(t2));
    GUARD(FIXNUM_P(t3) || NIL_P(t3));

    if(IO_P(t1)) {
      j = io_to_fd(t1);
    } else if(FIXNUM_P(t1)) {
      j = N2I(t1);
    } else {
      GUARD(0);
    }
    
    RET(cpu_event_wait_readable(state, c, t4, j, t2, N2I(t3)));
    CODE
  end
  
  def channel_send_on_writable
    <<-CODE
    OBJECT t1, t2;
    native_int j;

    POP(t2, CHANNEL);
    POP(t1, IO);
    
    j = io_to_fd(t1);
    RET(cpu_event_wait_writable(state, c, t2, j));
    CODE
  end
  
  def channel_send_on_signal
    <<-CODE
    OBJECT t1, t2;

    POP(t2, CHANNEL);
    POP(t1, FIXNUM);
    
    RET(cpu_event_wait_signal(state, c, t2, N2I(t1)));
    CODE
  end
  
  def channel_send_on_stopped
    <<-CODE
    OBJECT t1, t2, t3;

    POP(t3, CHANNEL);
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);
    
    RET(cpu_event_wait_child(state, c, t3, N2I(t1), N2I(t2)));
    CODE
  end

  def scheduler_cancel
    <<-CODE
    OBJECT t1;
    POP(t1, FIXNUM);

    RET(cpu_event_cancel_event(state, t1) ? Qtrue : Qfalse);
    CODE
  end
  
  def thread_new
    <<-CODE
    RET(cpu_thread_new(state, c));
    CODE
  end
  
  def thread_run
    <<-CODE
    GUARD(THREAD_P(msg->recv));
    
    GUARD(cpu_thread_alive_p(state, msg->recv));
    
    /* So when we're restored, there is a ret val. */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);    
    cpu_thread_force_run(state, c, msg->recv);
    CODE
  end
  
  def thread_schedule
    <<-CODE
    GUARD(THREAD_P(msg->recv));
    
    cpu_thread_schedule(state, msg->recv);
    RET(Qnil);
    CODE
  end
  
  def thread_yield
    <<-CODE
    /* Same reason as thread_run */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);
    THDEBUG("%d: thread yield.\\n", getpid());
    cpu_thread_run_best(state, c);
    CODE
  end
  
  def thread_dequeue
    <<-CODE
    THDEBUG("%d: dequeue thread.\\n", getpid());
    cpu_thread_exited(state, c);
    CODE
  end
  
  def thread_current
    <<-CODE
    RET(c->current_thread);
    CODE
  end
  
  def object_become
    <<-CODE
    void state_object_become(STATE, cpu c, OBJECT from, OBJECT to);
    OBJECT t1, t2;

    POP(t2, REFERENCE);
    POP(t1, REFERENCE);
    state_object_become(state, c, t2, t1);
    RET(t2);
    CODE
  end
  
  def sampler_activate
    <<-CODE
    OBJECT t1;

    POP(t1, FIXNUM);
    cpu_sampler_activate(state, N2I(t1));
    RET(ML2N(clock()));
    CODE
  end
  
  def sampler_stop
    <<-CODE
    OBJECT t1 = cpu_sampler_disable(state);
    RET(t1);
    CODE
  end
  
  def fork_process
    <<-CODE
    native_int k = fork();
    if(k == -1) {
      RAISE_FROM_ERRNO("Unable to fork");
    } else {
      if (k == 0) {
        environment_fork();
      }
      RET(I2N(k));
    }
    CODE
  end
  
  # aka execve().
  def replace_process
    <<-CODE
    native_int i, j, k;
    char *tmp, *file;
    char **argv;
    OBJECT t1, t2, t3;
    
    POP(t1, STRING);
    POP(t2, ARRAY);
    
    k = N2I(array_get_total(t2));
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
    RAISE_FROM_ERRNO("Unable to execute");
    
    CODE
  end

  def ivar_get
    <<-CODE
    OBJECT t1;
    POP(t1, SYMBOL);
    RET(object_get_ivar(state, msg->recv, t1));
    CODE
  end
  
  def ivar_set
    <<-CODE
    OBJECT t1, t2;
    POP(t1, SYMBOL);
    t2 = stack_pop();
    object_set_ivar(state, msg->recv, t1, t2);
    RET(t2);
    CODE
  end
  
  def ivars_get
    <<-CODE
    RET(object_get_ivars(state, msg->recv));
    CODE
  end
  
  def str_crypt
    <<-CODE
    OBJECT t1;

    GUARD(STRING_P(msg->recv));
    POP(t1, STRING);
    RET(string_new(state, crypt(string_byte_address(state, msg->recv),
        string_byte_address(state, t1))));
    CODE
  end
    
  def env_get
    <<-CODE
    char *key;
    OBJECT t1, t2;

    POP(t1, STRING);

    t2 = Qnil;

    key = string_byte_address(state, t1);
    if (key) {
      char *value = getenv(key);

      if (value) {
        t2 = string_new(state, value);
      }
    }

    RET(t2);
    CODE
  end

  def env_set
    <<-CODE
    char *key, *value;
    OBJECT t1, t2;

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
          unsetenv(key);
          RET(string_new(state, value));
        } else {
          RET(Qnil);
        }
      } else {
        GUARD(STRING_P(t2));
        value = string_byte_address(state, t2);
        if(value) {
          setenv(key, value, 1);
          RET(t2);
        } else {
          RET(Qnil);
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

    RET(hash);
    CODE
  end

  def bignum_size
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    RET(bignum_size(state, msg->recv));
    CODE
  end

  def iseq_compile
    <<-CODE
    GUARD(CMETHOD_P(msg->recv));
    cpu_compile_method(state, msg->recv);
    RET(Qtrue);
    CODE
  end

  def reset_method_cache
    <<-CODE
    OBJECT t1;
    POP(t1, SYMBOL);
    cpu_clear_cache_for_method(state, c, t1, TRUE);
    CODE
  end
  
  def bignum_from_float
    <<-CODE
    OBJECT t1;
    POP(t1, FLOAT);
    RET(bignum_from_double(state, FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  def save_encloser_path
    <<-CODE
    cpu_set_encloser_path(state, c, state->global->object);
    RET(Qnil);
    CODE
  end

  def restore_encloser_path
    <<-CODE
    cpu_push_encloser(state, c);
    RET(Qnil);
    CODE
  end
  
  def array_aref
    <<-CODE
    OBJECT t1, t3;
    native_int j, k;

    GUARD(msg->args == 1);
    t1 = stack_top();
    GUARD(FIXNUM_P(t1));

    j = N2I(array_get_total(msg->recv));

    k = N2I(t1);

    if(k < 0) k += j;
    
    if(k < 0 || k >= j) {
      stack_set_top(Qnil);
    } else {
      k += N2I(array_get_start(msg->recv));
      t3 = array_get_tuple(msg->recv);
      GUARD(k < NUM_FIELDS(t3));
      
      stack_set_top(tuple_at(state, t3, k));
    }
    CODE
  end
  
  def array_aset
    <<-CODE
    OBJECT t1, t3;
    native_int j, k;
    
    GUARD(msg->args == 2);
    t1 = stack_pop();
    GUARD(FIXNUM_P(t1));

    j = N2I(array_get_total(msg->recv));    
    k = N2I(t1);
  
    if(k < 0) k += j;
    GUARD(k >= 0);
    
    if(k >= j - 1) {
      array_set_total(msg->recv, I2N(k + 1));
    }

    k += N2I(array_get_start(msg->recv));
    t3 = array_get_tuple(msg->recv);
    GUARD(k < NUM_FIELDS(t3));
    
    tuple_put(state, t3, k, stack_top());
    CODE
  end
  
  def string_append
    <<-CODE
    OBJECT t1;

    GUARD(msg->args == 1);
    POP(t1, STRING);
    
    string_append(state, msg->recv, t1);
    RET(msg->recv);
    CODE
  end
  
  def string_dup
    <<-CODE
    RET(string_dup(state, msg->recv));
    CODE
  end
  
  def string_equal
    <<-CODE
    OBJECT t1, t2, t3;
    native_int j, k, m;

    POP(t1, STRING);
    
    if(msg->recv == t1) {
      RET(Qtrue);
    } else {
      t2 = string_get_bytes(msg->recv);
      t3 = string_get_bytes(t1);
      
      if(t2 != t3) {
        RET(Qfalse);
      } else {
        j = N2I(t2);
        k = N2I(t3);
        t2 = string_get_data(msg->recv);
        t3 = string_get_data(t1);

        m = memcmp(BYTEARRAY_ADDRESS(t2), BYTEARRAY_ADDRESS(t3), j < k ? j : k);
        RET(m == 0 ? Qtrue : Qfalse);
      }
    }
    CODE
  end
  
  def object_send
    <<-CODE
    OBJECT t1;
    GUARD(msg->args >= 1);
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
    
    cpu_send(state, c, msg->recv, t1, msg->args - 1, msg->block);
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

    POP(ary, ARRAY);

    argc = N2I(array_get_total(ary));
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
    
    DONE();
    CODE
  end

  def machine_join
    <<-CODE
    OBJECT t1;
    POP(t1, FIXNUM);
    if(environment_join_machine(environment_current(), N2I(t1))) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end

  def machine_get_message
    <<-CODE
    RET(environment_get_message(environment_current(), current_machine->id));
    CODE
  end

  def machine_send_message
    <<-CODE
    OBJECT t1, t2;
    POP(t1, FIXNUM);
    t2 = stack_pop();

    environment_send_message(environment_current(), N2I(t1), t2);

    RET(Qtrue);
    CODE
  end
  
  def dir_open
    <<-CODE
    OBJECT t1;
    DIR *dir;

    POP(t1, STRING);

    dir = opendir(string_byte_address(state, t1));
    if(!dir) RAISE_FROM_ERRNO("Unable to open directory");

    RET(ffi_new_pointer(state, dir));
    CODE
  end
  
  def dir_close
    <<-CODE
    DIR *dir;
    OBJECT t1;

    POP(t1, POINTER); 

    dir = *DATA_STRUCT(t1, void**);
    if(dir) {
      *DATA_STRUCT(t1, void**) = NULL;
      closedir(dir);
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end
  
  def dir_control
    <<-CODE
    OBJECT kind, pos;
    OBJECT t1;
    POP(t1, POINTER);
    DIR *dir = *DATA_STRUCT(t1, void**);

    POP(kind, FIXNUM);
    POP(pos, FIXNUM);
    
    switch(N2I(kind)) {
    case 0:
      seekdir(dir, N2I(pos));
      RET(Qtrue);
    case 1:
      rewinddir(dir);
      RET(Qtrue);
    case 2:
      RET(I2N(telldir(dir)));
    default:
      RET(Qnil);
    }
    CODE
  end
  
  def dir_read
    <<-CODE
    struct dirent *ent;
    OBJECT t1;
    POP(t1, POINTER); 
    DIR *dir = *DATA_STRUCT(t1, void**);
    ent = readdir(dir);
    
    if(!ent) {
      RET(Qnil);
    } else {
      RET(string_new(state, ent->d_name));
    }

    CODE
  end

  def opt_push_literal
    <<-CODE
    OBJECT lits;
    
    lits = cmethod_get_literals(msg->method);
    RET(fast_fetch(lits, 0));
    CODE
  end
  
  def opt_push_self
    <<-CODE
    RET(msg->recv);
    CODE
  end
  
  def opt_push_ivar
    <<-CODE
    OBJECT lits;
    
    lits = cmethod_get_literals(msg->method);
    RET(object_get_ivar(state, msg->recv, fast_fetch(lits, 0)));
    CODE
  end

  def opt_push_my_field
    <<-CODE
    OBJECT lits;
    
    lits = cmethod_get_literals(msg->method);
    RET(NTH_FIELD(msg->recv, N2I(fast_fetch(lits, 0))));
    CODE
  end
  
  def float_add
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(float_new(state, a + FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  def float_sub
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(float_new(state, a - FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  def float_mul
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(float_new(state, a * FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  def float_div
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(float_new(state, a / FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  def float_uminus
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    RET(float_new(state, -a));
    CODE
  end

  def float_equal
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(a == FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    CODE
  end
  
  def float_eql
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FLOAT_P(t1)) {
      double a = FLOAT_TO_DOUBLE(msg->recv),
             b = FLOAT_TO_DOUBLE(t1);
      RET(a == b ? Qtrue : Qfalse);
    } else {
      RET(Qfalse);
    }
    CODE
  end

  def float_compare
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    double b = FLOAT_TO_DOUBLE(t1);
    if(a < b) {
      RET(I2N(-1));
    } else if(a > b) {
      RET(I2N(1));
    } else {
      RET(I2N(0));
    }
    CODE
  end

  def float_lt
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(a < FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    CODE
  end

  def float_le
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(a <= FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    CODE
  end

  def float_gt
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(a > FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    CODE
  end

  def float_ge
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(a >= FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    CODE
  end

  def float_to_i
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double b = FLOAT_TO_DOUBLE(msg->recv);
    if(isinf(b)) {
      RAISE("FloatDomainError", b < 0 ? "-Infinity" : "Infinity");
    } else if(isnan(b)) {
      RET(msg->recv);
    } else {
      RET(float_to_i_prim(state, b));
    }
    CODE
  end

  def opt_kind_of
    <<-CODE
    OBJECT t1;
  
    POP(t1, REFERENCE);
    GUARD(CLASS_P(t1) || MODULE_P(t1));
  
    RET(object_kind_of_p(state, msg->recv, t1) ? Qtrue : Qfalse);
    CODE
  end

  def float_round
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double value = FLOAT_TO_DOUBLE(msg->recv);
    if (value > 0.0) value = floor(value+0.5);
    if (value < 0.0) value = ceil(value-0.5);
    RET(bignum_from_double(state, value));
    CODE
  end

  def float_divmod
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    double b = FLOAT_TO_DOUBLE(t1);
    if(b == 0.0) {
      RAISE("FloatDomainError", "divide by 0");
    } else {
      double div = floor(a / b);
      double mod = fmod(a, b);
      if(b * mod < 0) {
        mod += b;
      }
      OBJECT ary = array_new(state, 2);
      array_set(state, ary, 0, float_to_i_prim(state, div));
      array_set(state, ary, 1, float_new(state, mod));
      RET(ary);
    }
    CODE
  end

  def float_pow
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    RET(float_new(state, pow(a, FLOAT_TO_DOUBLE(t1))));
    CODE
  end

  def float_isnan
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    RET(isnan(FLOAT_TO_DOUBLE(msg->recv)) == 1 ? Qtrue : Qfalse);
    CODE
  end

  def float_isinf
    <<-CODE
    GUARD(FLOAT_P(msg->recv));
    double value = FLOAT_TO_DOUBLE(msg->recv);
    if(isinf(value) != 0) {
      RET(value < 0 ? I2N(-1) : I2N(1));
    } else {
      RET(Qnil);
    }
    CODE
  end

  def bignum_gt
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_gt(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(a > FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  def bignum_ge
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_ge(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(a >= FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  def bignum_lt
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_lt(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(a < FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  def bignum_le
    <<-CODE
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_le(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(a <= FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end
end

prim = ShotgunPrimitives.new
prim.generate_select(STDOUT)
