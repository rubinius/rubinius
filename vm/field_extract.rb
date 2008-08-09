class BasicPrimitive
  attr_accessor :pass_state
end

class CPPPrimitive < BasicPrimitive
  def initialize(name, type)
    @type = type
    @name = name
  end

  attr_accessor :type, :name
  attr_accessor :cpp_name, :return_type, :arg_types

  def generate_glue(klass="Primitives")
    str =  "bool #{klass}::#{@name}(STATE, VMExecutable* exec, Task* task, Message& msg) {\n"
    i = -1
    if arg_types.empty?
      args = []
    else
      args = arg_types.map { |t| "as<#{t}>(msg.get_argument(#{i += 1}))" }
    end

    str << "  OBJECT ret;\n"
    str << "  try {\n"
    args.unshift "state" if @pass_state
    str << "    ret = as<#{@type}>(msg.recv)->#{@cpp_name}(#{args.join(', ')});\n"
    str << "  } catch(PrimitiveFailed& e) {\n"
    str << "    abort(); // FIXME\n"
    str << "    return true;\n"
    str << "  }\n"
    str << "  task->primitive_return(ret, msg);\n"
    str << "  return false;\n"
    str << "}\n"
    return str
  end
end

class CPPStaticPrimitive < CPPPrimitive
  def generate_glue(klass="Primitives")
    str =  "bool #{klass}::#{@name}(STATE, VMExecutable* exec, Task* task, Message& msg) {\n"
    i = -1
    if arg_types.empty?
      args = []
    else
      args = arg_types.map { |t| "as<#{t}>(msg.get_argument(#{i += 1}))" }
    end

    str << "  OBJECT ret;\n"
    str << "  try {\n"
    args.unshift "state" if @pass_state
    str << "    ret = #{@type}::#{@cpp_name}(#{args.join(', ')});\n"
    str << "  } catch(PrimitiveFailed& e) {\n"
    str << "    abort(); // FIXME\n"
    str << "    return true;\n"
    str << "  }\n"
    str << "  task->primitive_return(ret, msg);\n"
    str << "  return false;\n"
    str << "}\n"
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

  def generate_glue(klass="Primitives")
    str =  "bool #{klass}::#{@name}(STATE, VMExecutable* exec, Task* task, Message& msg) {\n"
    str << "  OBJECT ret;\n"
    str << "  try {\n"

    @kinds.each do |prim|
      type = prim.arg_types.first
      str << "    if(#{type}* arg = try_as<#{type}>(msg.get_argument(0))) {\n"
      if @pass_state
        str << "      return as<#{@type}>(msg.recv)->#{@cpp_name}(state, arg);\n"
      else
        str << "      return as<#{@type}>(msg.recv)->#{@cpp_name}(arg);\n"
      end
      str << "    }\n"
    end
    str << "    else { throw new Assertion(\"unable to resolve primitive #{@name} types\"); }\n"

    str << "  } catch(PrimitiveFailed& e) {\n"
    str << "    abort(); // FIXME\n"
    str << "    return true;\n"
    str << "  }\n"
    str << "  task->primitive_return(ret, msg);\n"
    str << "  return false;\n"
    str << "}\n"
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
  end

  attr_accessor :name, :super, :fields, :primitives, :flags, :num_fields

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
      ary << [n, i + offset, flags]
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
    all_fields.each do |name, idx, flags|
      str << "  case #{idx}:\n"
      str << "    return target->#{name};\n"
    end

    return str
  end

  def generate_sets
    str = ""
    all_fields.each do |name, idx, flags|
      str << "  case #{idx}:\n"
      if flags[:readonly]
        str << "    throw new Assertion(\"#{name} is readonly\");\n"
      else
        str << "    SET(target, #{name}, val);\n"
      end
      str << "    return;\n"
    end

    return str
  end

  def generate_typechecks
    out = generate_sets()
    return '' if out.strip.empty?

    <<-EOF
void #{@name}::Info::set_field(STATE, OBJECT _t, size_t index, OBJECT val) {
  #{@name}* target = as<#{@name}>(_t);

  switch(index) {
#{out}  }
}

OBJECT #{@name}::Info::get_field(STATE, OBJECT _t, size_t index) {
  #{@name}* target = as<#{@name}>(_t);

  switch(index) {
#{generate_gets}  }

  throw std::runtime_error(\"Unable to access field\");
}

    EOF
  end

  def generate_marks(cpp)
    str = ''

    str << generate_marks(cpp.super) if cpp.super

    cpp.fields.each do |name, type, idx|
      str << <<-EOF
  {
    if(target->#{name}->reference_p()) {
      OBJECT res = mark.call(target->#{name});
      if(res) {
        target->#{name} = as<#{type}>(res);
        mark.just_set(target, res);
      }
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
void #{@name}::Info::mark(OBJECT _t, ObjectMark& mark) {
  #{@name}* target = as<#{@name}>(_t);

#{marks}
}

    EOF

    str
  end
end

class CPPParser

  attr_reader :classes, :class_order

  def initialize
    @classes = Hash.new
    @class_order = []

    @type_map = {
      "SYMBOL" => :Symbol,
      "InstructionSequence" =>   :InstructionSequence,
      "FIXNUM" => :Fixnum,
      "OBJECT" => :Object,
      "Object" => :Object,
      "Tuple"  => :Tuple,
      "MemoryPointer" => :MemoryPointer,
      "StaticScope" => :StaticScope,
      "INTEGER" => :Integer,
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
    while l = f.gets
      if m = /class ([^\s]+)\s*:\s*public\s+([^\s]+) \{/.match(l)
        if m[2] != "Object" and m[2] != "ObjectHeader"
          if sup = @classes[m[2]]
            cpp = CPPClass.new(m[1])
            cpp.super = sup
          else
            next
          end
        else
          cpp = CPPClass.new(m[1])
        end

        # puts "Seen #{m[1]}"

        add_class(cpp.name, cpp)

        @type_map[cpp.name] = cpp.name.to_sym

        next if /\};/.match(l)

        idx = 0
        while l = f.gets
          # break if /\};/.match(l)
          break if /^\s*class/.match(l)

          if m = %r!^\s*(\w+)\*?\s+\*?(\w+)\s*;\s*//\s*slot(.*)!.match(l)
            type = m[1]
            name = m[2]

            if mapped = @type_map[type]
              type = mapped
            else
              type = type.to_s
            end

            if m[3]
              flag = m[3].strip
            else
              flag = nil
            end

            cpp.add_field idx, name, type, flag
            idx += 1
          end

          if m = %r%^\s*//\s+Ruby.primitive(!)?\s+:(.*)\s*$%.match(l)
            overload = m[1] == "!"
            prim = m[2]
            prototype = f.gets

            m = %r!\s*(static\s+)?([\w\*]+)\s+([\w]+)\((.*)\)!.match(prototype)
            args = m[4].split(/\s*,\s*/)
            if args.first == "STATE"
              args.shift
              pass_state = true
            else
              pass_state = false
            end

            arg_types = args.map { |a| strip_and_map(a.split(/\s+/, 2).first, @type_map) }
            if m[1]
              if overload
                raise "Unable to overload static primitives."
              end
              obj = cpp.add_static_primitive prim, m[3], @type_map[m[2]], arg_types
            else
              obj = cpp.add_primitive prim, m[3], @type_map[m[2]], arg_types, overload
            end

            obj.pass_state = pass_state

          end
        end

        cpp.num_fields = idx + 1
      end
    end
  end
end

parser = CPPParser.new

includes = ARGV.map do |include|
  "#include \"#{include.sub(/^vm\//, '')}\""
end.join "\n"

parser.parse_stream ARGF

File.open 'vm/gen/includes.hpp', 'w' do |f|
  f << <<-EOF
#ifndef GEN_INCLUDES_HPP
#define GEN_INCLUDES_HPP

// DO NOT INCLUDE THIS IN SOMETHING THAT DOES NOT INCLUDE A GENERATED FILE

#{includes}

#endif
  EOF
end

File.open("vm/gen/simple_field.rb", "w") do |f|
  f.puts "# DO NOT EDIT!! Autogenerate by field_extract.rb"
  parser.class_order.each do |name|
    obj = parser.classes[name]

    if sup = obj.super and sup.name != "Class"
      f.puts "class #{name} < #{sup.name}"
    else
      f.puts "class #{name}"
    end

    f << "  Slots = ["
    f << obj.fields.map { |n,t,i| ":#{n}" }.join(", ")
    f.puts "]"

    f.puts "  def self.slots"
    if sup
      f.puts "    #{sup.name}.slots + Slots"
    else
      f.puts "    Slots"
    end
    f.puts "  end"
    f.puts ""

=begin
    obj.fields.each do |fname, ftype|
      f.puts "  attr_reader :#{fname}"
      if obj.readonly?(fname)
        f.puts "  # #{fname} is readonly"
      else
        f.puts "  def #{fname}=(obj)"
        f.puts "    unless obj.kind_of?(#{ftype})"
        f.puts "      raise TypeError, \"Only accepts a #{ftype}\""
        f.puts "    end"
        f.puts "    @#{fname} = obj"
        f.puts "  end"
      end
      f.puts ""
    end
=end
    f.puts "end"
    f.puts ""
  end
end

File.open("vm/gen/typechecks.gen.cpp", "w") do |f|
  f.puts "void TypeInfo::init(STATE) {"
  parser.classes.each do |n, cpp|
    next if n == "Object"

    f.puts "  {"
    f.puts "    TypeInfo *ti = new #{n}::Info(#{n}::type);"
    cpp.all_fields.each do |name, idx|
      f.puts "    ti->slots[state->symbol(\"@#{name}\")->index()] = #{idx};"
    end
    f.puts "    ti->type_name = std::string(\"#{n}\");"
    f.puts "    state->add_type_info(ti);"
    f.puts "  }"
    f.puts
  end
  f.puts "}"
  f.puts

  parser.classes.each do |n, cpp|
    f.puts cpp.generate_typechecks
    f.puts cpp.generate_mark
  end

end

File.open("vm/gen/primitives_declare.hpp", "w") do |f|
  parser.classes.each do |n, cpp|
    cpp.primitives.each do |pn, prim|
      f.puts "static bool #{pn}(STATE, VMExecutable* exec, Task* task, Message& msg);"
    end
  end
end

File.open("vm/gen/primitives_glue.gen.cpp", "w") do |f|
  names = []
  parser.classes.each do |n, cpp|
    cpp.primitives.each do |pn, prim|
      names << pn
      f.puts prim.generate_glue
    end
  end

  f.puts "executor Primitives::resolve_primitive(STATE, SYMBOL name) {"
  names.each do |name|
    f.puts "  if(name == state->symbol(\"#{name}\")) {"
    f.puts "    return &Primitives::#{name};"
    f.puts "  }"
  end
  f.puts "  std::string msg = std::string(\"Unable to resolve primitive: \") + (char*)*name->to_str(state);"
  f.puts "  std::cout << msg << std::endl;"
  f.puts "  return &Primitives::unknown_primitive;"
  # commented out while we have soft primitive failures
  #f.puts "  throw std::runtime_error(msg.c_str());"
  f.puts "}"
end

