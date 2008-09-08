
$TESTING = false unless defined? $TESTING

require 'sexp'

class Object

  ##
  # deep_clone is the usual Marshalling hack to make a deep copy.
  # It is rather slow, so use it sparingly. Helps with debugging
  # SexpProcessors since you usually shift off sexps.

  def deep_clone
    Marshal.load(Marshal.dump(self))
  end
end

##
# SexpProcessor base exception class.

class SexpProcessorError < StandardError; end

##
# Raised by SexpProcessor if it sees a node type listed in its
# unsupported list.

class UnsupportedNodeError < SexpProcessorError; end

##
# Raised by SexpProcessor if it is in strict mode and sees a node for
# which there is no processor available.

class UnknownNodeError < SexpProcessorError; end

##
# Raised by SexpProcessor if a processor did not process every node in
# a sexp and @require_empty is true.

class NotEmptyError < SexpProcessorError; end

##
# Raised if assert_type encounters an unexpected sexp type.

class SexpTypeError < SexpProcessorError; end

##
# SexpProcessor provides a uniform interface to process Sexps.
#
# In order to create your own SexpProcessor subclass you'll need
# to call super in the initialize method, then set any of the
# Sexp flags you want to be different from the defaults.
#
# SexpProcessor uses a Sexp's type to determine which process method
# to call in the subclass.  For Sexp <code>s(:lit, 1)</code>
# SexpProcessor will call #process_lit, if it is defined.
#
# You can also specify a default method to call for any Sexp types
# without a process_<type> method or use the default processor provided to
# skip over them.
#
# Here is a simple example:
#
#   class MyProcessor < SexpProcessor
#     def initialize
#       super
#       self.strict = false
#     end
#
#     def process_lit(exp)
#       val = exp.shift
#       return val
#     end
#   end

class SexpProcessor

  ##
  # Automatically shifts off the Sexp type before handing the
  # Sexp to process_<type>

  attr_accessor :auto_shift_type

  ##
  # Return a stack of contexts. Most recent node is first.

  attr_reader :context

  ##
  # A Hash of Sexp types and Regexp.
  #
  # Print a debug message if the Sexp type matches the Hash key
  # and the Sexp's #inspect output matches the Regexp.

  attr_accessor :debug

  ##
  # A default method to call if a process_<type> method is not found
  # for the Sexp type.

  attr_accessor :default_method

  ##
  # Expected result class

  attr_accessor :expected

  ##
  # Raise an exception if the Sexp is not empty after processing

  attr_accessor :require_empty

  ##
  # Raise an exception if no process_<type> method is found for a Sexp.

  attr_accessor :strict

  ##
  # An array that specifies node types that are unsupported by this
  # processor. SexpProcessor will raise UnsupportedNodeError if you try
  # to process one of those node types.

  attr_accessor :unsupported

  ##
  # Emit a warning when the method in #default_method is called.

  attr_accessor :warn_on_default

  ##
  # Creates a new SexpProcessor.  Use super to invoke this
  # initializer from SexpProcessor subclasses, then use the
  # attributes above to customize the functionality of the
  # SexpProcessor

  def initialize
    @default_method = nil
    @warn_on_default = true
    @auto_shift_type = false
    @strict = false
    @unsupported = [:alloca, :cfunc, :cref, :ifunc, :last, :memo, :newline, :opt_n, :method] # internal nodes that you can't get to
    @unsupported_checked = false
    @debug = {}
    @expected = Sexp
    @require_empty = true
    @exceptions = {}

    # we do this on an instance basis so we can subclass it for
    # different processors.
    @processors = {}
    @rewriters  = {}
    @context = []

    public_methods.each do |name|
      case name
      when /^process_(.*)/ then
        @processors[$1.intern] = name.intern
      when /^rewrite_(.*)/ then
        @rewriters[$1.intern]  = name.intern
      end
    end
  end

  def assert_empty(meth, exp, exp_orig)
    unless exp.empty? then
      msg = "exp not empty after #{self.class}.#{meth} on #{exp.inspect}"
      msg += " from #{exp_orig.inspect}" if $DEBUG
      raise NotEmptyError, msg
    end
  end

  def rewrite(exp)
    type = exp.first

    self.context.unshift type

    exp.map! { |sub| Array === sub ? rewrite(sub) : sub }

    self.context.shift

    begin
      meth = @rewriters[type]
      exp  = self.send(meth, exp) if meth
      break unless Sexp === exp
      old_type, type = type, exp.first
    end until old_type == type

    exp
  end

  ##
  # Default Sexp processor.  Invokes process_<type> methods matching
  # the Sexp type given.  Performs additional checks as specified by
  # the initializer.

  def process(exp)
    return nil if exp.nil?
    exp = self.rewrite(exp) if self.context.empty?

    unless @unsupported_checked then
      m = public_methods.grep(/^process_/) { |o| o.to_s.sub(/^process_/, '').intern }
      supported = m - (m - @unsupported)

      raise UnsupportedNodeError, "#{supported.inspect} shouldn't be in @unsupported" unless supported.empty?

      @unsupported_checked = true
    end

    result = self.expected.new

    type = exp.first
    raise "type should be a Symbol, not: #{exp.first.inspect}" unless
      Symbol === type

    self.context.unshift type

    if @debug.has_key? type then
      str = exp.inspect
      puts "// DEBUG: #{str}" if str =~ @debug[type]
    end

    exp_orig = nil
    exp_orig = exp.deep_clone if $DEBUG or
      @debug.has_key? type or @exceptions.has_key?(type)

    raise UnsupportedNodeError, "'#{type}' is not a supported node type" if @unsupported.include? type

    if @debug.has_key? type then
      str = exp.inspect
      puts "// DEBUG (rewritten): #{str}" if str =~ @debug[type]
    end

    # now do a pass with the real processor (or generic)
    meth = @processors[type] || @default_method
    if meth then

      if @warn_on_default and meth == @default_method then
        $stderr.puts "WARNING: Using default method #{meth} for #{type}"
      end

      exp.shift if @auto_shift_type and meth != @default_method

      result = error_handler(type, exp_orig) do
        self.send(meth, exp)
      end

      raise SexpTypeError, "Result must be a #{@expected}, was #{result.class}:#{result.inspect}" unless @expected === result

      self.assert_empty(meth, exp, exp_orig) if @require_empty
    else
      unless @strict then
        until exp.empty? do
          sub_exp = exp.shift
          sub_result = nil
          if Array === sub_exp then
            sub_result = error_handler(type, exp_orig) do
              process(sub_exp)
            end
            raise "Result is a bad type" unless Array === sub_exp
            raise "Result does not have a type in front: #{sub_exp.inspect}" unless Symbol === sub_exp.first unless sub_exp.empty?
          else
            sub_result = sub_exp
          end
          result << sub_result
        end

        # NOTE: this is costly, but we are in the generic processor
        # so we shouldn't hit it too much with RubyToC stuff at least.
        #if Sexp === exp and not exp.sexp_type.nil? then
        begin
          result.sexp_type = exp.sexp_type
        rescue Exception
          # nothing to do, on purpose
        end
      else
        msg = "Bug! Unknown node-type #{type.inspect} to #{self.class}"
        msg += " in #{exp_orig.inspect} from #{caller.inspect}" if $DEBUG
        raise UnknownNodeError, msg
      end
    end

    self.context.shift
    result
  end

  def generate # :nodoc:
    raise NotImplementedError, "not implemented yet"
  end

  ##
  # Raises unless the Sexp type for +list+ matches +typ+

  def assert_type(list, typ)
    raise SexpTypeError, "Expected type #{typ.inspect} in #{list.inspect}" if
      not Array === list or list.first != typ
  end

  def error_handler(type, exp=nil) # :nodoc:
    begin
      return yield
    rescue StandardError => err
      if @exceptions.has_key? type then
        return @exceptions[type].call(self, exp, err)
      else
        $stderr.puts "#{err.class} Exception thrown while processing #{type} for sexp #{exp.inspect} #{caller.inspect}" if $DEBUG
        raise
      end
    end
  end
  private :error_handler

  ##
  # Registers an error handler for +node+

  def on_error_in(node_type, &block)
    @exceptions[node_type] = block
  end

  ##
  # A fairly generic processor for a dummy node. Dummy nodes are used
  # when your processor is doing a complicated rewrite that replaces
  # the current sexp with multiple sexps.
  #
  # Bogus Example:
  #
  #   def process_something(exp)
  #     return s(:dummy, process(exp), s(:extra, 42))
  #   end

  def process_dummy(exp)
    result = @expected.new(:dummy) rescue @expected.new
    until exp.empty? do
      result << self.process(exp.shift)
    end
    result
  end
end

