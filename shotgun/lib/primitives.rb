class ShotgunPrimitives
  @@primitives = []

  def self.defprim(symbol)
    @@primitives << symbol
  end

  def generate_select(fd, op="prim")
    primitives = @@primitives

    File.open("primitive_implementation.gen", "w") do |f|
      primitives.each do |prim_name|
        f.puts "int cpu_primitive_#{prim_name}(STATE, cpu c, const struct message *msg) {"
        f.puts send(prim_name)
        f.puts "  DONE();\n}"
      end

      f.puts "\nprim_func cpu_lookup_primitive(int index) {"
      f.puts "  static prim_func funcs[] = { NULL, "

      primitives.each do |prim_name|
        f.puts "    cpu_primitive_#{prim_name},"
      end

      f.puts "  };"
      f.puts "  return funcs[index];"
      f.puts "}"

      f.puts "OBJECT cpu_populate_prim_names(STATE) {"
      f.puts "OBJECT tbl = lookuptable_new(state);"
      primitives.each_with_index do |prim_name,i|
        f.puts "lookuptable_store(state, tbl, I2N(#{i+1}), SYM(\"#{prim_name}\"));"
        f.puts "lookuptable_store(state, tbl, SYM(\"#{prim_name}\"), I2N(#{i+1}));"
      end
      f.puts "return tbl;}"
    end

    fd.puts "switch(#{op}) {"
    fd.puts "   // NOOP is 0 and signifies a method with no primitive"
    primitives.each_with_index do |prim_name,i|
      fd.puts "case #{i+1}: { // #{prim_name}"
      fd.puts "  cpu_patch_primitive(state, msg, cpu_primitive_#{prim_name}, #{i+1});"
      fd.puts "  _ret = cpu_primitive_#{prim_name}(state, c, msg);"
      fd.puts "  break;\n}"
    end

    fd.puts "default:"
    fd.puts '  printf("Error: Primitive index out of range for this VM (%d)\n", prim);'
    fd.puts "  sassert(0);"
    fd.puts "}"
    fd.puts

    File.open("primitive_indexes.h", "w") do |f|
      primitives.each_with_index do |prim_name,i|
        f.puts "#define CPU_PRIMITIVE_#{prim_name.to_s.upcase} #{i+1}"
      end
    end

    File.open("primitive_util.h", "w") do |f|
      size = primitives.size
      f.puts "struct prim2index { const char *name; int index; };"
      f.puts
      f.puts "static int calc_primitive_index(STATE, OBJECT str) {"
      f.puts "  static struct prim2index pi[] = {"

      primitives.each_with_index do |prim_name,i|
        f.puts %Q!    { "#{prim_name}", #{i+1} },!
      end

      f.puts "    { NULL, 0 } };"
      f.puts <<-CODE.gsub(/^\s{6}/,'')
        int i;
        char *target = rbx_string_as_cstr(state, str);
        for(i = 0; pi[i].name; i++) {
          if(!strcmp(target, pi[i].name)) return pi[i].index;
        }

        printf("Unknown primitive %s\\n", target);

        return -1;
      }
      CODE

    end

  end

  defprim :add
  def add
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      RET(fixnum_add(state, msg->recv, t1));
    } else if(BIGNUM_P(t1)) {
      RET(bignum_add(state, t1, msg->recv));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) + FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_add
  def bignum_add
    <<-CODE
    ARITY(1);
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

  defprim :sub
  def sub
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      RET(fixnum_sub(state, msg->recv, t1));
    } else if(BIGNUM_P(t1)) {
      OBJECT res = bignum_sub(state, t1, msg->recv);
      if(FIXNUM_P(res)) {
        res = I2N(-N2I(res));
      } else {
        res = bignum_neg(state, res);
      }
      RET(res);
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) - FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_sub
  def bignum_sub
    <<-CODE
    ARITY(1);
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

  defprim :fixnum_mul
  def fixnum_mul
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      RET(fixnum_mul(state, msg->recv, t1));
    } else if(BIGNUM_P(t1)) {
      RET(bignum_mul(state, t1, msg->recv));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) * FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :fixnum_size
  def fixnum_size
    <<-CODE
    ARITY(0);
    RET(I2N(sizeof(native_int)));
    CODE
  end

  defprim :bignum_mul
  def bignum_mul
    <<-CODE
    ARITY(1);
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

  defprim :fixnum_div
  def fixnum_div
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(FIXNUM_P(t1)) {
      native_int mod;
      GUARD(N2I(t1) != 0) // no divide by zero
      RET(I2N(fixnum_div(state, msg->recv, t1, &mod)));
    } else if(BIGNUM_P(t1)) {
      GUARD(!bignum_is_zero(state, t1));
      RET(bignum_div(state, bignum_new(state, N2I(msg->recv)), t1, NULL));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_new(state, FLOAT_TO_DOUBLE(t2) / FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_div
  def bignum_div
    <<-CODE
    ARITY(1);
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1)) {
      GUARD(!bignum_is_zero(state, t1));
      RET(bignum_div(state, msg->recv, t1, NULL));
    } else if(FIXNUM_P(t1)) {
      GUARD(N2I(t1) != 0) // no divide by zero
      RET(bignum_div(state, msg->recv, t1, NULL));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(float_new(state, a / FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_mod
  def bignum_mod
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(BIGNUM_P(msg->recv));
    POP(t1, INTEGER);

    RET(bignum_mod(state, msg->recv, t1));
    CODE
  end

  defprim :equal
  def equal
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();

    if(FIXNUM_P(t1)) {
      RET(msg->recv == t1 ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t1)) {
      RET(bignum_equal(state, t1, msg->recv));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) == FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  defprim :object_equal
  def object_equal
    <<-CODE
    ARITY(1);
    OBJECT t1;
    t1 = stack_pop();

    if(msg->recv == t1) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end

  defprim :bignum_equal
  def bignum_equal
    <<-CODE
    ARITY(1);
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_equal(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double a = bignum_to_double(state, msg->recv);
      RET(a == FLOAT_TO_DOUBLE(t1) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  defprim :compare
  def compare
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();

    if(FIXNUM_P(t1)) {
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
          FAIL();
        }
      }
    } else if(BIGNUM_P(t1)) {
      OBJECT t2 = bignum_new(state, N2I(msg->recv));
      RET(bignum_compare(state, t2, t1));
    } else if(FLOAT_P(t1)) {
      OBJECT t2 = float_coerce(state, msg->recv);
      RET(float_compare(state, FLOAT_TO_DOUBLE(t2), FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :fixnum_lt
  def fixnum_lt
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();

    if(FIXNUM_P(t2)) {
      native_int j = N2I(msg->recv);
      native_int k = N2I(t2);
      RET(j < k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      RET(bignum_ge(state, t2, msg->recv));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) < FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  defprim :fixnum_le
  def fixnum_le
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();

    if(FIXNUM_P(t2)) {
      native_int j = N2I(msg->recv);
      native_int k = N2I(t2);
      RET(j <= k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      RET(bignum_gt(state, t2, msg->recv));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) <= FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  defprim :fixnum_gt
  def fixnum_gt
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();

    if(FIXNUM_P(t2)) {
      native_int j = N2I(msg->recv);
      native_int k = N2I(t2);
      RET(j > k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      RET(bignum_le(state, t2, msg->recv));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) > FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end

  defprim :fixnum_ge
  def fixnum_ge
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t2 = stack_pop();

    if(FIXNUM_P(t2)) {
      native_int j = N2I(msg->recv);
      native_int k = N2I(t2);
      RET(j >= k ? Qtrue : Qfalse);
    } else if(BIGNUM_P(t2)) {
      RET(bignum_lt(state, t2, msg->recv));
    } else if(FLOAT_P(t2)) {
      OBJECT t1 = float_coerce(state, msg->recv);
      RET(FLOAT_TO_DOUBLE(t1) >= FLOAT_TO_DOUBLE(t2) ? Qtrue : Qfalse);
    } else {
      FAIL();
    }
    CODE
  end


  defprim :at
  def at
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(INDEXED(msg->recv));
    POP(t1, FIXNUM);

    native_int j = N2I(t1);

    GUARD(j >= 0 && j < NUM_FIELDS(msg->recv));

    RET(NTH_FIELD(msg->recv, j));
    CODE
  end

  defprim :put
  def put
    <<-CODE
    ARITY(2);
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

  defprim :fields
  def fields
    <<-CODE
    ARITY(0);
    GUARD(REFERENCE_P(msg->recv));

    RET(I2N(NUM_FIELDS(msg->recv)));
    CODE
  end

  defprim :allocate
  def allocate
    <<-CODE
    ARITY(0);
    OBJECT t1, t2;

    GUARD(CLASS_P(msg->recv));

    t1 = class_get_instance_fields(msg->recv);

    t2 = NEW_OBJECT(msg->recv, N2I(t1));
    RET(t2);
    CODE
  end

  defprim :allocate_count
  def allocate_count
    <<-CODE
    ARITY(1);
    OBJECT t1, t2;

    GUARD(CLASS_P(msg->recv));
    POP(t1, FIXNUM);

    t2 = NEW_OBJECT(msg->recv, N2I(t1));
    RET(t2);
    CODE
  end

  defprim :allocate_bytes
  def allocate_bytes
    <<-CODE
    ARITY(1);
    OBJECT t1, t2;

    GUARD(CLASS_P(msg->recv));

    POP(t1, FIXNUM);

    t2 = bytearray_new(state, N2I(t1));
    t2->klass = msg->recv;

    RET(t2);
    CODE
  end

  defprim :io_seek
  def io_seek
    <<-CODE
      ARITY(2);
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

  defprim :block_given
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

  defprim :block_call
  def block_call
    <<-CODE
    GUARD(BLOCKENV_P(msg->recv));

    blokenv_call(state, c, msg->recv, msg->args);
    CODE
  end

  defprim :io_write
  def io_write
    <<-CODE
    ARITY(1);
    OBJECT t1, t2;

    GUARD(IO_P(msg->recv));
    POP(t1, STRING);

    native_int j = io_to_fd(msg->recv);
    char *buf = rbx_string_as_cstr(state, t1);
    native_int k = N2I(string_get_bytes(t1));

    k = write(j, buf, k);
    if (k == -1) {
      RAISE_FROM_ERRNO("Unable to write");
    }

    t2 = I2N(k);
    if(k != N2I(t2)) {
      t2 = bignum_new(state, k);
    }

    RET(t2);
    CODE
  end

  defprim :io_read
  def io_read
    <<-CODE
    ARITY(1);
    OBJECT t1, t2, t3;
    native_int j, k;

    GUARD(IO_P(msg->recv));

    POP(t1, FIXNUM);

    t2 = string_new2(state, NULL, N2I(t1));
    j = io_to_fd(msg->recv);
    k = read(j, rbx_string_as_cstr(state, t2), N2I(t1));
    if(k == 0) {
      t2 = Qnil;
    } else if(k != N2I(t1)) {
      t3 = string_new2(state, NULL, k);
      memcpy(rbx_string_as_cstr(state, t3), rbx_string_as_cstr(state, t2), k);
      t2 = t3;
    }

    RET(t2);
    CODE
  end

  defprim :create_pipe
  def create_pipe
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;
    int j;
    int fds[2];

    POP(t1, IO);
    POP(t2, IO);

    j = pipe(fds);
    if(!j) {
      io_wrap(state, t1, fds[0], "r");
      io_wrap(state, t2, fds[1], "w");
    }

    RET(I2N(j));
    CODE
  end

  defprim :io_open
  def io_open
    <<-CODE
    ARITY(3);
    int fd, mode, perm;
    char *_path;
    OBJECT t1, t2, t3;

    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);

    _path = rbx_string_as_cstr(state, t1);
    mode = N2I(t2);
    perm = N2I(t3);

    fd = open(_path, mode, perm);

    RET(I2N(fd));
    CODE
  end

  defprim :io_reopen
  def io_reopen
    <<-CODE
    ARITY(1);
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

  defprim :io_operation
  def io_operation
    <<-CODE
    ARITY(1);
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

  defprim :file_unlink
  def file_unlink
    <<-CODE
    ARITY(1);
    OBJECT t1;
    char *name;

    POP(t1, STRING);

    name = rbx_string_as_cstr(state, t1);
    if(unlink(name) == 0) {
      RET(Qtrue);
    } else {
      /* TODO translate errno into an exception. */
      RET(Qfalse);
    }
    CODE
  end

  defprim :gettimeofday
  def gettimeofday
    <<-CODE
    ARITY(0);
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

  defprim :strftime
  def strftime
    <<-CODE
    ARITY(2);
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
    tm.tm_zone = rbx_string_as_cstr(state, array_get(state, t1, 10));
#endif

    format = rbx_string_as_cstr(state, t2);

    out = strftime(str, MAX_STRFTIME_OUTPUT-1, format, &tm);

    str[MAX_STRFTIME_OUTPUT] = '\\0';
    t3 = string_new2(state, str, out);
    RET(t3);
    CODE
  end

  defprim :time_switch
  def time_switch
    <<-CODE
    ARITY(2);
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

  defprim :mktime
  def mktime
    <<-CODE
    ARITY(9);
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

  defprim :fixnum_to_s
  def fixnum_to_s
    <<-CODE
    ARITY(1);
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

  defprim :bignum_to_s
  def bignum_to_s
    <<-CODE
    ARITY(1);
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));
    POP(t1, FIXNUM);

    RET(bignum_to_s(state, msg->recv, t1));
    CODE
  end

  defprim :logical_class
  def logical_class
    <<-CODE
    ARITY(0);
    /* msg->recv is ANY object because object_class knows all. */
    RET(object_class(state, msg->recv));
    CODE
  end

  defprim :object_id
  def object_id
    <<-CODE
    ARITY(0);
    RET(UI2N(object_get_id(state, msg->recv)));
    CODE
  end

  defprim :hash_set
  def hash_set
    <<-CODE
    ARITY(3);
    OBJECT t1, t2, t3;
    GUARD(HASH_P(msg->recv));

    POP(t1, FIXNUM);
    t2 = stack_pop(); // some type of object
    t3 = stack_pop(); // some type of object can we do an object guard?

    hash_add(state, msg->recv, N2I(t1), t2, t3);
    RET(t3);
    CODE
  end

  defprim :hash_get
  def hash_get
    <<-CODE
    ARITY(2);
    OBJECT t1, t2, t3;
    GUARD(HASH_P(msg->recv));

    POP(t1, FIXNUM);
    t2 = stack_pop();
    t3 = hash_find_entry(state, msg->recv, N2I(t1));
    RET(t3);
    CODE
  end

  defprim :hash_redistribute
  def hash_redistribute
    <<-CODE
    ARITY(0);
    GUARD(HASH_P(msg->recv));

    if(hash_redistribute_p(msg->recv)) {
      hash_redistribute(state, msg->recv);
    }
    RET(msg->recv);
    CODE
  end

  defprim :hash_object
  def hash_object
    <<-CODE
    ARITY(0);
    OBJECT t1;
    /* msg->recv is ANY object because object_class knows all. */
    t1 = UI2N(object_hash_int(state, msg->recv));
    RET(t1);
    CODE
  end

  defprim :hash_delete
  def hash_delete
    <<-CODE
    ARITY(1);
    OBJECT t1, t2;
    GUARD(HASH_P(msg->recv));

    POP(t1, FIXNUM);

    t2 = hash_delete(state, msg->recv, N2I(t1));
    RET(t2);
    CODE
  end

  defprim :hash_value_set
  def hash_value_set
    <<-CODE
    OBJECT t1;
    GUARD(REFERENCE_P(msg->recv));

    POP(t1, NUMERIC);
    abort();
    CODE
  end

  defprim :symbol_index
  def symbol_index
    <<-CODE
    ARITY(0);
    GUARD(SYMBOL_P(msg->recv));

    RET(I2N(symbol_to_index(state, msg->recv)));
    CODE
  end

  defprim :symbol_lookup
  def symbol_lookup
    <<-CODE
    ARITY(0);
    GUARD(STRING_P(msg->recv));
    GUARD(N2I(string_get_bytes(msg->recv)) > 0);
    RET(string_to_sym(state, msg->recv));
    CODE
  end

  defprim :dup_into
  def dup_into
    <<-CODE
    ARITY(3);
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

  defprim :bytes_dup_into
  def bytes_dup_into
    <<-CODE
    ARITY(1);
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

  defprim :object_dup
  def object_dup
    <<-CODE
    ARITY(0);
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

      if (object_tainted_p(state, msg->recv)) {
        object_set_tainted(state, t2);
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

  defprim :object_clone
  def object_clone
    <<-CODE
    ARITY(0);
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

    if (object_tainted_p(state, msg->recv)) {
      object_set_tainted(state, t2);
    }

    stack_push(t2);
    object_copy_ivars(state, msg->recv, t2);
    object_copy_metaclass(state, msg->recv, t2);
    cpu_perform_hook(state, c, t2, state->global->sym_init_copy, msg->recv);
    DONE();
    CODE
  end


  defprim :fastctx_dup
  def fastctx_dup
    <<-CODE
    ARITY(0);
    GUARD(RISA(msg->recv, fastctx));

    RET(methctx_dup(state, msg->recv));
    CODE
  end

  defprim :tuple_shifted
  def tuple_shifted
    <<-CODE
    ARITY(1);
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

  defprim :get_byte
  def get_byte
    <<-CODE
    ARITY(1);
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

  defprim :set_byte
  def set_byte
    <<-CODE
    ARITY(2);
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

  defprim :fetch_bytes
  def fetch_bytes
    <<-CODE
    ARITY(2);
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

  defprim :move_bytes
  def move_bytes
    <<-CODE
    ARITY(3);
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

  defprim :compare_bytes
  def compare_bytes
    <<-CODE
    ARITY(3);
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

  defprim :bytearray_size
  def bytearray_size
    <<-CODE
    ARITY(0);
    native_int j;
    GUARD( object_stores_bytes_p(state, msg->recv) )

    j = bytearray_bytes(state, msg->recv);

    RET(I2N(j));
    CODE
  end

  defprim :load_file
  def load_file
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;

    POP(t1, STRING);
    POP(t2, FIXNUM);

    char *path = rbx_string_as_cstr(state, t1);
    t2 = cpu_unmarshal_file(state, path, N2I(t2));

    RET(t2);
    CODE
  end

  defprim :activate_as_script
  def activate_as_script
    <<-CODE
    ARITY(0);
    GUARD(CMETHOD_P(msg->recv));

    cpu_run_script(state, c, msg->recv);
    DONE();
    CODE
  end

  defprim :process_exit
  def process_exit
    <<-CODE
    ARITY(1);
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

  defprim :micro_sleep
  def micro_sleep
    <<-CODE
    ARITY(1);
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

  defprim :activate_context
  def activate_context
    <<-CODE
    OBJECT t1;
    sassert(0);
    if(block_context_p(state, msg->recv)) {
      t1 = blokctx_home(state, msg->recv);
    } else {
      t1 = msg->recv;
    }

    cpu_activate_context(state, c, msg->recv, t1, 0);
    CODE
  end

  defprim :context_sender
  def context_sender
    <<-CODE
    ARITY(0);
    OBJECT t1;
    GUARD(CTX_P(msg->recv));

    t1 = FASTCTX(msg->recv)->sender;

    if(t1 != Qnil) {
      methctx_reference(state, t1);
    }

    RET(t1);
    CODE
  end

  defprim :string_to_sexp
  def string_to_sexp
    <<-CODE
    ARITY(3);
    bstring contents;
    const char *name;
    OBJECT t1, t2, t3;

    GUARD(STRING_P(msg->recv));

    POP(t1, STRING);
    POP(t2, FIXNUM);
    t3 = stack_pop();

    contents = cstr2bstr(rbx_string_as_cstr(state, msg->recv));
    name = rbx_string_as_cstr(state, t1);
    t1 = syd_compile_string(state, name, contents, N2I(t2), RTEST(t3));
    bdestroy(contents);

    RET(t1);
    CODE
  end

  defprim :file_to_sexp
  def file_to_sexp
    <<-CODE
    ARITY(2);
    FILE *file;
    char *name;
    OBJECT t1, t2;

    POP(t1, STRING); /* The filename */
    t2 = stack_pop();

    name = rbx_string_as_cstr(state, t1);
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

  defprim :terminal_raw
  def terminal_raw
    <<-CODE
    ARITY(0);
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

  defprim :terminal_normal
  def terminal_normal
    <<-CODE
    ARITY(0);
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

  defprim :regexp_new
  def regexp_new
    <<-CODE
    ARITY(2);
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

  defprim :regexp_match
  def regexp_match
    <<-CODE
    ARITY(1);
    OBJECT t1;
    GUARD(REGEXP_P(msg->recv));

    POP(t1, STRING);

    RET(regexp_match(state, msg->recv, t1));
    CODE
  end

  defprim :regexp_match_start
  def regexp_match_start
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;
    GUARD(REGEXP_P(msg->recv));

    POP(t1, STRING);
    POP(t2, FIXNUM);

    RET(regexp_match_start(state, msg->recv, t1, t2));
    CODE
  end

  defprim :regexp_search_region
  def regexp_search_region
    <<-CODE
    ARITY(4);
    OBJECT t1, t2, t3, t4, t5;
    GUARD(REGEXP_P(msg->recv));

    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    t4 = stack_pop();

    t5 = regexp_search_region(state, msg->recv, t1, t2, t3, t4);

    RET(t5);
    CODE
  end

  defprim :regexp_options
  def regexp_options
    <<-CODE
    ARITY(0);
    GUARD(REGEXP_P(msg->recv));

    RET(regexp_options(state, msg->recv));
    CODE
  end

  defprim :gc_start
  def gc_start
    <<-CODE
    ARITY(1);
    OBJECT t1 = stack_pop();
    if(RTEST(t1)) {
      state->om->collect_now = OMCollectYoung;
    } else {
      state->om->collect_now = OMCollectYoung | OMCollectMature;
    }
    RET(Qtrue);
    CODE
  end

  defprim :get_ivar
  def get_ivar
    <<-CODE
    ARITY(0);
    OBJECT t1 = NTH_FIELD(msg->method, 4);
    RET(object_get_ivar(state, msg->recv, t1));
    CODE
  end

  defprim :set_ivar
  def set_ivar
    <<-CODE
    ARITY(1);
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
  defprim :dispatch_as_method
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

  defprim :set_index
  def set_index
    <<-CODE
    ARITY(1);
    native_int j;
    GUARD(INDEXED(msg->recv));

    j = N2I(NTH_FIELD(msg->method, 4));
    SET_FIELD(msg->recv, j, stack_pop());

    RET(NTH_FIELD(msg->recv, j));
    CODE
  end

  defprim :get_index
  def get_index
    <<-CODE
    ARITY(0);
    native_int j;
    GUARD(INDEXED(msg->recv));

    j = N2I(NTH_FIELD(msg->method, 4));
    RET(NTH_FIELD(msg->recv, j));
    CODE
  end

  defprim :fixnum_modulo
  def fixnum_modulo
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    GUARD(N2I(t1) != 0) // no divide by zero
    native_int mod;
    fixnum_div(state, msg->recv, t1, &mod);
    RET(I2N(mod));
    CODE
  end

  defprim :marshal_object
  def marshal_object
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;
    t1 = stack_pop();
    POP(t2, FIXNUM);

    RET(cpu_marshal(state, t1, N2I(t2)));
    CODE
  end

  defprim :unmarshal_object
  def unmarshal_object
    <<-CODE
    ARITY(2);
    OBJECT t1, t2, t3;
    POP(t1, STRING);
    POP(t2, FIXNUM);
    t3 = cpu_unmarshal(state,
                       (uint8_t*)rbx_string_as_cstr(state, t1),
                       N2I(string_get_bytes(t1)),
                       N2I(t2));
    RET(t3);
    CODE
  end

  defprim :marshal_to_file
  def marshal_to_file
    <<-CODE
    ARITY(3);
    OBJECT t1, t2, t3;

    char *_path;
    t1 = stack_pop();
    POP(t2, STRING);
    POP(t3, FIXNUM);

    _path = rbx_string_as_cstr(state, t2);
    RET(cpu_marshal_to_file(state, t1, _path, N2I(t3)));
    CODE
  end

  defprim :unmarshal_from_file
  def unmarshal_from_file
    <<-CODE
    ARITY(2);
    char *_path;
    OBJECT t1, t2;

    POP(t1, STRING);
    POP(t2, FIXNUM);

    _path = rbx_string_as_cstr(state, t1);
    RET(cpu_unmarshal_file(state, _path, N2I(t2)));
    CODE
  end

  defprim :fixnum_and
  def fixnum_and
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    native_int j, k = 0;

    j = N2I(msg->recv);
    if(FIXNUM_P(t1)) {
      k = N2I(t1);
    } else if(BIGNUM_P(t1)) {
      RET(bignum_and(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      double a = FLOAT_TO_DOUBLE(t1);
      if(float_bounded_p(a)) {
        k = (native_int)float_truncate(a);
      } else {
        FAIL();
      }
    } else {
      FAIL();
    }
    RET(I2N(j & k));
    CODE
  end

  defprim :fixnum_or
  def fixnum_or
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    native_int j, k = 0;

    j = N2I(msg->recv);
    if(FIXNUM_P(t1)) {
      k = N2I(t1);
    } else if(BIGNUM_P(t1)) {
      RET(bignum_or(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      double a = FLOAT_TO_DOUBLE(t1);
      if(float_bounded_p(a)) {
        k = (native_int)float_truncate(a);
      } else {
        FAIL();
      }
    } else {
      FAIL();
    }
    RET(I2N(j | k));
    CODE
  end

  defprim :fixnum_xor
  def fixnum_xor
    <<-CODE
    ARITY(1);
    GUARD(FIXNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    native_int j, k = 0;

    j = N2I(msg->recv);
    if(FIXNUM_P(t1)) {
      k = N2I(t1);
    } else if(BIGNUM_P(t1)) {
      RET(bignum_xor(state, bignum_new(state, N2I(msg->recv)), t1));
    } else if(FLOAT_P(t1)) {
      double a = FLOAT_TO_DOUBLE(t1);
      if(float_bounded_p(a)) {
        k = (native_int)float_truncate(a);
      } else {
        FAIL();
      }
    } else {
      FAIL();
    }
    RET(I2N(j ^ k));
    CODE
  end

  defprim :fixnum_invert
  def fixnum_invert
    <<-CODE
    ARITY(0);
    native_int j;
    GUARD(FIXNUM_P(msg->recv));

    j = N2I(msg->recv);
    RET(I2N(~j));
    CODE
  end

  defprim :fixnum_neg
  def fixnum_neg
    <<-CODE
    ARITY(0);
    native_int j;
    GUARD(FIXNUM_P(msg->recv));

    j = N2I(msg->recv);
    RET(I2N(-j));
    CODE
  end

  defprim :fixnum_right_shift
  def fixnum_right_shift
    <<-CODE
    ARITY(1);
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

  defprim :fixnum_left_shift
  def fixnum_left_shift
    <<-CODE
    ARITY(1);
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

  defprim :bignum_new
  def bignum_new
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, FIXNUM);
    RET(bignum_new(state, N2I(t1)));
    CODE
  end

  defprim :bignum_to_float
  def bignum_to_float
    <<-CODE
    ARITY(0);
    GUARD(BIGNUM_P(msg->recv));

    RET(float_new(state, bignum_to_double(state, msg->recv)));
    CODE
  end

  defprim :bignum_and
  def bignum_and
    <<-CODE
    ARITY(1);
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(INTEGER_P(t1)) {
      RET(bignum_and(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      RET(bignum_and(state, msg->recv,
                     bignum_from_double(state, FLOAT_TO_DOUBLE(t1))));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_or
  def bignum_or
    <<-CODE
    ARITY(1);
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(INTEGER_P(t1)) {
      RET(bignum_or(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      RET(bignum_or(state, msg->recv,
                    bignum_from_double(state, FLOAT_TO_DOUBLE(t1))));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_xor
  def bignum_xor
    <<-CODE
    ARITY(1);
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();
    if(INTEGER_P(t1)) {
      RET(bignum_xor(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      RET(bignum_xor(state, msg->recv,
                     bignum_from_double(state, FLOAT_TO_DOUBLE(t1))));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :bignum_neg
  def bignum_neg
    <<-CODE
    ARITY(0);
    GUARD(BIGNUM_P(msg->recv));

    RET(bignum_neg(state, msg->recv));
    CODE
  end

  defprim :bignum_invert
  def bignum_invert
    <<-CODE
    ARITY(0);
    GUARD(BIGNUM_P(msg->recv));

    RET(bignum_invert(state, msg->recv));
    CODE
  end

  defprim :numeric_coerce
  def numeric_coerce
    <<-CODE
    ARITY(1);
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

  defprim :bignum_compare
  def bignum_compare
    <<-CODE
    ARITY(1);
    GUARD(BIGNUM_P(msg->recv));
    OBJECT t1 = stack_pop();    
    if(BIGNUM_P(t1) || FIXNUM_P(t1)) {
      RET(bignum_compare(state, msg->recv, t1));
    } else if(FLOAT_P(t1)) {
      double f = bignum_to_double(state, msg->recv);
      RET(float_compare(state, f, FLOAT_TO_DOUBLE(t1)));
    } else {
      FAIL();
    }
    CODE
  end

  defprim :fixnum_to_f
  def fixnum_to_f
    <<-CODE
    ARITY(0);
    GUARD(FIXNUM_P(msg->recv));

    RET(float_new(state, FIXNUM_TO_DOUBLE(msg->recv)));
    CODE
  end

  defprim :string_to_f
  def string_to_f
    <<-CODE
    ARITY(0);
    GUARD(STRING_P(msg->recv));

    RET(float_new(state, string_to_double(state, msg->recv)));
    CODE
  end

  defprim :fixnum_divmod
  def fixnum_divmod
    <<-CODE
    ARITY(1);
    OBJECT t1;
    GUARD(FIXNUM_P(msg->recv));
    POP(t1, FIXNUM);

    GUARD( N2I(t1) != 0 ) // no divide by zero
    RET(fixnum_divmod(state, msg->recv, t1));
    CODE
  end

  defprim :bignum_left_shift
  def bignum_left_shift
    <<-CODE
    ARITY(1);
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));

    POP(t1, FIXNUM);

    RET(bignum_left_shift(state, msg->recv, t1));
    CODE
  end

  defprim :bignum_right_shift
  def bignum_right_shift
    <<-CODE
    ARITY(1);
    OBJECT t1;
    GUARD(BIGNUM_P(msg->recv));

    POP(t1, FIXNUM);

    RET(bignum_right_shift(state, msg->recv, t1));
    CODE
  end

  defprim :find_method
  def find_method
    <<-CODE
    ARITY(1);
    OBJECT t1, t2, t3;
    POP(t1, SYMBOL);
    t1 = exported_cpu_find_method(state, c, msg->recv, t1, &t2);
    t3 = tuple_new(state, 2);

    tuple_put(state, t3, 0, t2);
    tuple_put(state, t3, 1, t1);

    RET(t3);
    CODE
  end

  defprim :bignum_divmod
  def bignum_divmod
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(BIGNUM_P(msg->recv));
    t1 = stack_pop();

    // no divide by zero
    if(FIXNUM_P(t1)) {
      GUARD(N2I(t1) != 0);
    } else if(BIGNUM_P(t1)) {
      GUARD(!bignum_is_zero(state, t1));
    } else {
      FAIL();
    }

    RET(bignum_divmod(state, msg->recv, t1));
    CODE
  end

  defprim :object_taint
  def object_taint
    <<-CODE
    ARITY(0);
    GUARD(REFERENCE_P(msg->recv));
    object_set_tainted(state, msg->recv);
    RET(msg->recv);
    CODE
  end

  defprim :object_tainted_p
  def object_tainted_p
    <<-CODE
    ARITY(0);
    GUARD(REFERENCE_P(msg->recv));
    RET(object_tainted_p(state, msg->recv) ? Qtrue : Qfalse);
    CODE
  end

  defprim :object_untaint
  def object_untaint
    <<-CODE
    ARITY(0);
    GUARD(REFERENCE_P(msg->recv));
    object_set_untainted(state, msg->recv);
    RET(msg->recv);
    CODE
  end

  defprim :object_freeze
  def object_freeze
    <<-CODE
    ARITY(0);
    GUARD(REFERENCE_P(msg->recv));
    object_set_frozen(state, msg->recv);
    RET(msg->recv);
    CODE
  end

  defprim :object_frozen_p
  def object_frozen_p
    <<-CODE
    ARITY(0);
    GUARD(REFERENCE_P(msg->recv));
    RET(object_frozen_p(state, msg->recv) ? Qtrue : Qfalse);
    CODE
  end

  defprim :fastctx_get_field
  def fastctx_get_field
    <<-CODE
    ARITY(1);
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

  defprim :fastctx_set_field
  def fastctx_set_field
    <<-CODE
    ARITY(2);
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

  defprim :fastctx_reload_method
  def fastctx_reload_method
    <<-CODE
    ARITY(0);
    struct fast_context *fc;
    GUARD(CTX_P(msg->recv));

    fc = FASTCTX(msg->recv);
    if(fc->method->obj_type == CMethodType) {
      fc->custom_iseq = Qnil;
      fc->data = BYTEARRAY_ADDRESS(cmethod_get_compiled(fc->method));
    }

    RET(Qtrue);
    CODE
  end

  defprim :fastctx_set_iseq
  def fastctx_set_iseq
    <<-CODE
    ARITY(1);
    struct fast_context *fc;
    OBJECT t1, ba;
    int target_size;

    GUARD(CTX_P(msg->recv));
    POP(t1, BYTEARRAY);
    fc = FASTCTX(msg->recv);
    GUARD(fc->method->obj_type == CMethodType);

#if DIRECT_THREADED
    target_size = (BYTEARRAY_SIZE(t1) / sizeof(uint32_t)) * sizeof(uintptr_t);
#else
    target_size = BYTEARRAY_SIZE(t1);
#endif

    ba = bytearray_new(state, target_size);
    cpu_compile_instructions(state, t1, ba);
    fc->custom_iseq = ba;
    fc->data = BYTEARRAY_ADDRESS(ba);

    RET(Qtrue);
    CODE
  end

  defprim :vm_stats
  def vm_stats
    <<-CODE
    ARITY(0);
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

  defprim :nmethod_call
  def nmethod_call
    <<-CODE
    OBJECT t1;
    cpu_flush_ip(c);
    cpu_flush_sp(c);
    cpu_save_registers(state, c, msg->args);
    t1 = nmc_new(state, msg->method, c->active_context, msg->recv, msg->name, msg->block, msg->args);

    ARITY(nmethod_get_args(msg->method));

    nmc_activate(state, c, t1, Qnil, FALSE);
    CODE
  end

  defprim :nfunc_call
  def nfunc_call
    <<-CODE
    /* The definition of beauty. Simplicity. To call a native function, there is no work
       to be done. The stub contains all the serialization code.

       That being said, this might get more complicated when callbacks are supported. */
    cpu_patch_ffi(state, msg);
    ffi_call(state, c, nfunc_get_data(msg->method));
    CODE
  end

  defprim :nfunc_call_object
  def nfunc_call_object
    <<-CODE
    /* The definition of beauty. Simplicity. To call a native function, there is no work
       to be done. The stub contains all the serialization code.

       That being said, this might get more complicated when callbacks are supported. */
    ffi_call(state, c, nfunc_get_data(msg->recv));
    CODE
  end

  defprim :nfunc_add
  def nfunc_add
    <<-CODE
    ARITY(4);
    OBJECT t1, t2, t3, t4;

    POP(t1, STRING_OR_NIL);
    POP(t2, STRING);
    POP(t3, ARRAY);
    POP(t4, FIXNUM);
    RET(ffi_function_create(state, t1, t2, t3, t4));
    CODE
  end

  defprim :load_library
  def load_library
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;

    POP(t1, STRING);
    POP(t2, STRING);
    RET(subtend_load_library(state, c, t1, t2));
    CODE
  end

  defprim :dir_glob
  def dir_glob
    <<-CODE
    ARITY(2);
    glob_t gd;
    char *pat;
    int flags = GLOB_NOSORT | GLOB_BRACE;
    OBJECT t1, t2;
    native_int j, k;

    POP(t1, STRING);
    POP(t2, FIXNUM);

    /* TODO: use t2. */

    pat = rbx_string_as_cstr(state, t1);
    k = glob(pat, flags, NULL, &gd);
    t2 = array_new(state, gd.gl_pathc);
    for(j = 0; j < gd.gl_pathc; j++) {
      array_set(state, t2, j, string_new(state, gd.gl_pathv[j]));
    }
    globfree(&gd);
    RET(t2);
    CODE
  end

  defprim :dir_chdir
  def dir_chdir
    <<-CODE
    ARITY(1);
    char *path;
    OBJECT t1;

    POP(t1, STRING);

    path = rbx_string_as_cstr(state, t1);
    if(!chdir(path)) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end

  defprim :yield_gdb
  def yield_gdb
    <<-CODE
    ARITY(1);
    OBJECT t1 = stack_pop();
    *((OBJECT*)4) = t1; /* cause a SIGBUS */
    RET(Qtrue);
    CODE
  end

  defprim :make_weak_ref
  def make_weak_ref
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, REFERENCE);

    RET(object_make_weak_ref(state, t1));
    CODE
  end

  defprim :gc_collect_references
  def gc_collect_references
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, REFERENCE);

    RET(object_memory_collect_references(state, state->om, t1));
    CODE
  end

  defprim :task_dup
  def task_dup
    <<-CODE
    ARITY(0);
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

  defprim :task_set_current
  def task_set_current
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, TASK);

    stack_push(Qnil);
    cpu_task_select(state, c, t1);
    CODE
  end

  defprim :task_associate
  def task_associate
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(TASK_P(msg->recv));
    POP(t1, BLOCKENV);

    RET(cpu_task_associate(state, c, msg->recv, t1));
    CODE
  end

  defprim :task_current
  def task_current
    <<-CODE
    ARITY(0);
    RET(c->current_task);
    CODE
  end

  defprim :task_at
  def task_at
    <<-CODE
    ARITY(1);
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

  defprim :task_set_debugging
  def task_set_debugging
    <<-CODE
    ARITY(2);
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

  defprim :task_debug_channel
  def task_debug_channel
    <<-CODE
    ARITY(0);
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));

    task = (struct cpu_task*)BYTES_OF(msg->recv);

    RET(task->debug_channel);
    CODE
  end

  defprim :task_control_channel
  def task_control_channel
    <<-CODE
    ARITY(0);
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));

    task = (struct cpu_task*)BYTES_OF(msg->recv);

    RET(task->control_channel);
    CODE
  end

  defprim :task_get_debug_context_change
  def task_get_debug_context_change
    <<-CODE
    ARITY(0);
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

  defprim :task_set_debug_context_change
  def task_set_debug_context_change
    <<-CODE
    ARITY(1);
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

  defprim :task_stack_size
  def task_stack_size
    <<-CODE
    ARITY(0);
    struct cpu_task *task;
    GUARD(TASK_P(msg->recv));

    task = (struct cpu_task*)BYTES_OF(msg->recv);
    OBJECT t1 = I2N(task->sp_ptr - task->stack_top);

    RET(t1);
    CODE
  end

  defprim :task_get_stack_value
  def task_get_stack_value
    <<-CODE
    ARITY(1);
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

  defprim :task_raise
  def task_raise
    <<-CODE
    ARITY(1);
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

  defprim :thread_raise
  def thread_raise
    <<-CODE
    ARITY(1);
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

  defprim :channel_new
  def channel_new
    <<-CODE
    ARITY(0);
    RET(cpu_channel_new(state));
    CODE
  end

  defprim :channel_send
  def channel_send
    <<-CODE
    ARITY(1);
    OBJECT t1;
    GUARD(CHANNEL_P(msg->recv));

    t1 = stack_pop();
    RET(cpu_channel_send(state, c, msg->recv, t1));
    CODE
  end

  defprim :channel_receive
  def channel_receive
    <<-CODE
    ARITY(0);
    GUARD(CHANNEL_P(msg->recv));

    cpu_channel_receive(state, c, msg->recv, c->current_thread);
    /* Don't touch the stack as we may be in a different task at this
       point. The original task's stack is updated when the channel
       is written to and the task restored. */
    CODE
  end

  defprim :channel_send_in_microseconds
  def channel_send_in_microseconds
    <<-CODE
    ARITY(3);
    double seconds;
    OBJECT channel, usec_r, tag;
    native_int k;

    POP(channel, CHANNEL);
    POP(usec_r, INTEGER);
    tag = stack_pop();

    if(FIXNUM_P(usec_r)) {
      k = (long)N2I(usec_r);
    } else {
      k = (long)bignum_to_int(state, usec_r);
    }

    seconds = k / 1000000.0;

    RET(cpu_event_wake_channel(state, c, channel, seconds, tag));
    CODE
  end

  defprim :channel_send_in_seconds
  def channel_send_in_seconds
    <<-CODE
    ARITY(3);
    double seconds;
    OBJECT channel, seconds_r, tag;

    POP(channel, CHANNEL);
    POP(seconds_r, FLOAT);
    tag = stack_pop();

    seconds = FLOAT_TO_DOUBLE(seconds_r);

    RET(cpu_event_wake_channel(state, c, channel, seconds, tag));
    CODE
  end

  defprim :channel_send_on_readable
  def channel_send_on_readable
    <<-CODE
    ARITY(4);
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

  defprim :channel_send_on_writable
  def channel_send_on_writable
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;
    native_int j;

    POP(t2, CHANNEL);
    POP(t1, IO);

    j = io_to_fd(t1);
    RET(cpu_event_wait_writable(state, c, t2, j));
    CODE
  end

  defprim :channel_send_on_signal
  def channel_send_on_signal
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;

    POP(t2, CHANNEL);
    POP(t1, FIXNUM);

    RET(cpu_event_wait_signal(state, c, t2, N2I(t1)));
    CODE
  end

  defprim :channel_send_on_stopped
  def channel_send_on_stopped
    <<-CODE
    ARITY(3);
    OBJECT t1, t2, t3;

    POP(t3, CHANNEL);
    POP(t1, FIXNUM);
    POP(t2, FIXNUM);

    RET(cpu_event_wait_child(state, c, t3, N2I(t1), N2I(t2)));
    CODE
  end

  defprim :scheduler_cancel
  def scheduler_cancel
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, FIXNUM);

    RET(cpu_event_cancel_event(state, t1) ? Qtrue : Qfalse);
    CODE
  end

  defprim :thread_new
  def thread_new
    <<-CODE
    ARITY(0);
    RET(cpu_thread_new(state, c));
    CODE
  end

  defprim :thread_run
  def thread_run
    <<-CODE
    ARITY(0);
    GUARD(THREAD_P(msg->recv));

    GUARD(cpu_thread_alive_p(state, msg->recv));

    /* So when we're restored, there is a ret val. */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);
    cpu_thread_force_run(state, c, msg->recv);
    CODE
  end

  defprim :thread_schedule
  def thread_schedule
    <<-CODE
    ARITY(0);
    GUARD(THREAD_P(msg->recv));

    cpu_thread_schedule(state, msg->recv);
    RET(Qnil);
    CODE
  end

  defprim :thread_yield
  def thread_yield
    <<-CODE
    ARITY(0);
    /* Same reason as thread_run */
    stack_push(Qnil);
    cpu_thread_schedule(state, c->current_thread);
    THDEBUG("%d: thread yield.\\n", getpid());
    cpu_thread_run_best(state, c);
    CODE
  end

  defprim :thread_dequeue
  def thread_dequeue
    <<-CODE
    ARITY(0);
    THDEBUG("%d: dequeue thread.\\n", getpid());
    cpu_thread_exited(state, c);
    CODE
  end

  defprim :thread_current
  def thread_current
    <<-CODE
    ARITY(0);
    RET(c->current_thread);
    CODE
  end

  defprim :object_become
  def object_become
    <<-CODE
    ARITY(2);
    void state_object_become(STATE, cpu c, OBJECT from, OBJECT to);
    OBJECT t1, t2;

    POP(t2, REFERENCE);
    POP(t1, REFERENCE);
    state_object_become(state, c, t2, t1);
    RET(t2);
    CODE
  end

  defprim :sampler_activate
  def sampler_activate
    <<-CODE
    ARITY(1);
    OBJECT t1;

    POP(t1, FIXNUM);
    cpu_sampler_activate(state, N2I(t1));
    RET(ML2N(clock()));
    CODE
  end

  defprim :sampler_stop
  def sampler_stop
    <<-CODE
    ARITY(0);
    OBJECT t1 = cpu_sampler_disable(state);
    RET(t1);
    CODE
  end

  defprim :fork_process
  def fork_process
    <<-CODE
    ARITY(0);
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


  defprim :replace_process# aka execve().
  def replace_process
    <<-CODE
    ARITY(2);
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

      tmp = rbx_string_as_cstr(state, t3);
      argv[j] = tmp ? strdup(tmp) : NULL;
    }

    argv[k] = NULL;

    tmp = rbx_string_as_cstr(state, t1);
    file = tmp ? strdup(tmp) : NULL;

    cpu_task_disable_preemption(state);
    k = execvp(file, argv);

    /* If you're here, there was an error. */
    cpu_task_configure_preemption(state);
    RAISE_FROM_ERRNO("Unable to execute");

    CODE
  end

  defprim :ivar_get
  def ivar_get
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, SYMBOL);
    RET(object_get_ivar(state, msg->recv, t1));
    CODE
  end

  defprim :ivar_set
  def ivar_set
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;
    POP(t1, SYMBOL);
    t2 = stack_pop();
    object_set_ivar(state, msg->recv, t1, t2);
    RET(t2);
    CODE
  end

  defprim :ivars_get
  def ivars_get
    <<-CODE
    ARITY(0);
    RET(object_get_ivars(state, msg->recv));
    CODE
  end

  defprim :str_crypt
  def str_crypt
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(STRING_P(msg->recv));
    POP(t1, STRING);
    RET(string_new(state, crypt(rbx_string_as_cstr(state, msg->recv),
        rbx_string_as_cstr(state, t1))));
    CODE
  end

  defprim :env_get
  def env_get
    <<-CODE
    ARITY(1);
    char *key;
    OBJECT t1, t2;

    POP(t1, STRING);

    t2 = Qnil;

    key = rbx_string_as_cstr(state, t1);
    if (key) {
      char *value = getenv(key);

      if (value) {
        t2 = string_new(state, value);
      }
    }

    RET(t2);
    CODE
  end

  defprim :env_set
  def env_set
    <<-CODE
    ARITY(2);
    char *key, *value;
    OBJECT t1, t2;

    POP(t1, STRING);
    t2 = stack_pop();

    key = rbx_string_as_cstr(state, t1);
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
        value = rbx_string_as_cstr(state, t2);
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

  defprim :env_as_hash
  def env_as_hash
    <<-CODE
    ARITY(0);
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

  defprim :bignum_size
  def bignum_size
    <<-CODE
    ARITY(0);
    GUARD(BIGNUM_P(msg->recv));
    RET(bignum_size(state, msg->recv));
    CODE
  end

  defprim :iseq_compile
  def iseq_compile
    <<-CODE
    ARITY(0);
    GUARD(CMETHOD_P(msg->recv));
    cpu_compile_method(state, msg->recv);
    RET(Qtrue);
    CODE
  end

  defprim :reset_method_cache
  def reset_method_cache
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, SYMBOL);
    cpu_clear_cache_for_method(state, c, t1, TRUE);
    CODE
  end

  defprim :bignum_from_float
  def bignum_from_float
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, FLOAT);
    RET(bignum_from_double(state, FLOAT_TO_DOUBLE(t1)));
    CODE
  end

  defprim :save_encloser_path
  def save_encloser_path
    <<-CODE
    ARITY(0);
    cpu_set_encloser_path(state, c, state->global->object);
    RET(Qnil);
    CODE
  end

  defprim :restore_encloser_path
  def restore_encloser_path
    <<-CODE
    ARITY(0);
    cpu_push_encloser(state, c);
    RET(Qnil);
    CODE
  end

  defprim :array_aref
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

  defprim :array_aset
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

  defprim :string_append
  def string_append
    <<-CODE
    OBJECT t1;

    GUARD(msg->args == 1);
    POP(t1, STRING);

    string_append(state, msg->recv, t1);
    RET(msg->recv);
    CODE
  end

  defprim :string_dup
  def string_dup
    <<-CODE
    ARITY(0);
    RET(string_dup(state, msg->recv));
    CODE
  end

  defprim :string_equal
  def string_equal
    <<-CODE
    ARITY(1);
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

  defprim :object_send
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

  defprim :machine_new
  def machine_new
    <<-CODE
    ARITY(1);
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
        argv[i] = strdup(rbx_string_as_cstr(state, str));
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

  defprim :machine_join
  def machine_join
    <<-CODE
    ARITY(1);
    OBJECT t1;
    POP(t1, FIXNUM);
    if(environment_join_machine(environment_current(), N2I(t1))) {
      RET(Qtrue);
    } else {
      RET(Qfalse);
    }
    CODE
  end

  defprim :machine_get_message
  def machine_get_message
    <<-CODE
    ARITY(0);
    RET(environment_get_message(environment_current(), current_machine->id));
    CODE
  end

  defprim :machine_send_message
  def machine_send_message
    <<-CODE
    ARITY(2);
    OBJECT t1, t2;
    POP(t1, FIXNUM);
    t2 = stack_pop();

    environment_send_message(environment_current(), N2I(t1), t2);

    RET(Qtrue);
    CODE
  end

  defprim :dir_open
  def dir_open
    <<-CODE
    ARITY(1);
    OBJECT t1;
    DIR *dir;

    POP(t1, STRING);

    dir = opendir(rbx_string_as_cstr(state, t1));
    if(!dir) RAISE_FROM_ERRNO("Unable to open directory");

    RET(ffi_new_pointer(state, dir));
    CODE
  end

  defprim :dir_close
  def dir_close
    <<-CODE
    ARITY(1);
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

  defprim :dir_control
  def dir_control
    <<-CODE
    ARITY(3);
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

  defprim :dir_read
  def dir_read
    <<-CODE
    ARITY(1);
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

  defprim :opt_push_literal
  def opt_push_literal
    <<-CODE
    ARITY(0);
    OBJECT lits;

    lits = cmethod_get_literals(msg->method);
    RET(fast_fetch(lits, 0));
    CODE
  end

  defprim :opt_push_self
  def opt_push_self
    <<-CODE
    ARITY(0);
    RET(msg->recv);
    CODE
  end

  defprim :opt_push_ivar
  def opt_push_ivar
    <<-CODE
    ARITY(0);
    OBJECT lits;

    lits = cmethod_get_literals(msg->method);
    RET(object_get_ivar(state, msg->recv, fast_fetch(lits, 0)));
    CODE
  end

  defprim :opt_push_my_field
  def opt_push_my_field
    <<-CODE
    ARITY(0);
    OBJECT lits;

    lits = cmethod_get_literals(msg->method);
    RET(NTH_FIELD(msg->recv, N2I(fast_fetch(lits, 0))));
    CODE
  end

  defprim :opt_kind_of
  def opt_kind_of
    <<-CODE
    ARITY(1);
    OBJECT t1;

    POP(t1, REFERENCE);
    GUARD(CLASS_P(t1) || MODULE_P(t1));

    RET(object_kind_of_p(state, msg->recv, t1) ? Qtrue : Qfalse);
    CODE
  end

  defprim :float_add
  def float_add
    <<-CODE
    ARITY(1);
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

  defprim :float_sub
  def float_sub
    <<-CODE
    ARITY(1);
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

  defprim :float_mul
  def float_mul
    <<-CODE
    ARITY(1);
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

  defprim :float_div
  def float_div
    <<-CODE
    ARITY(1);
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

  defprim :float_uminus
  def float_uminus
    <<-CODE
    ARITY(0);
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    RET(float_new(state, -a));
    CODE
  end

  defprim :float_equal
  def float_equal
    <<-CODE
    ARITY(1);
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

  defprim :float_eql
  def float_eql
    <<-CODE
    ARITY(1);
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

  defprim :float_compare
  def float_compare
    <<-CODE
    ARITY(1);
    GUARD(FLOAT_P(msg->recv));
    double a = FLOAT_TO_DOUBLE(msg->recv);
    OBJECT t1 = stack_pop();
    if(!FLOAT_P(t1)) {
      t1 = float_coerce(state, t1);
      GUARD(FLOAT_P(t1));
    }
    double b = FLOAT_TO_DOUBLE(t1);
    RET(float_compare(state, a, b));
    CODE
  end

  defprim :float_lt
  def float_lt
    <<-CODE
    ARITY(1);
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

  defprim :float_le
  def float_le
    <<-CODE
    ARITY(1);
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

  defprim :float_gt
  def float_gt
    <<-CODE
    ARITY(1);
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

  defprim :float_ge
  def float_ge
    <<-CODE
    ARITY(1);
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

  defprim :float_to_i
  def float_to_i
    <<-CODE
    ARITY(0);
    GUARD(FLOAT_P(msg->recv));
    double b = FLOAT_TO_DOUBLE(msg->recv);
    if(isinf(b)) {
      RAISE("FloatDomainError", b < 0 ? "-Infinity" : "Infinity");
    } else if(isnan(b)) {
      RET(msg->recv);
    } else {
      RET(bignum_from_double(state, float_truncate(b)));
    }
    CODE
  end

  defprim :float_round
  def float_round
    <<-CODE
    ARITY(0);
    GUARD(FLOAT_P(msg->recv));
    double value = FLOAT_TO_DOUBLE(msg->recv);
    if (value > 0.0) value = floor(value+0.5);
    if (value < 0.0) value = ceil(value-0.5);
    RET(bignum_from_double(state, value));
    CODE
  end

  defprim :float_divmod
  def float_divmod
    <<-CODE
    ARITY(1);
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
      array_set(state, ary, 0, bignum_from_double(state, float_truncate(div)));
      array_set(state, ary, 1, float_new(state, mod));
      RET(ary);
    }
    CODE
  end

  defprim :float_pow
  def float_pow
    <<-CODE
    ARITY(1);
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

  defprim :float_isnan
  def float_isnan
    <<-CODE
    ARITY(0);
    GUARD(FLOAT_P(msg->recv));
    RET(isnan(FLOAT_TO_DOUBLE(msg->recv)) == 1 ? Qtrue : Qfalse);
    CODE
  end

  defprim :float_isinf
  def float_isinf
    <<-CODE
    ARITY(0);
    GUARD(FLOAT_P(msg->recv));
    double value = FLOAT_TO_DOUBLE(msg->recv);
    if(isinf(value) != 0) {
      RET(value < 0 ? I2N(-1) : I2N(1));
    } else {
      RET(Qnil);
    }
    CODE
  end

  defprim :bignum_gt
  def bignum_gt
    <<-CODE
    ARITY(1);
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

  defprim :bignum_ge
  def bignum_ge
    <<-CODE
    ARITY(1);
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

  defprim :bignum_lt
  def bignum_lt
    <<-CODE
    ARITY(1);
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

  defprim :bignum_le
  def bignum_le
    <<-CODE
    ARITY(1);
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

  defprim :sendsite_create
  def sendsite_create
    <<-CODE
    ARITY(1);
    OBJECT t1, ss;

    GUARD(CLASS_P(msg->recv));
    POP(t1, SYMBOL);

    ss = send_site_create(state, t1);
    RET(ss);
    CODE
  end

  defprim :sendsite_set_sender
  def sendsite_set_sender
    <<-CODE
    ARITY(1);
    OBJECT cm;

    GUARD(SENDSITE_P(msg->recv));
    POP(cm, CMETHOD);

    send_site_set_sender(state, msg->recv, cm);
    RET(Qnil);
    CODE
  end

  defprim :sendsite_at
  def sendsite_at
    <<-CODE
    ARITY(1);
    OBJECT t1;

    GUARD(SENDSITE_P(msg->recv));
    POP(t1, FIXNUM);

    switch(N2I(t1)) {
    case 0:
      RET(SENDSITE(msg->recv)->name);
    case 1:
      RET(SENDSITE(msg->recv)->selector);
    case 2:
      RET(SENDSITE(msg->recv)->data1);
    case 3:
      RET(SENDSITE(msg->recv)->data2);
    case 4:
      RET(SENDSITE(msg->recv)->data3);
    case 5:
      RET(ffi_new_pointer(state, SENDSITE(msg->recv)->c_data));
    case 6:
      RET(I2N(SENDSITE(msg->recv)->hits));
    case 7:
      RET(I2N(SENDSITE(msg->recv)->misses));
    case 8:
      RET(SENDSITE(msg->recv)->sender);
    default:
      RET(Qnil);
    }
    CODE
  end

  defprim :selector_clear
  def selector_clear
    <<-CODE
    GUARD(SELECTOR_P(msg->recv));

    selector_clear(state, msg->recv);

    RET(Qnil);
    CODE
  end

  defprim :dtrace_fire_ruby_probe
  def dtrace_fire_ruby_probe
    <<-CODE
    ARITY(2);

    OBJECT t1, t2;

    POP(t1, STRING);
    POP(t2, STRING);

#if ENABLE_DTRACE
    if (RUBINIUS_RUBY_PROBE_ENABLED()) {
      RUBINIUS_RUBY_PROBE(rbx_string_as_cstr(state, t1), rbx_string_as_cstr(state, t2));
    }
#endif

    RET(Qnil);
    CODE
  end

  defprim :allocate_table
  def allocate_table
    <<-CODE
    ARITY(0);
    GUARD(CLASS_P(msg->recv));
    OBJECT t1;

    t1 = lookuptable_new(state);
    SET_CLASS(t1, msg->recv);
    RET(t1);
    CODE
  end

  defprim :lookuptable_store
  def lookuptable_store
    <<-CODE
    ARITY(2);
    GUARD(LOOKUPTABLE_P(msg->recv));
    OBJECT t1, t2;

    t1 = stack_pop();
    t2 = stack_pop();
    RET(lookuptable_store(state, msg->recv, t1, t2));
    CODE
  end

  defprim :lookuptable_fetch
  def lookuptable_fetch
    <<-CODE
    ARITY(1);
    GUARD(LOOKUPTABLE_P(msg->recv));
    OBJECT t1, t2;

    t1 = stack_pop();
    t2 = lookuptable_fetch(state, msg->recv, t1);
    RET(t2 == Qundef ? Qnil : t2);
    CODE
  end

  defprim :lookuptable_delete
  def lookuptable_delete
    <<-CODE
    ARITY(1);
    GUARD(LOOKUPTABLE_P(msg->recv));
    OBJECT t1, t2;

    t1 = stack_pop();
    t2 = lookuptable_delete(state, msg->recv, t1);
    RET(t2 == Qundef ? Qnil : t2);
    CODE
  end

  defprim :lookuptable_has_key
  def lookuptable_has_key
    <<-CODE
    ARITY(1);
    GUARD(LOOKUPTABLE_P(msg->recv));
    OBJECT t1;

    t1 = stack_pop();
    RET(lookuptable_has_key(state, msg->recv, t1));
    CODE
  end

  defprim :lookuptable_keys
  def lookuptable_keys
    <<-CODE
    ARITY(0);
    GUARD(LOOKUPTABLE_P(msg->recv));
    RET(lookuptable_keys(state, msg->recv));
    CODE
  end

  defprim :lookuptable_values
  def lookuptable_values
    <<-CODE
    ARITY(0);
    GUARD(LOOKUPTABLE_P(msg->recv));
    RET(lookuptable_values(state, msg->recv));
    CODE
  end

  defprim :lookuptable_entries
  def lookuptable_entries
    <<-CODE
    ARITY(0);
    GUARD(LOOKUPTABLE_P(msg->recv));
    RET(lookuptable_entries(state, msg->recv));
    CODE
  end

  defprim :lookuptable_dup
  def lookuptable_dup
    <<-CODE
    ARITY(0);
    GUARD(LOOKUPTABLE_P(msg->recv));
    RET(lookuptable_dup(state, msg->recv));
    CODE
  end

  defprim :allocate_module
  def allocate_module
    <<-CODE
    ARITY(0);
    GUARD(CLASS_P(msg->recv));
    OBJECT t1, t2;

    t1 = class_get_instance_fields(msg->recv);
    t2 = NEW_OBJECT(msg->recv, N2I(t1));
    module_setup_fields(state, t2);
    RET(t2);
    CODE
  end

  defprim :allocate_hash
  def allocate_hash
    <<-CODE
    ARITY(0);
    GUARD(CLASS_P(msg->recv));
    OBJECT t1;

    t1 = hash_new(state);
    SET_CLASS(t1, msg->recv);
    RET(t1);
    CODE
  end

  defprim :string_tr_expand
  def string_tr_expand
    <<-CODE
    ARITY(1);
    GUARD(STRING_P(msg->recv));
    OBJECT t1;
    t1 = stack_pop();
    RET(string_tr_expand(state, msg->recv, t1));
    CODE
  end

  defprim :string_template
  def string_template
    <<-CODE
    ARITY(2);
    GUARD(CLASS_P(msg->recv));
    OBJECT t1, t2, t3;
    native_int k;
    char *data;

    POP(t1, FIXNUM);
    k = N2I(t1);
    t3 = string_new2(state, NULL, k);
    SET_CLASS(t3, msg->recv);
    t3->IsTainted = msg->recv->IsTainted;
    data = rbx_string_as_cstr(state, t3);

    t2 = stack_pop();
    if(FIXNUM_P(t2)) {
      memset(data, N2I(t2), k);
    } else if(STRING_P(t2)) {
      int bytes;
      char *str;

      t3->IsTainted |= t2->IsTainted;
      bytes = N2I(string_get_bytes(t2));
      str = rbx_string_as_cstr(state, t2);
      if(bytes == 1) {
        memset(data, str[0], k);
      } else if(bytes > 1) {
        int i, j, n, size;

        size = k / bytes;
        for(n = i = 0; i < size; i++) {
          for(j = 0; j < bytes; j++, n++) {
            data[n] = str[j];
          }
        }
        for(i = n, j = 0; i < k; i++, j++) {
          data[i] = str[j];
        }
      }
    } else {
      FAIL();
    }
    RET(t3);
    CODE
  end

  defprim :string_apply_and
  def string_apply_and
    <<-CODE
    ARITY(1);
    GUARD(STRING_P(msg->recv));
    OBJECT t1;
    native_int i, j, k, size;
    char *a, *b;

    POP(t1, STRING);
    a = rbx_string_as_cstr(state, msg->recv);
    b = rbx_string_as_cstr(state, t1);
    j = N2I(string_get_bytes(msg->recv));
    k = N2I(string_get_bytes(t1));
    size = j < k ? j : k;

    for(i = 0; i < size; i++) {
      a[i] = a[i] && b[i];
    }
    RET(Qnil);
    CODE
  end

  defprim :tuple_template
  def tuple_template
    <<-CODE
    ARITY(2)
    GUARD(CLASS_P(msg->recv));
    OBJECT t1, t2, t3;
    native_int i, k;

    POP(t1, FIXNUM);
    k = N2I(t1);
    t2 = stack_pop();
    t3 = tuple_new(state, k);
    for(i = 0; i < k; i++) {
      tuple_put(state, t3, i, t2);
    }
    RET(t3);
    CODE
  end

  defprim :string_copy_from
  def string_copy_from
    <<-CODE
    ARITY(4);
    GUARD(STRING_P(msg->recv));
    OBJECT t1, t2, t3, t4;
    native_int n, start, size, dest;
    char *a, *b;

    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);
    POP(t4, FIXNUM);

    start = N2I(t2);
    size = N2I(t3);
    dest = N2I(t4);

    if(start < 0) { start = 0; }
    n = N2I(string_get_bytes(t1));
    if(start >= n) { RET(msg->recv); }
    if(size > n - start) { size = n - start; }

    n = N2I(string_get_bytes(msg->recv));
    if(dest < 0) { dest = 0; }
    if(dest >= n) { RET(msg->recv); }
    if(size > n - dest) { size = n - dest; }

    a = rbx_string_as_cstr(state, msg->recv);
    b = rbx_string_as_cstr(state, t1);
    memcpy(a + dest, b + start, size);

    RET(msg->recv);
    CODE
  end

  defprim :string_compare_substring
  def string_compare_substring
    <<-CODE
    ARITY(3);
    GUARD(STRING_P(msg->recv));
    OBJECT t1, t2, t3;
    native_int bytes, start, size, cmp;
    char *a, *b;

    POP(t1, STRING);
    POP(t2, FIXNUM);
    POP(t3, FIXNUM);

    start = N2I(t2);
    size = N2I(t3);

    bytes = N2I(string_get_bytes(t1));
    if(start < 0) { start = bytes + start; }
    if(start >= bytes || start < 0) { FAIL(); }
    if(start + size > bytes) { size = bytes - start; }

    bytes = N2I(string_get_bytes(msg->recv));
    if(size > bytes) { size = bytes; }

    a = rbx_string_as_cstr(state, msg->recv);
    b = rbx_string_as_cstr(state, t1);
    cmp = memcmp(a, b + start, size);
    if(cmp < 0) {
      RET(I2N(-1));
    } else if(cmp > 0) {
      RET(I2N(1));
    } else {
      RET(I2N(0));
    }
    CODE
  end

  defprim :io_close_ng
  def io_close_ng
    <<-CODE
    ARITY(0);
    native_int j;

    GUARD(IO_P(msg->recv));

    j = io_to_fd(msg->recv);

    if(j == -1) {
      RAISE("IOError", "instance of IO already closed");
    } else if(close(j)) {
      RAISE_FROM_ERRNO("Unable to close IO object");
    } else {
      cpu_event_clear(state, j);
      io_set_descriptor(msg->recv, I2N(-1));
      RET(Qnil);
    }
    CODE
  end
end

prim = ShotgunPrimitives.new
prim.generate_select(STDOUT)
