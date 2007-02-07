require 'rubygems'
require 'spec'

module PrimitiveSpecHelper
  def run_code(code)
  `shotgun/rubinius -e '#{code}'`.strip
  end

  def run_asm(asm)
    eval(run_code("p Ruby.asm(\"#{asm}\")"))
  end

  # have to do this the long way in case literals need to be pushed
  def run_primitive(name, *args)
    raise "There must be at least one argument passed to a primitive (self)" if args.empty?

    declarations = ""
    instructions = ""
    num_args = (args.length - 1)

    ('a'..'zzz').each do |id|
      break unless _value = args.pop
      declarations << "#{id} = #{_value.inspect}; "
      instructions << "push #{id}\n"
    end

    code = "begin; #{declarations} p Ruby.asm(\"" +
           "#{instructions}send_primitive #{name} #{num_args}" +
           "\"); " +
           "rescue Exception => e; p e.class.to_s; end"
    begin
      eval run_code(code)
    rescue SyntaxError
      puts "!! The following code caused shotgun to crash:"
      p code
      puts run_code(code)
      abort
    end
  end
end

# stub so we can be declarative in the specs
class PrimitiveFailure < Exception; end

class Object
  # all this does is assumes that the string rep of self _is_ the exception name
  def should_raise(exc)
    self.to_s.should == exc.to_s
  end
end

