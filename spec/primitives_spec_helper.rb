require 'rubygems'
require 'spec'

module PrimitiveSpecHelper
  def run_asm(asm)
    try_code "p Ruby.asm(\"#{asm}\")"
  end

  # have to do this the long way in case literals need to be pushed
  def run_primitive(name, *args)
    raise "There must be at least one argument passed to a primitive (self)" if args.empty?

    declarations = ""
    instructions = ""
    num_args = (args.length - 1)

    ('a'..'zzz').each do |id|
      _value = args.pop
      break if _value.nil?
      declarations << "#{id} = #{_value.inspect}; "
      instructions << "push #{id}\n"
    end

    code = "#{declarations} begin; p Ruby.asm(\"" +
           "#{instructions}send_primitive #{name} #{num_args}" +
           "\"); " +
           "rescue Exception => e; p e.class.to_s; end"
    try_code(code)
  end

  # shows information about eval failure if needed
  def try_code(code)
    output = run_code(code)
    output.gsub!("NaN", "0.0 / 0.0") # transfer NaN's
    begin
      eval(output)
    rescue Exception => e
      puts "!#{e.class}! The following code caused shotgun to crash:"
      p code
      puts output
      abort
    end
  end

  def run_code(code)
    `shotgun/rubinius -e '#{code}'`.strip
  end
end

# stub so we can be declarative in the specs
class PrimitiveFailure < Exception; end

class Object
  # all this does is assumes that the string rep of self _is_ the exception name
  def should_raise(exc)
    self.to_s.should == exc.to_s
  end

  def prim
    @proxy ||= PrimitiveProxy.new(self)
  end
end

class PrimitiveProxy
  include PrimitiveSpecHelper
  def initialize(target)
    @self = target
  end

  def method_missing(prim, *args, &block)
    run_primitive(prim, @self, *args)
  end
end

# this is good brix, thx for the trick
class Spec::Runner::Context
  def before_context_eval
    @context_eval_module.include PrimitiveSpecHelper
  end
end

