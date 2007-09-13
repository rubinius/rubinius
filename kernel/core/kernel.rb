module Kernel

  # Conversion methods
  def coerce_to(type, method, do_raise=true)
    if self.respond_to?(method)
      result = self.__send__(method)
      raise TypeError, "#{self.class}##{method} should return #{type}" unless result.is_a?(type)
      return result
    elsif do_raise
      raise TypeError, "can't convert #{self.class} into #{type}"
    end
  end
  
  def Float(obj)
    raise TypeError, "can't convert nil into Float" if obj.nil?
    obj.coerce_to(Float, :to_f)
  end
  
  def Integer(obj)
    result = obj.coerce_to(Integer, :to_int, false)
    result = obj.coerce_to(Integer, :to_i) if result.nil?
    return result
  end

  def Array(obj)
    result = obj.coerce_to(Array, :to_ary, false)
    result = obj.coerce_to(Array, :to_a, false) if result.nil?
    result = [obj] if result.nil?
    return result
  end

  def String(obj)
    obj.coerce_to(String, :to_s)
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
  
  alias_method :fail, :raise
  
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
  alias_method :format, :sprintf
  
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
  
