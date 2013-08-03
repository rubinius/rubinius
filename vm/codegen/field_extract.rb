class BasicPrimitive
  attr_accessor :pass_state
  attr_accessor :pass_self
  attr_accessor :pass_call_frame
  attr_accessor :pass_message
  attr_accessor :pass_arguments
  attr_accessor :pass_gctoken
  attr_accessor :raw
  attr_accessor :safe
  attr_accessor :can_fail

  def output_header(str)
    str << "Object* Primitives::#{@name}(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args) {\n"
    str << "  state->set_call_frame(call_frame);\n"
    str << "  Object* ret;\n"
    return str if @raw
    str << "  Object* self;\n" if @pass_self
    str << "  GCTokenImpl gct;\n" if @pass_gctoken
  end

  def output_args(str, arg_types)
    unless @pass_arguments
      str << "  if(unlikely(args.total() != #{arg_types.size}))\n"
      str << "    goto fail;\n\n"
    end

    args = []
    i = -1
    arg_types.each do |t|
      i += 1
      case t
      when :arguments
        args << "args"
      else
        str << "  #{t}* a#{i};\n"
        str << "  a#{i} = try_as<#{t}>(args.get_argument(#{i}));\n"
        str << "  if(unlikely(a#{i} == NULL))\n"
        str << "    goto fail;\n\n"
        args << "a#{i}"
      end
    end
    args.unshift "gct" if @pass_gctoken
    args.unshift "self" if @pass_self
    args.unshift "state" if @pass_state

    if @pass_message
      args.push "exec"
      args.push "mod"
    end
    args.push "call_frame" if @pass_call_frame

    return args
  end

  def prim_return(str, indent=2)
    str << "#{' ' * indent}return ret;\n"
  end

  def output_call(str, call, args)
    str << "\n"
    str << "  try {\n"
    str << "#ifdef RBX_PROFILER\n"
    str << "    if(unlikely(state->vm()->tooling())) {\n"
    str << "      tooling::MethodEntry method(state, exec, mod, args);\n"
    str << "      RUBINIUS_METHOD_PRIMITIVE_ENTRY_HOOK(state, mod, args.name(), call_frame);\n"
    str << "      ret = #{call}(#{args.join(', ')});\n"
    str << "      RUBINIUS_METHOD_PRIMITIVE_RETURN_HOOK(state, mod, args.name(), call_frame);\n"
    str << "    } else {\n"
    str << "      RUBINIUS_METHOD_PRIMITIVE_ENTRY_HOOK(state, mod, args.name(), call_frame);\n"
    str << "      ret = #{call}(#{args.join(', ')});\n"
    str << "      RUBINIUS_METHOD_PRIMITIVE_RETURN_HOOK(state, mod, args.name(), call_frame);\n"
    str << "    }\n"
    str << "#else\n"
    str << "    RUBINIUS_METHOD_PRIMITIVE_ENTRY_HOOK(state, mod, args.name(), call_frame);\n"
    str << "    ret = #{call}(#{args.join(', ')});\n"
    str << "    RUBINIUS_METHOD_PRIMITIVE_RETURN_HOOK(state, mod, args.name(), call_frame);\n"
    str << "#endif\n"
    str << "  } catch(const RubyException& exc) {\n"
    str << "    exc.exception->locations(state,\n"
    str << "          Location::from_call_stack(state, call_frame));\n"
    str << "    state->raise_exception(exc.exception);\n"
    str << "    RUBINIUS_METHOD_PRIMITIVE_RETURN_HOOK(state, mod, args.name(), call_frame);\n"
    str << "    return NULL;\n"
    str << "  }\n"
    str << "\n"
    str << "  if(unlikely(ret == reinterpret_cast<Object*>(kPrimitiveFailed)))\n"
    str << "    goto fail;\n\n"
    prim_return(str);
    str << "fail:\n"
    str << "  return CompiledCode::primitive_failed(state, call_frame, exec, mod, args);\n"
    str << "}\n\n"
  end

end

class CPPPrimitive < BasicPrimitive
  def initialize(name, type)
    @type = type
    @name = name
  end

  attr_accessor :type, :name
  attr_accessor :cpp_name, :return_type, :arg_types

  def arg_count
    arg_types.size
  end

  def generate_glue
    str = ""
    if @raw
      arg_count = nil
    else
      arg_count = arg_types.size
    end

    output_header str

    str << "  #{@type}* recv = try_as<#{@type}>(args.recv());\n"
    str << "  if(unlikely(recv == NULL)) goto fail;\n"

    if @raw
      str << "\n"
      str << "  try {\n"
      str << "    ret = recv->#{@cpp_name}(state, call_frame, exec, mod, args);\n"
      str << "  } catch(const RubyException& exc) {\n"
      str << "    exc.exception->locations(state,\n"
      str << "          Location::from_call_stack(state, call_frame));\n"
      str << "    state->raise_exception(exc.exception);\n"
      str << "    return NULL;\n"
      str << "  }\n"
      str << "  if(ret == Primitives::failure()) goto fail;\n"
      str << "  return ret;\n"
      str << "\n"
      str << "fail:\n"
      str << "  return CompiledCode::primitive_failed(state, call_frame, exec, mod, args);\n"
      str << "}\n\n"
    else
      args = output_args str, arg_types
      str << "    self = recv;\n" if @pass_self

      output_call str, "recv->#{@cpp_name}", args
    end

    return str
  end

  def generate_jit_stub
    return if @raw or @pass_message or @pass_arguments

    # Default value, overriden below
    @can_fail = true

    str = ""
    if arg_types.empty?
      arg_list = ""
    else
      list = []
      arg_types.size.times { |i| list << "Object* ra#{i}" }
      arg_list = ", " + list.join(", ")
    end

    if @safe && !@pass_call_frame
      str << "extern \"C\" Object* jit_stub_#{@name}(STATE, Object* recv #{arg_list}) {\n"
    else
      str << "extern \"C\" Object* jit_stub_#{@name}(STATE, CallFrame* call_frame, Object* recv #{arg_list}) {\n"
    end

    str << "  Object* ret;\n"
    str << "  GCTokenImpl gct;\n" if @pass_gctoken

    emit_fail = false

    i = 0
    arg_types.each do |t|
      str << "  #{t}* a#{i};\n"
      i += 1
    end

    if @type == "Object"
      str << "  Object* self = recv;\n"
    else
      emit_fail = true
      str << "  #{@type}* self = try_as<#{@type}>(recv);\n"
      str << "  if(unlikely(self == NULL)) goto fail;\n"
    end

    args = []
    i = 0
    arg_types.each do |t|
      emit_fail = true
      str << "  a#{i} = try_as<#{t}>(ra#{i});\n"
      str << "  if(unlikely(a#{i} == NULL)) goto fail;\n"
      args << "a#{i}"
      i += 1
    end

    unless @safe
      str << "  state->set_call_frame(call_frame);\n"
    end

    args.unshift "gct" if @pass_gctoken
    args.unshift "recv" if @pass_self
    args.unshift "state" if @pass_state
    args.push "call_frame" if @pass_call_frame

    if @safe
      str << "  ret = self->#{@cpp_name}(#{args.join(', ')});\n"
      str << "  return ret;\n"
    else
      str << "\n"
      str << "  try {\n"
      str << "    ret = self->#{@cpp_name}(#{args.join(', ')});\n"
      str << "  } catch(const RubyException& exc) {\n"
      str << "    exc.exception->locations(state,\n"
      str << "          Location::from_call_stack(state, call_frame));\n"
      str << "    state->raise_exception(exc.exception);\n"
      str << "    return NULL;\n"
      str << "  }\n"
      str << "\n"
      str << "  if(unlikely(ret == reinterpret_cast<Object*>(kPrimitiveFailed)))\n"
      str << "    goto fail;\n\n"
      str << "  return ret;\n"
    end

    if !@safe or emit_fail
      str << "fail:\n"
      str << "  return cUndef;\n"
    end

    if @safe and !emit_fail
      @can_fail = false
    end

    str << "}\n\n"
  end

  def generate_invoke_stub
    return if @raw or @pass_arguments

    str = ""
    if arg_types.empty?
      arg_list = ""
    else
      list = []
      arg_types.size.times { |i| list << "Object* ra#{i}" }
      arg_list = ", " + list.join(", ")
    end

    str << "extern \"C\" Object* invoke_#{@name}(STATE, CallFrame* call_frame, Object** args, int arg_count) {\n"

    str << "  Object* ret;\n"
    str << "  GCTokenImpl gct;\n" if @pass_gctoken

    i = 0
    arg_types.each do |t|
      str << "  #{t}* a#{i};\n"
      i += 1
    end

    arity = arg_types.size + 1

    if @type == "Object"
      str << "  Object* self;\n"
    else
      str << "  #{@type}* self;\n"
    end

    str << "  if(arg_count != #{arity}) goto fail;\n"

    if @type == "Object"
      str << "  self = args[0];\n"
    else
      str << "  self = try_as<#{@type}>(args[0]);\n"
      str << "  if(unlikely(self == NULL)) goto fail;\n"
    end

    args = []
    i = 0
    arg_types.each do |t|
      emit_fail = true
      str << "  a#{i} = try_as<#{t}>(args[#{i + 1}]);\n"
      str << "  if(unlikely(a#{i} == NULL)) goto fail;\n"
      args << "a#{i}"
      i += 1
    end

    args.unshift "gct" if @pass_gctoken
    args.unshift "recv" if @pass_self
    args.unshift "state" if @pass_state
    args.push "call_frame" if @pass_call_frame

    str << "\n"
    str << "  try {\n"
    str << "    ret = self->#{@cpp_name}(#{args.join(', ')});\n"
    str << "  } catch(const RubyException& exc) {\n"
    str << "    exc.exception->locations(state,\n"
    str << "          Location::from_call_stack(state, call_frame));\n"
    str << "    state->raise_exception(exc.exception);\n"
    str << "    return NULL;\n"
    str << "  }\n"
    str << "\n"
    str << "  if(unlikely(ret == reinterpret_cast<Object*>(kPrimitiveFailed)))\n"
    str << "    goto fail;\n\n"
    str << "  return ret;\n"

    str << "fail:\n"
    str << "  Exception::internal_error(state, call_frame, \"invoked primitive failed\");"
    str << "  return 0;\n"

    str << "}\n\n"
  end
end

class CPPStaticPrimitive < CPPPrimitive
  def generate_glue
    str = ""
    output_header str

    if @raw
      str << "\n"
      str << "  try {\n"
      str << "    return #{@type}::#{@cpp_name}(state, exec, call_frame, mod);\n"
      str << "  } catch(const RubyException& exc) {\n"
      str << "    exc.exception->locations(state,\n"
      str << "          Location::from_call_stack(state, call_frame));\n"
      str << "    state->raise_exception(exc.exception);\n"
      str << "    return NULL;\n"
      str << "  }\n"
      str << "\n"
      str << "}\n\n"
    else
      args = output_args str, arg_types
      str << "    self = args.recv();\n" if @pass_self
      output_call str, "#{@type}::#{@cpp_name}", args
    end
    return str
  end

  def generate_jit_stub
    return if @raw or @pass_arguments

    # Default value, overriden below
    @can_fail = true

    str = ""
    if arg_types.empty?
      arg_list = ""
    else
      list = []
      arg_types.size.times { |i| list << "Object* ra#{i}" }
      arg_list = ", " + list.join(", ")
    end

    if @safe && !@pass_call_frame
      str << "extern \"C\" Object* jit_stub_#{@name}(STATE, Object* recv #{arg_list}) {\n"
    else
      str << "extern \"C\" Object* jit_stub_#{@name}(STATE, CallFrame* call_frame, Object* recv #{arg_list}) {\n"
    end

    str << "  Object* ret;\n"
    str << "  GCTokenImpl gct;\n" if @pass_gctoken

    i = 0
    arg_types.each do |t|
      str << "  #{t}* a#{i};\n"
      i += 1
    end

    emit_fail = false

    args = []
    i = 0
    arg_types.each do |t|
      emit_fail = true
      str << "  a#{i} = try_as<#{t}>(ra#{i});\n"
      str << "  if(unlikely(a#{i} == NULL)) goto fail;\n"
      args << "a#{i}"
      i += 1
    end

    args.unshift "gct" if @pass_gctoken
    args.unshift "recv" if @pass_self
    args.unshift "state" if @pass_state
    args.push "call_frame" if @pass_call_frame

    if @safe
      str << "  ret = #{@type}::#{@cpp_name}(#{args.join(', ')});\n"
      str << "  return ret;\n"
    else
      str << "\n"
      str << "  try {\n"
      str << "    ret = #{@type}::#{@cpp_name}(#{args.join(', ')});\n"
      str << "  } catch(const RubyException& exc) {\n"
      str << "    exc.exception->locations(state,\n"
      str << "          Location::from_call_stack(state, call_frame));\n"
      str << "    state->raise_exception(exc.exception);\n"
      str << "    return NULL;\n"
      str << "  }\n"
      str << "\n"
      str << "  if(unlikely(ret == reinterpret_cast<Object*>(kPrimitiveFailed)))\n"
      str << "    goto fail;\n\n"
      str << "  return ret;\n"
    end

    if !@safe or emit_fail
      str << "fail:\n"
      str << "  return cUndef;\n"
    end

    if @safe and !emit_fail
      @can_fail = false
    end

    str << "}\n\n"
  end

  def generate_invoke_stub
    return if @raw or @pass_arguments

    str = ""
    if arg_types.empty?
      arg_list = ""
    else
      list = []
      arg_types.size.times { |i| list << "Object* ra#{i}" }
      arg_list = ", " + list.join(", ")
    end

    str << "extern \"C\" Object* invoke_#{@name}(STATE, CallFrame* call_frame, Object** args, int arg_count) {\n"

    str << "  Object* ret;\n"
    str << "  GCTokenImpl gct;\n" if @pass_gctoken

    i = 0
    arg_types.each do |t|
      str << "  #{t}* a#{i};\n"
      i += 1
    end

    arity = arg_types.size
    arity += 1 if @pass_self
    str << "  if(arg_count != #{arity}) goto fail;\n"

    args = []
    i = 0
    arg_types.each do |t|
      str << "  a#{i} = try_as<#{t}>(args[#{i}]);\n"
      str << "  if(unlikely(a#{i} == NULL)) goto fail;\n"
      args << "a#{i}"
      i += 1
    end

    args.unshift "gct" if @pass_gctoken
    args.unshift "args[arg_count-1]" if @pass_self
    args.unshift "state" if @pass_state
    args.push "call_frame" if @pass_call_frame

    str << "\n"
    str << "  try {\n"
    str << "    ret = #{@type}::#{@cpp_name}(#{args.join(', ')});\n"
    str << "  } catch(const RubyException& exc) {\n"
    str << "    exc.exception->locations(state,\n"
    str << "          Location::from_call_stack(state, call_frame));\n"
    str << "    state->raise_exception(exc.exception);\n"
    str << "    return NULL;\n"
    str << "  }\n"
    str << "\n"
    str << "  if(unlikely(ret == reinterpret_cast<Object*>(kPrimitiveFailed)))\n"
    str << "    goto fail;\n\n"
    str << "  return ret;\n"

    str << "fail:\n"
    str << "  Exception::internal_error(state, call_frame, \"invoked primitive failed\");"
    str << "  return 0;\n"

    str << "}\n\n"
  end
end

class CPPOverloadedPrimitive < BasicPrimitive
  def initialize(prim)
    @name     = prim.name
    @type     = prim.type
    @cpp_name = prim.cpp_name
    @kinds    = [prim]
  end

  def add_kind(prim)
    if prim.arg_types.size != 1
      raise "Invalid overloaded primitive: Only 2nd arg can be overloaded."
    end
    @kinds << prim
  end

  def generate_glue
    str = ""
    output_header str

    str << "  #{@type}* recv = try_as<#{@type}>(args.recv());\n"
    str << "  if(likely(recv)) {\n"
    str << "    if(args.total() != 1) goto fail;\n"

    @kinds.each do |prim|
      type = prim.arg_types.first
      str << "    if(#{type}* arg = try_as<#{type}>(args.get_argument(0))) {\n"
      str << "      try {\n"
      if @pass_state
        call = "        ret = recv->#{@cpp_name}(state, arg);\n"
      else
        call = "        ret = recv->#{@cpp_name}(arg);\n"
      end
      str << "#ifdef RBX_PROFILER\n"
      str << "        if(unlikely(state->vm()->tooling())) {\n"
      str << "          tooling::MethodEntry method(state, exec, mod, args);\n"
      str << "  " << call
      str << "        } else {\n"
      str << "  " << call
      str << "        }\n"
      str << "#else\n"
      str << call
      str << "#endif\n"
      str << "      } catch(const RubyException& exc) {\n"
      str << "        exc.exception->locations(state,\n"
      str << "              Location::from_call_stack(state, call_frame));\n"
      str << "        state->raise_exception(exc.exception);\n"
      str << "        return NULL;\n"
      str << "      }\n"
      str << "      if(likely(ret != reinterpret_cast<Object*>(kPrimitiveFailed))) {\n"
      prim_return(str, 8);
      str << "      }\n"
      str << "    }\n"
      str << "\n"
    end

    str << "  }\n"
    str << "fail:\n"
    str << "  return CompiledCode::primitive_failed(state, call_frame, exec, mod, args);\n"
    str << "}\n\n"
    return str
  end
end

class CPPClass
  def initialize(name)
    @name = name
    @super = nil
    @fields = []
    @flags = {}
    @primitives = {}
    @num_fields = 0
    @subclasses = []
  end

  attr_accessor :name, :super, :fields, :primitives, :flags, :num_fields
  attr_accessor :subclasses

  def add_field(idx, name, type, flag=nil)
    if flag
      @flags[name] = flag
    end

    @fields << [name, type, idx]
  end

  def field_index_offset
    return @super.all_fields if @super
    return 0
  end

  def all_fields
    if @super
      ary = @super.all_fields
    else
      ary = []
    end

    offset = ary.size
    @fields.each do |n,t,i|
      if readonly?(n)
        flags = {:readonly => true}
      else
        flags = {}
      end
      ary << [n, t, i + offset, flags]
    end

    return ary
  end

  def access_primitives
    all_fields.map do |name, type, offset, flags|
      "access_#{@name}_#{name}"
    end
  end

  def readonly?(name)
    @flags[name] == "readonly"
  end

  def add_primitive(name, cpp_name, ret, args, overload=false)
    prim = CPPPrimitive.new(name, @name)
    prim.cpp_name = cpp_name
    prim.return_type = ret
    prim.arg_types = args

    if cur = @primitives[name]
      unless overload
        raise "Illegal overloading of primitive #{name}"
      end

      unless prim.arg_types.size == 1
        raise "Overloaded variant of #{name} must only take one arg"
      end

      if cur.kind_of? CPPOverloadedPrimitive
        cur.add_kind prim
        return cur
      else
        over = CPPOverloadedPrimitive.new(cur)
        @primitives[name] = over
        over.add_kind prim
        return over
      end

    else
      @primitives[name] = prim
    end

    return prim
  end

  def add_static_primitive(name, cpp_name, ret, args)
    prim = CPPStaticPrimitive.new(name, @name)
    prim.cpp_name = cpp_name
    prim.return_type = ret
    prim.arg_types = args

    if @primitives[name]
      raise "Already defined primitive #{name}"
    end

    @primitives[name] = prim
    return prim
  end

  def generate_gets
    str = ""
    all_fields.each do |name, type, idx, flags|
      str << "  case #{idx}:\n"
      str << "    return target->#{name}();\n"
    end

    return str
  end

  def generate_sets
    str = ""
    all_fields.each do |name, type, idx, flags|
      str << "  case #{idx}:\n"
      if flags[:readonly]
        str << "    Exception::assertion_error(state, \"#{name} is readonly\");\n"
      else
        str << "    target->#{name}(state, val->nil_p() ? nil<#{type}>() : as<#{type}>(val));\n"
      end
      str << "    return;\n"
    end

    return str
  end

  def generate_accessors
    str = ""
    all_fields.each do |name, type, idx, flags|
      str << "Object* Primitives::access_#{@name}_#{name}(STATE, CallFrame* call_frame, Executable* exec, Module* mod,
                   Arguments& args) {\n"
      str << "  AccessVariable* access = as<AccessVariable>(exec);\n"
      str << "  if(access->write()->true_p()) {\n"
      str << <<-ARGS
        if(args.total() != 1) {
          Exception::argument_error(state, 1, args.total());
          return NULL;
        }
      ARGS
      str << "    Object* val = args.get_argument(0);\n"
      str << "    if(val->nil_p()) {\n"
      str << "      as<#{@name}>(args.recv())->#{name}(state, nil<#{type}>());\n"
      str << "    } else {\n"
      str << "      as<#{@name}>(args.recv())->#{name}(state, as<#{type}>(val));\n"
      str << "    }\n"
      str << "    return args.get_argument(0);\n"
      str << "  }\n"
      str << <<-ARGS
        if(args.total() != 0) {
          Exception::argument_error(state, 0, args.total());
          return NULL;
        }
      ARGS
      str << "  return as<#@name>(args.recv())->#{name}();\n"
      str << "}\n\n"
    end

    return str

  end

  def generate_typechecks
    out = generate_sets()
    return '' if out.strip.empty?

    <<-EOF
void #{@name}::Info::set_field(STATE, Object* _t, size_t index, Object* val) {
  #{@name}* target = as<#{@name}>(_t);

  switch(index) {
#{out}  }
}

Object* #{@name}::Info::get_field(STATE, Object* _t, size_t index) {
  #{@name}* target = as<#{@name}>(_t);

  switch(index) {
#{generate_gets}  }

  std::ostringstream error;
  error << "Unable to access field " << index << " in a #{@name} instance";

  Exception::assertion_error(state, error.str().c_str());

  return cNil; // never reached
}

    EOF
  end

  def generate_marks(cpp)
    str = ''

    str << generate_marks(cpp.super) if cpp.super

    cpp.fields.each do |name, type, idx|
      str << <<-EOF
  {
    if(target->#{name}()->reference_p()) {
      Object* old = target->#{name}();
      Object* cur = mark.call(old);
      if(cur && cur != old) target->#{name}(mark.state(), force_as<#{type}>(cur));
    }
  }

      EOF
    end

    return str
  end

  def generate_visits(cpp)
    str = ''

    str << generate_visits(cpp.super) if cpp.super

    cpp.fields.each do |name, type, idx|
      str << <<-EOF
    visit.call(target->#{name}());
      EOF
    end

    return str

  end

  def generate_mark
    marks = generate_marks(self).rstrip

    str = ''

    str << <<-EOF unless marks.empty?
void #{@name}::Info::auto_mark(Object* _t, ObjectMark& mark) {
  #{@name}* target = as<#{@name}>(_t);

#{marks}
}

    EOF

    str
  end

  def kind_of_code(what)
    case @name
    when "Fixnum"
      return "#{what}->fixnum_p()"
    when "Symbol"
      return "#{what}->symbol_p()"
    when "TrueClass"
      return "#{what}->true_p()"
    when "FalseClass"
      return "#{what}->false_p()"
    when "NilClass"
      return "#{what}->nil_p()"
    else
      return "(#{what}->reference_p() && #{what}->type_id() == #{@name}Type)"
    end
  end

  def kind_of_conditions(what)
    [kind_of_code(what)] + @subclasses.map do |cpp|
      cpp.kind_of_conditions(what)
    end
  end

  def generate_kind_of
    checks = kind_of_conditions("obj")

    # We forward declare the class in here to keep everything happy
    str = <<-CPP
template <>
SPECIALIZATION_STORAGE bool kind_of<#{@name}>(const Object* obj) {
  return #{checks.join(' || ')};
}

    CPP
  end

  def class_has_object_size
    @flags[:object_size] = true
  end

  def class_has_object_size?
    @flags[:object_size]
  end
end

class CPPParser

  attr_reader :classes, :class_order

  def initialize
    @classes = Hash.new
    @class_order = []

    @type_map = {
      "InstructionSequence" => :InstructionSequence,
      "Symbol" => :Symbol,
      "Object" => :Object,
      "Tuple"  => :Tuple,
      "MemoryPointer" => :MemoryPointer,
      "ConstantScope" => :ConstantScope,
      "Integer" => :Integer,
      "Fixnum" => :Fixnum,
      "CompiledCode" => :CompiledCode,
      "String" => :String,
      "Module" => :Module,
      "Class" => :Class,
      "Float" => :Float,
      "Bignum" => :Bignum,
      "Hash" => :Hash,
      "Channel" => :Channel
    }

  end

  def add_class(name, obj)
    @class_order << name unless @class_order.include? name
    @classes[name] = obj
  end

  def strip_and_map(type, map)
    if type[-1] == ?*
      type = type[0..-2]
    end

    if val = map[type]
      return val
    end

    raise "Unable to resolve type '#{type}'"
  end

  def parse_stream(f)
    class_pattern = /class\s+([^\s]+)\s*:\s*public\s+([^\s]+)/
    slot_pattern = %r!^\s*(\w+)\*?\s+\*?(\w+)_\s*;\s*//\s*slot(.*)!
    primitive_pattern = %r%^\s*//\s+Rubinius.primitive([?!\+])?\s+:(.*)\s*$%
    prototype_pattern = %r!\s*(static\s+)?([\w\*]+)\s+([\w]+)\((.*)\)!
    object_size_pattern = %r|size_t\s+object_size\s*\(const\s+ObjectHeader\s*|

    while l = f.gets
      next unless m = class_pattern.match(l)
      if m[2] != "Object" and m[2] != "ObjectHeader"
        # We've found a class definition that doesn't extend Object.
        # If we recognize the superclass, make a new CPPClass instance
        # for this class definition.
        if sup = @classes[m[2]]
          cpp = CPPClass.new(m[1])
          cpp.super = sup
          sup.subclasses << cpp
        else
          next
        end
      else
        # A class definition directly subclassing Object or ObjectHeader
        cpp = CPPClass.new(m[1])
      end

      add_class(cpp.name, cpp)

      @type_map[cpp.name] = cpp.name.to_sym

      # We are finished with this class if it was a one-liner.
      next if /\};/.match(l)

      # Otherwise, keep scanning to look for fields and primitives
      idx = 0
      while l = f.gets
        break if /^\s*class\s+/.match(l)

        # Field declarations marked with a "// slot" comment
        if m = slot_pattern.match(l)
          type = m[1]
          name = m[2]

          # Optional 'flag' argument after '// slot'
          # Currently 'readonly' is the only supported flag
          if m[3]
            flag = m[3].strip
          else
            flag = nil
          end

          field_type = @type_map[type] || type.to_s

          cpp.add_field idx, name, field_type, flag
          idx += 1
        # A primitive declaration marked with '// Rubinius.primitive'
        elsif m = primitive_pattern.match(l)
          overload = m[1] == "!"
          raw = m[1] == "?"
          safe = m[1] == "+"
          prim = m[2]
          prototype = f.gets
          pass_state = false
          pass_self = false
          pass_call_frame = false
          pass_message = false
          pass_arguments = false
          pass_gctoken = false

          m = prototype_pattern.match(prototype)
          unless m
            raise "Unable to parse: '#{prototype}'"
          end
          args = m[4].split(/\s*,\s*/)
          # If the first argument is the +STATE+ macro, handle it in +output_args+
          if args.first == "STATE"
            args.shift and pass_state = true

            if args.first =~ /GCToken .*/
              args.shift
              pass_gctoken = true
            end


            # If the second argument is +Object* self+, we will automatically pass
            # in the receiver of the primitive message in +output_call+
            if args.first == "Object* self"
              args.shift and pass_self = true
            end
          end

          if args.last == "CallFrame* calling_environment"
            pass_call_frame = true
            args.pop
          end

          if i = args.index("Arguments& args")
            pass_arguments = true
            args[i] = :arguments
          end

          if raw then
            arg_types = []
          else
            arg_types = args.map do |a|
              case a
              when Symbol
                a
              else
                strip_and_map(a.split(/\s+/, 2).first, @type_map)
              end
            end
          end

          if m[1]
            if overload
              raise "Unable to overload static primitives."
            end
            obj = cpp.add_static_primitive prim, m[3], @type_map[m[2]], arg_types
          else
            obj = cpp.add_primitive prim, m[3], @type_map[m[2]], arg_types, overload
          end

          obj.safe = safe
          obj.raw = raw
          obj.pass_state = pass_state
          obj.pass_self = pass_self
          obj.pass_call_frame = pass_call_frame
          obj.pass_message = pass_message
          obj.pass_arguments = pass_arguments
          obj.pass_gctoken = pass_gctoken
        elsif object_size_pattern.match(l)
          cpp.class_has_object_size
        end
      end

      # If ::Info class has object_size method, we cannot simply return
      # sizeof of it.
      if l =~ /TypeInfo/
        while l = f.gets
          break if /^\s*class/.match(l)
          cpp.class_has_object_size if object_size_pattern.match(l)
        end
      elsif l && m = l.match(/public\s+(\S+)::Info/)
        # A class definition subclasses another Info class. Probably, should check
        # here for object_size either if subclass doesn't have one.
        superclass = @classes[m[1]]
        cpp.class_has_object_size if superclass && superclass.class_has_object_size?
      end

      cpp.num_fields = idx + 1
      redo if l
    end
  end
end

parser = CPPParser.new

includes = ARGV.map do |include|
  "#include \"#{include.sub(/^vm\//, '')}\""
end.join "\n"

inputs = ARGV.sort
builtins = Dir['vm/builtin/*.hpp'].sort

missing = builtins - inputs

DEV_NULL = RUBY_PLATFORM =~ /mingw|mswin/ ? 'NUL' : '/dev/null'

unless missing.empty? then
  abort "Missing #{missing.join ', '} in rakelib/vm.rake's field_extract list"
end

ARGV.each do |file|
  File.open(file) do |f|
    parser.parse_stream f
  end
end

def write_if_new(path)
  tmp_path = "#{path}.tmp"

  File.open tmp_path, 'w' do |f|
    yield f
  end

  File.rename tmp_path, path unless system("diff -q #{path} #{tmp_path} > #{DEV_NULL} 2>&1")
ensure
  File.unlink tmp_path if File.exist? tmp_path
end

write_if_new 'vm/gen/includes.hpp' do |f|
  f << <<-EOF
#ifndef GEN_INCLUDES_HPP
#define GEN_INCLUDES_HPP

// DO NOT INCLUDE THIS IN SOMETHING THAT DOES NOT INCLUDE A GENERATED FILE

#{includes}

#endif
  EOF
end

write_if_new "vm/gen/typechecks.gen.cpp" do |f|
  f.puts "size_t TypeInfo::instance_sizes[(int)LastObjectType] = {ObjectHeader::align(sizeof(Object))};"
  f.puts "void TypeInfo::auto_init(ObjectMemory* om) {"
  parser.classes.sort_by {|n, _| n }.each do |n, cpp|
    f.puts "  {"
    f.puts "    TypeInfo *ti = new #{n}::Info(#{n}::type);"
    f.puts "    ti->type_name = std::string(\"#{n}\");"
    if cpp.class_has_object_size?
      f.puts "    TypeInfo::instance_sizes[#{n}::type] = 0;"
    else
      f.puts "    TypeInfo::instance_sizes[#{n}::type] = ObjectHeader::align(sizeof(#{n}));"
    end
    f.puts "    ti->instance_size = ObjectHeader::align(sizeof(#{n}));"
    f.puts "    om->add_type_info(ti);"
    f.puts "  }"
    f.puts
  end
  f.puts "}"
  f.puts

  f.puts "void TypeInfo::auto_learn_fields(STATE) {"
  parser.classes.sort_by {|n, _| n }.each do |n, cpp|
    f.puts "  {"
    f.puts "    TypeInfo* ti = state->vm()->find_type(#{n}::type);"
    f.puts "    ti->set_state(state);"

    fields = cpp.all_fields
    f.puts "    ti->slot_accessors.resize(#{fields.size});\n"
    f.puts "    ti->slot_types.resize(#{fields.size});\n"
    fields.each do |name, type, idx|
      symbol_name = "access_#{n}_#{name}"
      f.puts "    ti->slots[state->symbol(\"@#{name}\")->index()] = #{idx};"
      f.puts "    ti->slot_types[#{idx}] = #{type}::type;"
      f.puts "    ti->slot_accessors[#{idx}] = &Primitives::#{symbol_name};"
    end
    f.puts "    ti->populate_slot_locations();"
    f.puts "  }"
    f.puts
  end
  f.puts "}"

  parser.classes.sort_by {|n, _| n }.each do |n, cpp|
    next if cpp.all_fields.size == 0

    f.puts "void #{n}::Info::populate_slot_locations() {"

    if cpp.super
      f.puts "  this->#{cpp.super.name}::Info::populate_slot_locations();\n"
      offset = cpp.super.all_fields.size
    else
      offset = 0
    end

    f.puts "  slot_locations.resize(#{cpp.all_fields.size});\n"

    cpp.fields.each do |name, type, idx|
      f.puts "  slot_locations[#{offset + idx}] = FIELD_OFFSET(#{n}, #{name}_);\n"
    end
    f.puts "}"
  end

  parser.classes.sort_by {|n, _| n }.each do |n, cpp|
    f.puts cpp.generate_typechecks
    f.puts cpp.generate_mark
  end
end

write_if_new "vm/gen/primitives_declare.hpp" do |f|
  total_prims = 0
  parser.classes.sort_by {|n, _| n }.each do |n, cpp|
    total_prims += cpp.primitives.size
    cpp.primitives.map{|pn, prim| pn }.sort.each do |pn|
      f.puts "static Object* #{pn}(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);"
    end

    total_prims += cpp.access_primitives.size
    cpp.access_primitives.sort.each do |name|
      f.puts "static Object* #{name}(STATE, CallFrame* call_frame, Executable* exec, Module* mod, Arguments& args);"
    end
  end

  f.puts "const static int cTotalPrimitives = #{total_prims};"
end

write_if_new "vm/gen/object_types.hpp" do |f|
  stripped = parser.classes.reject {|n, cpp| cpp.name =~ /(Nil|True|False)Class/ }

  f.puts "typedef enum {"
  f.puts "  InvalidType = 0,"
  f.puts "  NilType,"
  f.puts "  TrueType,"
  f.puts "  FalseType,"
  stripped.map { |n, cpp| cpp.name }.sort.each do |name|
    f.puts "  #{name}Type,"
  end
  f.puts "  LastObjectType"
  f.puts "} object_type;"
  f.puts
end

write_if_new "vm/gen/kind_of.hpp" do |f|
  parser.classes.sort_by {|n, _| n }.each do |n, cpp|
    next if cpp.name == "Object"
    f.puts "class #{cpp.name};"
    f.puts cpp.generate_kind_of
  end
end

class PrimitiveCodeGenerator
  def initialize(parser)
    classes = parser.classes.sort_by { |n, | n }

    @indexes = {}
    @jit_functions = []
    @invoke_functions = []

    @names = []
    @primitives = []

    @files = classes.map do |n, file|
      primitives = file.primitives.sort_by { |n, | n }
      primitives.each { |n, p| @names << n }

      @names += file.access_primitives

      @primitives += primitives

      file
    end

    @names.sort!
  end

  def generate
    method_primitives
    jit_primitives
    invoke_primitives
    accessor_primitives
    method_resolver
    jit_resolver
    invoke_resolver
  end

  def method_primitives
    write_if_new "vm/gen/method_primitives.cpp" do |f|
      @primitives.each { |_, p| f << p.generate_glue }
    end
  end

  def jit_primitives
    write_if_new "vm/gen/jit_primitives.cpp" do |f|
      @primitives.each do |n, p|
        if p.respond_to? :generate_jit_stub
          code = p.generate_jit_stub
          if code
            f << code
            @jit_functions << [n, p]
          end
        end
      end
    end
  end

  def invoke_primitives
    write_if_new "vm/gen/invoke_primitives.cpp" do |f|
      @primitives.each do |n, p|
        if p.respond_to? :generate_invoke_stub
          code = p.generate_invoke_stub
          if code
            f << code
            @invoke_functions << [n, p]
          end
        end
      end
    end
  end

  def accessor_primitives
    write_if_new "vm/gen/accessor_primitives.cpp" do |f|
      @files.each { |file| f << file.generate_accessors }
    end
  end

  def method_resolver
    write_if_new "vm/gen/method_resolver.cpp" do |f|
      f.puts "executor Primitives::resolve_primitive(STATE, Symbol* name, int* index) {"

      @names.each_with_index do |name, index|
        @indexes[name] = index

        f.puts <<-EOC
  if(name == state->symbol("#{name}", #{name.bytesize})) {
    if(index) *index = #{index};
    return &Primitives::#{name};
  }

        EOC
      end

      f.puts <<-EOC
  //  if(!state->probe()->nil_p()) {
  //    state->probe()->missing_primitive(state, name->c_str(state));
  //  }
  return &Primitives::unknown_primitive;
  // commented out while we have soft primitive failures
  // throw std::runtime_error(msg.c_str());
}
      EOC
    end
  end

  def jit_resolver
    write_if_new "vm/gen/jit_resolver.cpp" do |f|
      f.puts "bool Primitives::get_jit_stub(int index, JITStubResults& res) {"
      f.puts "  switch(index) {"

      @jit_functions.each do |n, p|
        f.puts "  case #{@indexes[n]}: // #{n}"
        f.puts "    res.set_arg_count(#{p.arg_count});"
        f.puts "    res.set_name(\"jit_stub_#{n}\");"
        f.puts "    res.set_pass_callframe(false);" if p.safe && !p.pass_call_frame
        f.puts "    res.set_can_fail(false);" unless p.can_fail
        f.puts "    return true;"
      end

      f.puts "  }"
      f.puts "  return false;"
      f.puts "}"
    end
  end

  def invoke_resolver
    write_if_new "vm/gen/invoke_resolver.cpp" do |f|
      f.puts "InvokePrimitive Primitives::get_invoke_stub(STATE, Symbol* name) {"

      @invoke_functions.each do |n, |
        f.puts <<-EOC
  if(name == state->symbol("#{n}", #{n.bytesize})) {
    return &invoke_#{n};
  }
        EOC
      end

      f.puts "  return &invoke_unknown_primitive;"
      f.puts "}"
    end
  end
end

PrimitiveCodeGenerator.new(parser).generate
