# -*- encoding: us-ascii -*-

module Kernel

  def raise(exc=undefined, msg=undefined, ctx=nil)
    skip = false
    if undefined.equal? exc
      exc = $!
      if exc
        skip = true
      else
        exc = RuntimeError.new("No current exception")
      end
    elsif exc.respond_to? :exception
      if undefined.equal? msg
        exc = exc.exception
      else
        exc = exc.exception msg
      end
      raise ::TypeError, 'exception class/object expected' unless exc.kind_of?(::Exception)
    elsif exc.kind_of? String
      exc = ::RuntimeError.exception exc
    else
      raise ::TypeError, 'exception class/object expected'
    end

    unless skip
      exc.set_context ctx if ctx
      exc.capture_backtrace!(2) unless exc.backtrace?
    end

    if $DEBUG and $VERBOSE != nil
      if loc = exc.locations
        pos = loc[1].position
      else
        pos = Rubinius::VM.backtrace(1)[0].position
      end

      STDERR.puts "Exception: `#{exc.class}' #{pos} - #{exc.message}"
    end

    Rubinius.raise_exception exc
  end
  module_function :raise

  alias_method :fail, :raise
  module_function :fail

  def method_missing(meth, *args)
    cls = NoMethodError

    case Rubinius.method_missing_reason
    when :private
      msg = "private method `#{meth}' called"
    when :protected
      msg = "protected method `#{meth}' called"
    when :super
      msg = "no superclass method `#{meth}'"
    when :vcall
      msg = "undefined local variable or method `#{meth}'"
      cls = NameError
    else
      msg = "undefined method `#{meth}'"
    end

    object_class = Rubinius::Type.object_class(self)

    if Rubinius::Type.object_kind_of?(self, Module)
      msg << " on #{self} (#{object_class})"

    # A separate case for nil, because people like to patch methods to
    # nil, so we can't call methods on it reliably.
    elsif nil.equal?(self)
      msg << " on nil:NilClass."
    elsif ImmediateValue === self
      msg << " on #{self}:#{object_class}."
    else
      msg << " on an instance of #{object_class}."
    end

    Kernel.raise cls.new(msg, meth, args)
  end

  private :method_missing

  # Add in $! in as a hook, to just do $!. This is for accesses to $!
  # that the compiler can't see.
  Rubinius::Globals.set_hook(:$!) { $! }

  get = proc do
    # We raise an exception here because Regexp.last_match won't work
    raise TypeError, "Unable to handle $~ in this context"
  end
  set = proc do |key, val|
    if val.nil? || val.kind_of?(MatchData)
      Rubinius.invoke_primitive :regexp_set_last_match, val
    else
      raise TypeError, "Cannot assign #{val.class}, expexted nil or instance MatchData."
    end
  end
  Rubinius::Globals.set_hook(:$~, get, set)

  Rubinius::Globals.set_hook(:$*) { ARGV }

  Rubinius::Globals.set_hook(:$@) { $! ? $!.backtrace : nil }

  Rubinius::Globals.set_hook(:$$) { Process.pid }

  prc = proc { ARGF.filename }
  Rubinius::Globals.set_hook(:$FILENAME, prc, :raise_readonly)

  write_filter = proc do |key, io|
    unless io.respond_to? :write
      raise ::TypeError, "#{key} must have write method, #{io.class} given"
    end
    io
  end

  get = proc { |key| Thread.current[:$_] }
  set = proc { |key, val| Thread.current[:$_] = val }
  Rubinius::Globals.set_hook(:$_, get, set)

  Rubinius::Globals.add_alias :$stdout, :$>
  Rubinius::Globals.set_filter(:$stdout, write_filter)
  Rubinius::Globals.set_filter(:$stderr, write_filter)

  # Proper kcode support
  get = proc { |key| Rubinius.kcode.to_s }
  set = proc { |key, val| Rubinius.kcode = val }
  Rubinius::Globals.set_hook(:$KCODE, get, set)

  set = proc do |key, val|
    val = Rubinius::Type.coerce_to val, String, :to_str
    Rubinius.invoke_primitive :vm_set_process_title, val
  end
  Rubinius::Globals.set_hook(:$0, :[], set)

  set = proc { |key, val| STDERR.puts("WARNING: $SAFE is not supported on Rubinius."); val }
  Rubinius::Globals.set_hook(:$SAFE, :[], set)

  # Alias $0 $PROGRAM_NAME
  Rubinius::Globals.add_alias(:$0, :$PROGRAM_NAME)

  Rubinius::Globals.read_only :$:, :$LOAD_PATH, :$-I
  Rubinius::Globals.read_only :$", :$LOADED_FEATURES
  Rubinius::Globals.read_only :$<

  Rubinius::Globals[:$-a] = false
  Rubinius::Globals[:$-l] = false
  Rubinius::Globals[:$-p] = false
  Rubinius::Globals.read_only :$-a, :$-l, :$-p

  Rubinius::Globals.add_alias :$DEBUG,   :$-d
  Rubinius::Globals.add_alias :$VERBOSE, :$-v
  Rubinius::Globals.add_alias :$VERBOSE, :$-w

  set_string = proc do |key, value|
    unless value.nil? or value.kind_of? String
      raise TypeError, "value of #{key} must be a String"
    end

    Rubinius::Globals.set! key, value
  end

  Rubinius::Globals.set_filter :$/, set_string
  Rubinius::Globals.add_alias :$/, :$-0

  Rubinius::Globals.set_filter :$,, set_string
end
