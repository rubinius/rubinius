require 'compiler'
require 'generate'
require 'bytecode'
require 'text'


class Compiler2::Node
  def to_dot(output, ds)
    i = ds.next
    output << %Q!node#{i} [shape=box, label="#{dot_name}", fontname="Monaco"];\n!
    orig = i
    instance_variables.each do |name|
      obj = instance_variable_get(name)
      n = name.to_s[::Range.new(1,-1)]
      
      if obj.kind_of? Compiler2::Node
        j = obj.to_dot(output, ds)
        output << "node#{orig} -> node#{j} [label=\" #{n} \", fontsize=8.0, fontname=\"Monaco\"];\n"
      elsif obj.kind_of? Array
        j = 0
        obj.each do |x|
          if x.kind_of? Compiler2::Node
            k = x.to_dot(output, ds)
            output << "node#{orig} -> node#{k} [label=\"#{n}:#{j}\", fontsize=8.0, fontname=\"Monaco\"];\n"
          end
          j += 1          
        end
      end
    end
    
    orig
  end
  
  def dot_name
    self.class.name.split(':').last
  end
  
  class FCall
    def dot_name
      "fcall:#{@method}"
    end
  end
  
  class Call
    def dot_name
      "call:#{@method}" 
    end
  end
  
  class LocalAccess
    def dot_name
      "lvar:#{@name}"
    end
  end
  
  class LocalAssignment
    def dot_name
      "lasgn:#{@name}"
    end
  end
  
  class Newline
    def dot_name
      "line:#{@line}"
    end
  end
  
  class StringLiteral
    def dot_name
      r = ::Range.new(1,-2)
      "'#{@string.dump[r]}'"
    end
  end
  
  class NumberLiteral
    def dot_name
      "#{@value}"
    end
  end
  
  class ArrayLiteral
    def dot_name
      "[...]"
    end
  end
  
  class HashLiteral
    def dot_name
      "{...}"
    end
  end
  
  class IVar
    def dot_name
      "ivar:#{@name}"
    end
  end
  
  class IAssign
    def dot_name
      "iasgn:#{@name}"
    end
  end
  
  class Literal
    def dot_name
      if @value.kind_of? Symbol
        ":#{@value.to_s.dump[::Range.new(1,-2)]}"
      else
        "#{@value.inspect.dump[::Range.new(1,-2)]}"
      end
    end
  end
  
  class Nil
    def dot_name
      "nil"
    end
  end
  
  class True
    def dot_name
      "true"
    end
  end
  
  class False
    def dot_name
      "false"
    end
  end
end

class DotState
  def initialize
    @node = 0
  end
  
  def next
    @node += 1
  end
end

file = ARGV.shift


gen = Compiler2::Generator

c = Compiler2.new(gen)


if file
  x = File.to_sexp(file)
  n = c.into_script(x)
else
  code = Readline.readline("> ")
  n = c.into_script(code.to_sexp)
end

output = "digraph G {\n"

ds = DotState.new

n.to_dot(output, ds)

output << "}\n"

puts output
