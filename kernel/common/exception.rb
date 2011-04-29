class Exception

  attr_accessor :locations
  attr_accessor :parent

  def initialize(message = nil)
    @reason_message = message
    @locations = nil
    @backtrace = nil
    @custom_backtrace = nil
  end

  # This is here rather than in yaml.rb because it contains "private"
  # information, ie, the list of ivars. Putting it over in the yaml
  # sounce means it's easy to forget about.
  def to_yaml_properties
    list = super
    list.delete "@backtrace"
    list.delete "@custom_backtrace"
    return list
  end

  def message
    @reason_message
  end

  # Needed to properly implement #exception, which must clone and call
  # #initialize again, BUT not a subclasses initialize.
  alias_method :__initialize__, :initialize

  def backtrace
    return @custom_backtrace if @custom_backtrace

    if backtrace?
      awesome_backtrace.to_mri
    else
      nil
    end
  end

  # Indicates if the Exception has a backtrace set
  def backtrace?
    (@backtrace || @locations) ? true : false
  end

  def awesome_backtrace
    @backtrace ||= Rubinius::Backtrace.backtrace(@locations)
  end

  def render(header="An exception occurred", io=STDERR, color=true)
    io.puts header
    io.puts "    #{message} (#{self.class})"

    if @custom_backtrace
      io.puts "\nUser defined backtrace:"
      @custom_backtrace.each do |line|
        io.puts "    #{line}"
      end
    end

    io.puts "\nBacktrace:"
    io.puts awesome_backtrace.show("\n", color)

    extra = @parent
    while extra
      io.puts "\nCaused by: #{extra.message} (#{extra.class})"

      if @custom_backtrace
        io.puts "\nUser defined backtrace:"
        @custom_backtrace.each do |line|
          io.puts "    #{line}"
        end
      end

      io.puts "\nBacktrace:"
      io.puts extra.awesome_backtrace.show

      extra = extra.parent
    end

  end

  def set_backtrace(bt)
    if bt.kind_of? Rubinius::Backtrace
      @backtrace = bt
    else
      # See if we stashed a Backtrace object away, and use it.
      if hidden_bt = Rubinius::Backtrace.detect_backtrace(bt)
        @backtrace = hidden_bt
      else
        @custom_backtrace = bt
      end
    end
  end

  def set_context(ctx)
    if ctx.kind_of? Exception
      @parent = ctx
    else
      set_backtrace(ctx)
    end
  end

  def to_s
    @reason_message || self.class.to_s
  end

  # This is important, because I subclass can just override #to_s and calling
  # #message will call it. Using an alias doesn't achieve that.
  def message
    to_s
  end

  alias_method :to_str, :message

  def inspect
    "#<#{self.class.name}: #{self.to_s}>"
  end

  class << self
    alias_method :exception, :new
  end

  def exception(message=nil)
    if message
      unless message.equal? self
        # As strange as this might seem, this IS actually the protocol
        # that MRI implements for this. The explicit call to
        # Exception#initialize (via __initialize__) is exactly what MRI
        # does.
        e = clone
        e.__initialize__(message)
        return e
      end
    end

    self
  end

  def location
    [context.file.to_s, context.line]
  end
end

class PrimitiveFailure < Exception
end

class ScriptError < Exception
end

class StandardError < Exception
end

class SignalException < Exception
end

class NoMemoryError < Exception
end

class ZeroDivisionError < StandardError
end

class ArgumentError < StandardError
  def message
    return @reason_message if @reason_message
    if @method_name
      "method '#{@method_name}': given #{@given}, expected #{@expected}"
    else
      "given #{@given}, expected #{@expected}"
    end
  end
end

class IndexError < StandardError
end

class StopIteration < IndexError
end

class RangeError < StandardError
end

class FloatDomainError < RangeError
end

class LocalJumpError < StandardError
end

class NameError < StandardError
  attr_reader :name

  def initialize(*args)
    super(args.shift)
    @name = args.shift
  end
end

class NoMethodError < NameError
  attr_reader :name
  attr_reader :args

  def initialize(*arguments)
    super(arguments.shift)
    @name = arguments.shift
    @args = arguments.shift
  end
end

class RuntimeError < StandardError
end

class SecurityError < StandardError
end

class SystemStackError < StandardError
end

class ThreadError < StandardError
end

class FiberError < StandardError
end

class TypeError < StandardError
end

class FloatDomainError < RangeError
end

class RegexpError < StandardError
end

class LoadError < ScriptError
  class InvalidExtensionError < LoadError
  end

  class MRIExtensionError < InvalidExtensionError
  end
end

class NotImplementedError < ScriptError
end

class Interrupt < SignalException
end

class IOError < StandardError
end

class EOFError < IOError
end

class LocalJumpError < StandardError
end

class NotImplementedError < ScriptError
end

class SyntaxError < ScriptError
  attr_accessor :column
  attr_accessor :line
  attr_accessor :file
  attr_accessor :code

  def self.from(message, column, line, code, file)
    exc = new message
    exc.import_position column, line, code
    exc.file = file
    exc
  end

  def import_position(c,l, code)
    @column = c
    @line = l
    @code = code
  end

  def reason
    @reason_message
  end

  def message
    msg = super
    msg = "#{file}:#{@line}: #{msg}" if file && @line
    msg
  end
end

class SystemExit < Exception

  ##
  # Process exit status if this exception is raised

  attr_reader :status

  ##
  # Creates a SystemExit exception with optional status and message.  If the
  # status is omitted, Process::EXIT_SUCCESS is used.
  #--
  # *args is used to simulate optional prepended argument like MRI

  def initialize(first=nil, *args)
    if first.kind_of?(Fixnum)
      status = first
      super(*args)
    else
      status = Process::EXIT_SUCCESS
      super
    end

    @status = status
  end

end


class SystemCallError < StandardError

  attr_reader :errno

  def self.errno_error(message, errno)
    Ruby.primitive :exception_errno_error
    raise PrimitiveFailure, "SystemCallError.errno_error failed"
  end

  # We use .new here because when errno is set, we attempt to
  # lookup and return a subclass of SystemCallError, specificly,
  # one of the Errno subclasses.
  def self.new(message=undefined, errno=undefined)
    # This method is used 2 completely different ways. One is when it's called
    # on SystemCallError, in which case it tries to construct a Errno subclass
    # or makes a generic instead of itself.
    #
    # Otherwise it's called on a Errno subclass and just helps setup
    # a instance of the subclass
    if self.equal? SystemCallError
      if message.equal? undefined
        raise ArgumentError, "must supply at least a message/errno"
      end

      if errno.equal?(undefined)
        if message.kind_of?(Fixnum)
          if inst = SystemCallError.errno_error(nil, message)
            return inst
          else # It's some random errno
            errno = message
            message = nil
          end
        else
          errno = nil
        end
      else
        message = StringValue(message) if message

        if errno.kind_of? Fixnum
          if error = SystemCallError.errno_error(message, errno)
            return error
          end
        end
      end

      return super(message, errno)
    else
      unless errno.equal? undefined
        raise ArgumentError, "message is the only argument"
      end

      if message and !(message.equal?(undefined))
        message = StringValue(message)
      end

      if error = SystemCallError.errno_error(message, self::Errno)
        return error
      end

      raise TypeError, "invalid Errno subclass"
    end
  end

  # Must do this here because we have a unique new and otherwise .exception will
  # call Exception.new because of the alias in Exception.
  class << self
    alias_method :exception, :new
  end

  def initialize(message, errno)
    @errno = errno

    msg = "unknown error"
    msg << " - #{StringValue(message)}" if message
    super(msg)
  end
end

##
# Base class for various exceptions raised in the VM.

class Rubinius::VMException < Exception
end

##
# Raised in the VM when an assertion fails.

class Rubinius::AssertionError < Rubinius::VMException
end

##
# Raised in the VM when attempting to read/write outside
# the bounds of an object.

class Rubinius::ObjectBoundsExceededError < Rubinius::VMException
end

# Defined by the VM itself
class Rubinius::InvalidBytecode < Rubinius::Internal
  attr_reader :compiled_method
  attr_reader :ip

  def message
    if @compiled_method
      if @ip and @ip >= 0
        "#{super} - at #{@compiled_method.name}+#{@ip}"
      else
        "#{super} - method #{@compiled_method.name}"
      end
    else
      super
    end
  end
end
