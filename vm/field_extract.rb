class CPPPrimitive
  def initialize(name, type)
    @type = type
    @name = name
  end

  attr_accessor :type, :name
  attr_accessor :cpp_name, :return_type, :arg_types

  def generate_glue(klass="Primitives")
    str =  "OBJECT #{klass}::#{@name}(STATE, Message& msg) {\n"
    i = -1
    args = arg_types.map { |t| "as<#{t}>(msg.get_argument(#{i += 1}))" }.join(", ")
    str << "  return as<#{@type}>(msg.recv)->#{@cpp_name}(state, #{args});\n"
    str << "}\n"
    return str
  end
end

class CPPOverloadedPrimitive
  def initialize(prim)
    @name = prim.name
    @type = prim.type

    @kinds = [prim]
  end

  def add_kind(prim)
    if prim.arg_types.size != 1
      raise "Invalid overloaded primitive: Only 2nd arg can be overloaded."
    end
    @kinds << prim
  end

  def generate_glue(klass="Primitives")
    str =  "OBJECT #{klass}::#{@name}(STATE, Message& msg) {\n"
    @kinds.each do |prim|
      type = prim.arg_types.first
      str << "  if(#{type}* arg = try_as<#{type}>(msg.get_argument(0))) {\n"
      str << "    return as<#{@type}>(msg.recv)->#{@cpp_name}(state, arg);\n"
      str << "  }\n"
    end
    str << " throw new Assertion(\"unable to resolve primitive #{@name} types\");\n"
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

      unless cur.kind_of? CPPOverloadedPrimitive
        over = CPPOverloadedPrimitive.new(cur)
        cur = @primitives[name] = over
      end

      cur.add_kind prim
    else
      @primitives[name] = prim
    end
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
    return if out.strip.empty?

    str =  "void #{@name}::Info::set_field(STATE, OBJECT _t, size_t index, OBJECT val) {\n"
    str << "  #{@name}* target = as<#{@name}>(_t);\n"
    str << "  switch(index) {\n"

    str << out

    str << "  }\n}\n"
    str << "OBJECT #{@name}::Info::get_field(STATE, OBJECT _t, size_t index) {\n"
    str << "  #{@name}* target = as<#{@name}>(_t);\n"
    str << "  switch(index) {\n"

    str << generate_gets()

    str << "  }\n"
    str << "  throw std::runtime_error(\"Unable to access field\");\n"
    str << "}\n"
  end

  def generate_marks(cpp)
    if cpp.super
      str = generate_marks(cpp.super)
    else
      str = ""
    end

    cpp.fields.each do |name, type, idx|
      str << "  {\n"
      str << "    if(target->#{name}->reference_p()) {\n"
      str << "      OBJECT res = mark.call(target->#{name});\n"
      str << "      if(res) {\n"
      str << "        target->#{name} = as<#{type}>(res);\n"
      str << "        mark.just_set(target, res);\n"
      str << "      }\n"
      str << "    }\n"
      str << "  }\n"
    end

    return str
  end

  def generate_mark
    out = generate_sets()
    return if out.strip.empty?

    str =  "void #{@name}::Info::mark(OBJECT _t, ObjectMark& mark) {\n"
    str << "  #{@name}* target = as<#{@name}>(_t);\n"

    str << generate_marks(self)
    str << "}\n"
    return str
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
    @class_order << name
    @classes[name] = obj
  end

  def strip_and_map(type, map)
    if type[-1] == ?*
      type = type[0..-2]
    end

    return map[type]
  end

  def parse_stream(f)
    while l = f.gets
      if m = /class ([^\s]+)\s*:\s*public\s+([^\s]+) \{/.match(l)
        if m[2] != "BuiltinType"
          if sup = @classes[m[2]]
            cpp = CPPClass.new(m[1])
            cpp.super = sup
          else
            # puts "Skipping #{m[1]}"
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
          next if /static/.match(l)
          break if /\};/.match(l)
          break if /^\s*class/.match(l)

          if m = %r!^\s*([\w\d_]+)\*?\s+\*?([\w\d_]+)\s*;\s*//\s*slot(.*)!.match(l)
            type = m[1]
            name = m[2]

            if name[0] == ?*
              name = name[1..-1]
            end

            if type[-1] == ?*
              type = type[0..-2]
            end

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
            cpp.add_field idx, name, mapped, flag
            idx += 1
          end

          if m = %r%^\s*//\s+Ruby.primitive(!)?\s+:(.*)\s*$%.match(l)
            overload = m[1] == "!"
            prim = m[2]
            prototype = f.gets

            m = %r!\s*([\w_]+)\s+([\w_]+)\((.*)\)!.match(prototype)
            args = m[3].split(/\s*,\s*/)
            if args.shift != "STATE"
              raise "Invalid primitive #{prim}, STATE is not first argument"
            end

            arg_types = args.map { |a| strip_and_map(a.split(/\s+/, 2).first, @type_map) }
            cpp.add_primitive prim, m[2], @type_map[m[1]], arg_types, overload

          end
        end

        cpp.num_fields = idx + 1
      end
    end
  end
end

parser = CPPParser.new

parser.parse_stream ARGF

File.open("gen/simple_field.rb", "w") do |f|
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

File.open("gen/typechecks.gen.cpp", "w") do |f|
  f.puts "void TypeInfo::init(STATE) {"
  parser.classes.each do |n, cpp|
    f.puts "  {"
    f.puts "    TypeInfo *ti = new #{n}::Info(#{n}::type);"
    cpp.all_fields.each do |name, idx|
      f.puts "    ti->slots[state->symbol(\"@#{name}\")->index()] = #{idx};"
    end
    f.puts "    state->add_type_info(ti);"
    f.puts "  }"
  end
  f.puts "}"

  parser.classes.each do |n, cpp|
    if tc = cpp.generate_typechecks
      f.puts tc
    end

    if mc = cpp.generate_mark
      f.puts mc
    end
  end

end

File.open("gen/primitives_glue.gen.cpp", "w") do |f|
  parser.classes.each do |n, cpp|
    cpp.primitives.each do |pn, prim|
      f.puts prim.generate_glue
    end
  end
end

