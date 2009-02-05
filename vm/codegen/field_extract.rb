class BasicPrimitive
  attr_accessor :pass_state
  attr_accessor :pass_self
  attr_accessor :pass_call_frame
  attr_accessor :raw

  def output_header(str)
    str << "Object* Primitives::#{@name}(STATE, CallFrame* call_frame, Message& msg) {\n"
    # str << " std::cout << \"[Primitive #{@name}]\\n\";\n"
    return str if @raw
    str << "  Object* ret;\n"
    str << "  Object* self;\n" if @pass_self
  end

  def output_args(str, arg_types)
    str << "  if(unlikely(msg.args() != #{arg_types.size}))\n"
    str << "    goto fail;\n\n"

    args = []
    i = -1
    arg_types.each do |t|
      i += 1
      str << "  #{t}* a#{i};\n"
      str << "  a#{i} = try_as<#{t}>(msg.get_argument(#{i}));\n"
      str << "  if(unlikely(a#{i} == NULL))\n"
      str << "    goto fail;\n\n"
      args << "a#{i}"
    end
    args.unshift "self" if @pass_self
    args.unshift "state" if @pass_state

    args.push "call_frame" if @pass_call_frame

    return args
  end

  def prim_return(str, indent=2)
    str << "#{' ' * indent}return ret;\n"
  end

  def output_call(str, call, args)
    str << "\n"
    str << "  try {\n"
    #str << "    if(unlikely(task->profiler)) task->profiler->enter_primitive(state, msg);\n"
    str << "    ret = #{call}(#{args.join(', ')});\n"
    #str << "    if(unlikely(task->profiler)) task->profiler->leave_method();\n"
    str << "  } catch(const RubyException& exc) {\n"
    str << "    state->thread_state()->raise_exception(exc.exception);\n"
    str << "    return Qnil;\n"
    str << "  }\n"
    str << "\n"
    str << "  if(unlikely(ret == reinterpret_cast<Object*>(kPrimitiveFailed)))\n"
    str << "    goto fail;\n\n"
    prim_return(str);
    str << "fail:\n"
    str << "  return VMMethod::execute(state, call_frame, msg);\n"
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

  def generate_glue
    str = ""
    if @raw
      arg_count = nil
    else
      arg_count = arg_types.size
    end

    output_header str

    str << "  #{@type}* recv = try_as<#{@type}>(msg.recv);\n"
    str << "  if(unlikely(recv == NULL)) goto fail;\n"

    if @raw
      str << "\n"
      str << "  return recv->#{@cpp_name}(state, msg.method, call_frame, msg);\n"
      str << "\n"
      str << "fail:\n"
      str << "  return VMMethod::execute(state, call_frame, msg);\n"
      str << "}\n\n"
    else
      args = output_args str, arg_types
      str << "    self = recv;\n" if @pass_self

      output_call str, "recv->#{@cpp_name}", args
    end

    return str
  end
end

class CPPStaticPrimitive < CPPPrimitive
  def generate_glue
    str = ""
    output_header str

    if @raw
      str << "\n"
      str << "  return #{@type}::#{@cpp_name}(state, msg.method, call_frame, msg);\n"
      str << "\n"
      str << "}\n\n"
    else
      args = output_args str, arg_types
      str << "    self = msg.recv;\n" if @pass_self

      output_call str, "#{@type}::#{@cpp_name}", args
    end
    return str
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

    str << "  #{@type}* recv = try_as<#{@type}>(msg.recv);\n"
    str << "  if(likely(recv)) {\n"
    str << "    if(msg.args() != 1) goto fail;\n"

    @kinds.each do |prim|
      type = prim.arg_types.first
      str << "    if(#{type}* arg = try_as<#{type}>(msg.get_argument(0))) {\n"
      str << "      try {\n"
      #str << "        if(unlikely(task->profiler))\n"
      #str << "          task->profiler->enter_primitive(state, msg);\n"
      if @pass_state
        str << "      ret = recv->#{@cpp_name}(state, arg);\n"
      else
        str << "      ret = recv->#{@cpp_name}(arg);\n"
      end
      #str << "        if(unlikely(task->profiler)) task->profiler->leave_method();\n"
      str << "      } catch(const RubyException& exc) {\n"
      str << "        state->thread_state()->raise_exception(exc.exception);\n"
      str << "        return Qnil;\n"
      str << "      }\n"
      str << "      if(likely(ret != reinterpret_cast<Object*>(kPrimitiveFailed))) {\n"
      prim_return(str, 8);
      str << "      }\n"
      str << "    }\n"
      str << "\n"
    end

    str << "  }\n"
    str << "fail:\n"
    str << "  return VMMethod::execute(state, call_frame, msg);\n"
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
      if type == :MethodContext or type == :BlockContext
        str << "    if(MethodContext* sub = try_as<MethodContext>(target->#{name}())) {\n"
        str << "      sub->reference(state);\n"
        str << "    }\n"
      end
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
        str << "    target->#{name}(state, val->nil_p() ? (#{type}*)Qnil : as<#{type}>(val));\n"
      end
      str << "    return;\n"
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

  std::stringstream error;
  error << "Unable to access field " << index << " in a #{@name} instance";

  Exception::assertion_error(state, error.str().c_str());

  return Qnil; // never reached
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
      Object* res = mark.call(target->#{name}());
      if(res) target->#{name}(mark.gc->object_memory->state, (#{type}*)res);
    }
  }

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
      return "FIXNUM_P(#{what})"
    when "Symbol"
      return "SYMBOL_P(#{what})"
    when "TrueClass"
      return "#{what} == Qtrue"
    when "FalseClass"
      return "#{what} == Qfalse"
    when "NilClass"
      return "#{what} == Qnil"
    else
      return "(REFERENCE_P(#{what}) && #{what}->obj_type == #{@name}Type)"
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
      "StaticScope" => :StaticScope,
      "Integer" => :Integer,
      "Fixnum" => :Fixnum,
      "MethodContext" => :MethodContext,
      "CompiledMethod" => :CompiledMethod,
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
    class_pattern = /class ([^\s]+)\s*:\s*public\s+([^\s]+)/
    slot_pattern = %r!^\s*(\w+)\*?\s+\*?(\w+)_\s*;\s*//\s*slot(.*)!
    primitive_pattern = %r%^\s*//\s+Ruby.primitive([?!])?\s+:(.*)\s*$%
    prototype_pattern = %r!\s*(static\s+)?([\w\*]+)\s+([\w]+)\((.*)\)!

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
        break if /^\s*class/.match(l)

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
        # A primitive declaration marked with '// Ruby.primitive'
        elsif m = primitive_pattern.match(l)
          overload = m[1] == "!"
          raw = m[1] == "?"
          prim = m[2]
          prototype = f.gets
          pass_state = false
          pass_self = false
          pass_call_frame = false

          m = prototype_pattern.match(prototype)
          unless m
            raise "Unable to parse: '#{prototype}'"
          end
          args = m[4].split(/\s*,\s*/)
          # If the first argument is the +STATE+ macro, handle it in +output_args+
          if args.first == "STATE"
            args.shift and pass_state = true
            # If the second argument is +Object* self+, we will automatically pass
            # in the receiver of the primitive message in +output_call+
            if args.first == "Object* self"
              args.shift and pass_self = true
            end
          end

          if args.last == "CallFrame *calling_environment"
            pass_call_frame = true
            args.pop
          end

          if raw then
            arg_types = []
          else
            arg_types = args.map { |a| strip_and_map(a.split(/\s+/, 2).first, @type_map) }
          end

          if m[1]
            if overload
              raise "Unable to overload static primitives."
            end
            obj = cpp.add_static_primitive prim, m[3], @type_map[m[2]], arg_types
          else
            obj = cpp.add_primitive prim, m[3], @type_map[m[2]], arg_types, overload
          end

          obj.raw = raw
          obj.pass_state = pass_state
          obj.pass_self = pass_self
          obj.pass_call_frame = pass_call_frame
        end
      end

      cpp.num_fields = idx + 1
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

  File.rename tmp_path, path unless system("diff -q #{path} #{tmp_path} > /dev/null 2>&1")
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
  f.puts "void TypeInfo::auto_init(ObjectMemory* om) {"
  parser.classes.each do |n, cpp|
    f.puts "  {"
    f.puts "    TypeInfo *ti = new #{n}::Info(#{n}::type);"
    f.puts "    ti->type_name = std::string(\"#{n}\");"
    f.puts "    ti->instance_size = sizeof(#{n});"
    f.puts "    om->type_info[#{n}::type] = ti;"
    f.puts "  }"
    f.puts
  end
  f.puts "}"
  f.puts

  f.puts "void TypeInfo::auto_learn_fields(STATE) {"
  parser.classes.each do |n, cpp|
    f.puts "  {"
    f.puts "    TypeInfo* ti = state->find_type(#{n}::type);"
    f.puts "    ti->set_state(state);"
    cpp.all_fields.each do |name, type, idx|
      f.puts "    ti->slots[state->symbol(\"@#{name}\")->index()] = #{idx};"
    end
    f.puts "  }"
    f.puts
  end

  f.puts "}"

  parser.classes.each do |n, cpp|
    f.puts cpp.generate_typechecks
    f.puts cpp.generate_mark
  end
end

write_if_new "vm/gen/primitives_declare.hpp" do |f|
  parser.classes.each do |n, cpp|
    cpp.primitives.each do |pn, prim|
      f.puts "static Object* #{pn}(STATE, CallFrame* call_frame, Message& msg);"
    end
  end
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
  parser.classes.each do |n, cpp|
    next if cpp.name == "Object"
    f.puts "class #{cpp.name};"
    f.puts cpp.generate_kind_of
  end
end

write_if_new "vm/gen/primitives_glue.gen.cpp" do |f|
  names = []
  parser.classes.sort_by { |name,| name }.each do |n, cpp|
    cpp.primitives.sort_by { |name,| name }.each do |pn, prim|
      names << pn

      f << prim.generate_glue
    end
  end

  f.puts "executor Primitives::resolve_primitive(STATE, Symbol* name) {"

  names.sort.each do |name|
    f.puts <<-EOF
  if(name == state->symbol("#{name}")) {
    return &Primitives::#{name};
  }

    EOF
  end

  f.puts <<-EOF
  if(!G(current_task)->probe()->nil_p()) {
    G(current_task)->probe()->missing_primitive(state, name->c_str(state));
  }
return &Primitives::unknown_primitive;
// commented out while we have soft primitive failures
// throw std::runtime_error(msg.c_str());
}
  EOF
end
