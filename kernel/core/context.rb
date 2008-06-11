# depends on: class.rb

##
# Stores information about a running method:
#
# sender::          the MethodContext calling this one
# block::           block argument passed in
# CompiledMethod::  the method being run
# locals::          locals for the callee
# defining module:: which module (or class) the CompiledMethod was defined in
# receiver::        object this CompiledMethod was sent too.
# CPU registers::   VM state for running this CompiledMethod
#
# == Life of a MethodContext
#
# Given a CompiledMethod "m" defined on a module or class K, if the
# CompiledMethod has no primitive
#
# When calling a method, a MethodContext "mc" is created if the CompiledMethod
# representing the method either has no primitive, or the primitive failed.
#
# mc is filled in with various details from the CompiledMethod.  The VM state
# is then saved into the active MethodContext and the VM state from mc is
# coppied into the VM.  The VM will run the CompiledMethod's bytecode until
# encountering a return instruction (:ret).
#
# Upon encountering the return instruction, the VM pops off the top of the
# stack for use as the return value, restores the previously running
# MethodContext's VM state and pushes the return value back onto the stack.
#--
# Hey! Be careful with this! This is used by backtrace and if it doesn't work,
# you can get recursive exceptions being raised (THATS BAD, BTW).

class MethodContext

  attr_accessor :last_match
  alias_method :next_frame, :sender

  def from_eval?
    false
  end

  def position_info
    ret = []
    if self.method.name and self.method.name != :__script__
      ret << "#{self.file}:#{self.line}:in `#{self.method.name}'" 
    else
      ret << "#{self.file}:#{self.line}"
    end
    ret
  end
  ##
  # The Nth group of the last regexp match.

  def nth_ref(n)
    if lm = @last_match
      return lm[n]
    end

    return nil
  end

  ##
  # One of the special globals $&, $`, $' or $+.

  def back_ref(kind)
    if lm = @last_match
      res = case kind
      when :&
        lm[0]
      when :"`"
        lm.pre_match
      when :"'"
        lm.post_match
      when :+
        lm.captures.last
      end

      return res
    end

    return nil
  end

  def to_s
    "#<#{self.class}:0x#{self.object_id.to_s(16)} #{receiver}##{name} #{file}:#{line}>"
  end
  alias_method :inspect, :to_s
  # File in which associated method defined.
  def file
    return "(unknown)" unless self.method
    method.file
  end

  # See CompiledMethod#lines
  def lines
    return [] unless self.method
    method.lines
  end

  # Current line being executed by the VM.
  def line
    return 0 unless self.method
    # We subtract 1 because the ip is actually set to what it should do
    # next, not what it's currently doing (unless we are at the start of
    # a new context).
    if self.from_eval? and self.env.caller_env
      ip = self.env.caller_env.registration_ip - 1
    else
      ip = self.ip - 1
      ip = 0 if ip < 0
    end
    self.method.line_from_ip(ip)
  end

  ##
  # Copies context. If locals is true local variable values are also copied
  # into new context.

  def copy(locals=false)
    d = self.dup
    return d unless locals

    i = 0
    lc = self.locals
    tot = lc.fields
    nl = Tuple.new(tot)
    while i < tot
      nl.put i, lc.at(i)
      i += 1
    end

    # d.put 10, nl

    return d
  end

  ##
  # Place in the source that this method was created at.

  def location
    l = line()
    if l == 0
      "#{file}+#{ip-1}"
    else
      "#{file}:#{line}"
    end
  end

  def disable_long_return!
    # 12 => fc->flags
    # CTX_FLAG_NO_LONG_RETURN => 1
    _set_field(12, 1)
  end

  def context_stack
    ret = []
    ctx = self
    while ctx
      ret << ctx
      ctx = ctx.next_frame
    end
    ret
  end

  def stack_trace_starting_at(start=1)
    ret = []
    trace = self.context_stack
    return nil if start > trace.size
    trace.each_with_index do |frame, i|
      next if i < start
      ret.concat frame.position_info
    end
    ret
  end

  ##
  # Desrcibes the execution state of this context.  Produces the message you
  # would see in a backtrace print-out.

  def describe
    if method_module.equal?(Kernel)
      str = "Kernel."
    elsif method_module.kind_of?(MetaClass)
      str = "#{receiver}."
    elsif method_module and method_module != receiver.class
      str = "#{method_module}(#{receiver.class})#"
    else
      str = "#{receiver.class}#"
    end

    if kind_of? BlockContext
      str << "#{name} {}"
    elsif name == method.name
      str << "#{name}"
    else
      str << "#{name} (#{method.name})"
    end
  end

  def const_defined?(name)
    scope = method.staticscope
    while scope
      return true if scope.module.const_defined?(name)
      scope = scope.parent
    end

    return Object.const_defined?(name)
  end

  def const_path_defined?(path)
    if path.prefix? "::"
      return Object.const_path_defined? path[2..-1]
    end

    parts = path.split("::")
    top = parts.shift

    scope = method.staticscope

    while scope
      if mod = scope.module.__send__(:recursive_const_get, top, false)
        return mod.const_path_defined? parts.join("::")
      end

      scope = scope.parent
    end

    return Object.const_path_defined? parts.join("::")
  end

  def class_variable_get(name)
    return current_scope.class_variable_get(name)
  end

  def class_variable_set(name, val)
    return current_scope.class_variable_set(name, val)
  end

  def class_variable_defined?(name)
    return current_scope.class_variable_defined?(name)
  end

  def current_scope
    if ss = method.staticscope
      return ss.module
    else
      return method_module
    end
  end

  ##
  # Safely dups this MethodContext's method for manipulation.

  def make_independent
    self.method = method.dup
  end

  def send_private?
    @send_private
  end

  ##
  # Look up the staticscope chain to find the one with a Script object
  # attached to it. Return that object.

  def script_object
    if ss = method.staticscope
      while ss and !ss.script
        ss = ss.parent
      end

      return ss.script if ss
    end

    return nil
  end

  ##
  # Used to implement __FILE__ properly. kernel/core/compile.rb stashes
  # the path used to load this file in the Script object located in
  # the top staticscope.

  def active_path
    if script = script_object()
      if path = script.path
        return path.dup
      end
    end

    # If for some reason that didn't work, return the compile time filename.
    method.file.to_s
  end

  ##
  # Used to set the module body toggles

  attr_accessor :method_scope

  def alias_method(name, original)
    scope = MethodContext.current.sender.current_scope
    scope.__send__(:alias_method, name, original)
  end

  # This version is trivial, and is meant to match the API of BlockContext
  def __const_set__(name, value)
    const_scope = MethodContext.current.sender.receiver
    const_scope.__send__(:__const_set__, name, value)
  end

  ##
  # Called when 'def name' is used in userland

  def __add_method__(name, obj)
    s = MethodContext.current.sender
    scope = s.method_scope || :public

    if name == :initialize or scope == :module
      visibility = :private
    else
      visibility = scope
    end

    # All userland added methods start out with a serial of 1.
    obj.serial = 1

    # Push the scoping down.
    obj.staticscope = s.method.staticscope

    Rubinius::VM.reset_method_cache(name)

    obj.staticscope.module.method_table[name] = Tuple[visibility, obj]

    if scope == :module
      s.current_scope.module_function name
    end

    if s.current_scope.respond_to? :method_added
      s.current_scope.method_added(name)
    end

    # Return value here is the return value of the 'def' expression
    return obj
  end

end

##
# Stores all the information about a running NativeMethod.

class NativeMethodContext
  def location
    "#{file}"
  end
end

##
# Stores all information about a running BlockEnvironment.
#
# A BlockContext contains a home and a BlockEnvironment in addition to a
# MethodContext.  The home points to the MethodContext that encloses the
# block which is used to find self, etc.

class BlockContext < MethodContext

  def next_frame
    if self.env.caller_env
      self.env.caller_env.home_block
    else
      self.sender
    end
  end

  def from_eval?
    self.env.from_eval?
  end

  def position_info
    ret = super()
    if self.from_eval?
      home = self.env.home
      ret << "#{home.file}:#{home.line} in `#{home.method.name}'"
    end
    ret
  end

  def last_match
    home.last_match
  end

  def last_match=(match)
    home.last_match = match
  end

  def nth_ref(idx)
    home.nth_ref(idx)
  end

  def back_ref(idx)
    home.back_ref(idx)
  end

  def method_scope
    @method_scope || env.home_block.method_scope
  end

  ##
  # Active context (instance of MethodContext) that started
  # execution of this block context.

  def home
    env.home
  end

  ##
  # Name of home method.

  def name
    home.name
  end

  ##
  # Block context has no receiver thus uses receiver from it's home method
  # context.

  def receiver
    home.receiver
  end

  ##
  # Block context has no own module thus uses module from it's home method
  # context.

  def method_module
    home.method_module
  end

  ##
  # Static scope of home method context.

  def current_scope
    if ss = method.staticscope
      return ss.module
    else
      home.current_scope
    end
  end

  ##
  # instance_eval needs alternate const behavior

  def __const_set__(name, value)
    const_scope = env.constant_scope.module
    const_scope.__send__(:__const_set__, name, value)
  end
end

##
# Describes the environment a block was created in.  BlockEnvironment is used
# to create a BlockContext.

class BlockEnvironment
  ivar_as_index :__ivars__ => 0, :home => 1, :initial_ip => 2, :last_ip => 3,
    :post_send => 4, :home_block => 5, :local_count => 6, :metadata_container => 7, :method => 8
  def __ivars__   ; @__ivars__   ; end
  def home        ; @home        ; end
  def initial_ip  ; @initial_ip  ; end
  def last_ip     ; @last_ip     ; end
  def post_send   ; @post_send   ; end
  def home_block  ; @home_block  ; end
  def local_count ; @local_count ; end
  def method      ; @method      ; end

  attr_accessor :caller_env
  attr_accessor :registration_ip

  def home_block=(block)
    @home_block = block
  end

  def initial_ip=(ip)
    @initial_ip = ip
  end

  def last_ip=(ip)
    @last_ip = ip
  end

  def metadata_container
    @metadata_container
  end

  def under_context(home, cmethod)
    if home.kind_of? BlockContext
      home_block = home
      home = home.home
    else
      home_block = home
    end

    @home = home
    @initial_ip = 0
    @last_ip = 0x10000000 # 2**28
    @post_send = 0
    @home_block = home_block
    @method = cmethod
    @local_count = cmethod.local_count
    return self
  end

  ##
  # Holds a Tuple of additional metadata.
  # First field of the tuple holds a boolean indicating if the context is from
  # eval
  def metadata_container=(tup)
    @metadata_container = tup
  end

  def from_eval?
    @metadata_container and @metadata_container[0]
  end

  def from_eval!
    @metadata_container = Tuple.new(1) unless @metadata_container
    @metadata_container[0] = true
  end

  ##
  # The CompiledMethod object that we were called from

  def method=(tup)
    @method = tup
  end

  def home=(home)
    @home = home
  end

  def scope=(tup)
    @scope = tup
  end

  def make_independent
    @home = @home.dup
    @home_block = @home_block.dup
    @method = @method.dup
  end

  def redirect_to(obj)
    env = dup
    env.make_independent
    env.home.receiver = obj
    return env
  end

  def call_on_instance(obj, *args)
    obj = redirect_to(obj)
    obj.call(*args)
  end

  def disable_long_return!
    @post_send = nil
  end

  def arity
    method.required
  end

  # Static scope for constant lookup
  def constant_scope=(scope)
    @constant_scope = scope
  end

  def constant_scope
    @constant_scope ||= @method.staticscope
  end
end

