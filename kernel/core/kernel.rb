module Kernel

  # Conversion methods
  def Float(obj)
    obj = obj.to_f
    if Float === obj
      obj
    else
      raise TypeError, "#{obj.class}#to_f should return Float"
    end
  rescue NoMethodError
    Integer(obj).to_f
  rescue TypeError
    raise TypeError, "can't convert #{obj.class} into Float"
  end
  
  def Integer(obj)
    obj = obj.to_i
    if Integer === obj
      obj
    else
      raise TypeError, "#{obj.class}#to_i should return Integer"
    end
  rescue NoMethodError
    raise TypeError, "can't convert #{obj.class} into Integer"
  end

  def Array(obj)
    ret = obj.respond_to?(:to_ary) ? obj.to_ary : obj.to_a
    if Array === ret
      ret
    else
      raise TypeError, "#{obj.class}#to_a did not return Array"
    end
  rescue NoMethodError
    raise TypeError, "can't convert #{obj.class} into Array"
  end

  def String(obj)
    obj.to_s
  end
  
  # Reporting methods
  
  def raise(exc=$!, msg=nil, trace=nil)
    if exc.respond_to? :exception
      exc = exc.exception msg
      raise TypeError, 'exception class/object expected' unless Exception === exc
      exc.set_backtrace trace if trace
    elsif exc.kind_of? String or !exc
      exc = RuntimeError.exception exc
    else
      raise TypeError, 'exception class/object expected'
    end

    if $DEBUG
      STDERR.puts "Exception: #{exc.message} (#{exc.class})"
    end

    exc.set_backtrace MethodContext.current.sender unless exc.backtrace
    Ruby.asm "#local exc\nraise_exc"
  end
  
  alias fail raise
  
  def warn(warning)
    unless $VERBOSE.nil?
      $stderr.write warning
      $stderr.write "\n"
    end
    nil
  end

  def exit(code=0)
    raise SystemExit.new(code)
  end

  def exit!(code=0)
    Process.exit(code)
  end

  def abort(msg=nil)
    $stderr.puts(msg) if(msg)
    exit 1
  end

  # Display methods
  def printf(*args)
    if args[0].class == IO
      args[0].write(Sprintf::Parser.format(args[1], args[2..-1]))
    elsif args[0].class == String
      $stdout.write(Sprintf::Parser.format(args[0], args[1..-1]))
    else
      raise TypeError, "The first arg to printf should be an IO or a String"
    end
    nil
  end

  def sprintf(str, *args)
    Sprintf::Parser.format(str, args)
  end
  alias :format :sprintf
  
  def puts(*a)
    a = [""] if a.empty?
    a.each do |obj| 
      str = obj.to_s
      if str[-1] == 10
        $CONSOLE.print str
      else
        $CONSOLE.puts str
      end
    end
    nil
  end
  
  def p(*a)
    a = [nil] if a.empty?
    a.each { |obj| $CONSOLE.puts obj.inspect }
    nil
  end

  def print(*args)
    args.each do |obj|
      $CONSOLE.write obj.to_s
    end
    nil
  end
    
  # NOTE - this isn't quite MRI compatible, we don't store return the previous
  # seed value from srand and we don't seed the RNG by default with a combination
  # of time, pid and sequence number
  def srand(seed)
    Platform::POSIX.srand(seed.to_i)
  end

  def rand(max=nil)
    max = max.to_i.abs
    x = Platform::POSIX.rand
    # scale result of rand to a domain between 0 and max
    if max.zero?
      x / 0x7fffffff.to_f
    else
      if max < 0x7fffffff
        x / (0x7fffffff / max)
      else
         x * (max / 0x7fffffff)
      end
    end
  end
  
  def eval(string, binding = self, filename = '(eval)', lineno = 1)
    compiled_method = string.compile_as_method(filename, lineno)
    method = Method.new(binding, nil, compiled_method)
    method.call
  end

  def caller
    ret = []
    # Since calling this will change the sender, we have to go up twice
    ctx = MethodContext.current.sender.sender
    until ctx.nil?
      if ctx.method.name == :__script__ # This is where MRI stops
        ret << "#{ctx.file}:#{ctx.line}"
        break
      else
        ret << "#{ctx.file}:#{ctx.line}:in `#{ctx.method.name}'"
      end
      ctx = ctx.sender
    end
    ret
  end
  
  def at_exit(&block)
    Rubinius::AtExit.unshift(block)
  end
  
  def self.after_loaded
    # This nukes the bootstrap raise so the Kernel one is
    # used.
    Object.method_table[:raise] = nil
  end

  # TODO - Improve this check for metaclass support
  def __verify_metaclass__
    if Fixnum === self or Symbol === self
      raise TypeError, "no virtual class for #{self.class}"
    end
  end
end

Object.include Kernel

class SystemExit < Exception
  def initialize(code)
    @code = code
  end
  
  attr_reader :code
  
  def message
    "System is exiting with code '#{code}'"
  end
end
  
