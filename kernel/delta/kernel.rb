module Kernel

  ##
  #--
  # HACK todo handle cascading raises (ie, TypeError raise
  # raising forever blows)
  #++

  def raise(exc=undefined, msg=undefined, ctx=nil)
    skip = false
    if exc.equal? undefined
      exc = $!
      if exc
        skip = true
      else
        exc = RuntimeError.new("No current exception")
      end
    elsif exc.respond_to? :exception
      if msg.equal? undefined
        exc = exc.exception
      else
        exc = exc.exception msg
      end
      raise ::TypeError, 'exception class/object expected' unless exc.kind_of?(::Exception)
    elsif exc.kind_of? String or !exc
      exc = ::RuntimeError.exception exc
    else
      raise ::TypeError, 'exception class/object expected'
    end

    unless skip
      exc.set_context ctx if ctx
      unless exc.locations
        exc.locations = Rubinius::VM.backtrace 1
      end
    end

    if $DEBUG and $VERBOSE != nil
      STDERR.puts "Exception: `#{exc.class}' #{exc.locations[1].position} - #{exc.message}"
    end

    Rubinius.raise_exception exc
  end
  module_function :raise

  alias_method :fail, :raise
  module_function :fail

  def method_missing(meth, *args)
    case Rubinius.method_missing_reason
    when :private
      msg = "private method '#{meth}'"
    when :protected
      msg = "protected method '#{meth}'"
    when :super
      msg = "no superclass method '#{meth}'"
    else
      msg = "no method '#{meth}'"
    end

    if __kind_of__(Module)
      msg << " on #{self} (#{self.class})"
    else
      msg << " on an instance of #{self.class}."
    end

    Kernel.raise NoMethodError.new(msg, meth, args)
  end

  private :method_missing

  # Add in $! in as a hook, to just do $!. This is for accesses to $!
  # that the compiler can't see.
  get = proc { $! }
  Rubinius::Globals.set_hook(:$!, get, nil)

  # Same as $!, for any accesses we might miss.
  # HACK. I doubt this is correct, because of how it will be called.
  get = proc { Regex.last_match }
  Rubinius::Globals.set_hook(:$~, get, nil)

  get = proc { ARGV }
  Rubinius::Globals.set_hook(:$*, get, nil)

  get = proc { $! ? $!.backtrace : nil }
  Rubinius::Globals.set_hook(:$@, get, nil)

  get = proc { Process.pid }
  Rubinius::Globals.set_hook(:$$, get, nil)

  get = proc { ::STDOUT }
  set = proc do |io, key|
    unless io.respond_to? :write
      raise ::TypeError, "#{key} must have write method, #{io.class} given"
    end
    ::STDOUT = io
  end
  Rubinius::Globals.set_hook(:$>, get, set)
  Rubinius::Globals.set_hook(:$stdout, get, set)
  Rubinius::Globals.set_hook(:$defout, get, set)

  get = proc { ::STDIN }
  set = proc { |io| ::STDIN = io }
  Rubinius::Globals.set_hook(:$stdin, get, set)

  get = proc { ::STDERR }
  set = proc do |io, key|
    unless io.respond_to? :write
      raise ::TypeError, "#{key} must have write method, #{io.class} given"
    end
    ::STDERR = io
  end
  Rubinius::Globals.set_hook(:$stderr, get, set)

  # Implements rb_path2name. Based on code from wycats
  def const_lookup(name)
    names = name.split '::'
    names.shift if names.first.empty?
    names.inject(Object) do |m, n|
      m.const_defined?(n) ? m.const_get(n) : m.const_missing(n)
    end
  end

  private :const_lookup
end
