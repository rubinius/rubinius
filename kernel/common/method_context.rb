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

  def from_eval?
    false
  end

  def normalized_name
    if method_module.is_a?(MetaClass)
      begin
        "#{method_module.attached_instance.inspect}.#{name}"
      rescue Object
        "#{method_module.attached_instance.class}##{name}"
      end
    else
      if method_module
        "#{method_module.name}##{name}"
      else
        "#{receiver}.#{name}"
      end
    end
  end

  def position_info
    if [:__script__, :__block__].include?(self.name)
      "#{self.file}:#{self.line}"
    else
      "#{self.file}:#{self.line}:in `#{self.name}'"
    end
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
    ip = self.ip - 1
    ip = 0 if ip < 0
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
    frame = self
    while frame
      # Condition removes VM internal frames, which have no file.
      ret << frame if frame.file
      # If this context's env was created from a Proc binding
      # then we duplicate the frame and reset its instruction pointer
      # in order to show the first line of the block as the active
      # line in stack trace output
      if frame.__kind_of__(BlockContext) and frame.env.from_proc?
        frame = frame.context_from_proc
      else
        frame = frame.sender
      end
    end
    ret
  end

  # Get the first IP value in the 'home_block' that is on the
  # first line of the method representing the Proc and return
  # a copy of the Proc environment's home block initialized with
  # this IP
  def context_from_proc
    frame = self.env.proc_environment.home_block.dup

    first_line = self.env.proc_environment.method.first_line
    frame.ip = frame.method.first_ip_on_line(first_line) + 1

    return frame
  end

  def stack_trace_starting_at(start=1)
    ret = []
    trace = self.context_stack
    return nil if start > trace.size
    trace.each_with_index do |frame, i|
      next if i < start
      # The condition is so we don't output special VM frames, which
      # have no file.
      ret << frame.position_info if frame.file
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
    elsif method_module and method_module != receiver.__class__
      str = "#{method_module}(#{receiver.__class__})#"
    else
      str = "#{receiver.__class__}#"
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
    scope = method.scope
    while scope and scope.module != Object
      return true if scope.module.const_defined?(name)
      scope = scope.parent
    end

    return Object.const_defined?(name)
  end

  def const_path_defined?(path)
    if path.prefix? "::"
      return Object.const_path_defined?(path[2..-1])
    end

    parts = path.split("::")
    top = parts.shift

    scope = method.scope

    while scope
      mod = top.to_s !~ /self/ ? scope.module.__send__(:recursive_const_get, top, false) : scope.module
      return mod.const_path_defined?(parts.join("::")) if mod

      scope = scope.parent
    end

    return Object.const_path_defined?(parts.join("::"))
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
    if ss = method.scope
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
    if ss = method.scope
      while ss and !ss.script
        ss = ss.parent
      end

      return ss.script if ss
    end

    return nil
  end

  ##
  # Used to implement __FILE__ properly. kernel/common/compile.rb stashes
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
    obj.scope = s.method.scope

    Rubinius::VM.reset_method_cache(name)

    cm_vis = CompiledMethod::Visibility.new(obj, visibility)

    obj.scope.module.method_table[name] = cm_vis

    if scope == :module
      s.current_scope.module_function name
    end

    if s.current_scope.respond_to? :method_added
      s.current_scope.method_added(name)
    end

    # Return value here is the return value of the 'def' expression
    return obj
  end

  attr_reader :dynamic_locals

  ##
  # Called from BlockContext#set_eval_local to assign a local created
  # by eval.

  def set_eval_local(name, val)
    @dynamic_locals ||= LookupTable.new

    @dynamic_locals[name] = val
    return val
  end

  ##
  # Called from BlockContext#get_eval_local to access a local created
  # by eval.

  def get_eval_local(name)
    return nil unless @dynamic_locals

    return @dynamic_locals[name]
  end

end

#
# Stores all the information about a running NativeMethod.
#
class NativeMethodContext < MethodContext

  attr_accessor :sender
  attr_accessor :home
  attr_accessor :self
  attr_accessor :cm
  attr_accessor :module
  attr_accessor :block
  attr_accessor :name

  #
  # Special-needs copying.
  #
  def clone()
    raise "Why the hell are we cloning a context?"
    Ruby.primitive :nativemethodcontext_clone
  end

  #
  # Special-needs copying.
  #
  def dup()
    raise "Why the hell are we duping a context?"
    Ruby.primitive :nativemethodcontext_dup
  end

  def location()
    Ruby.primitive :nativemethodcontext_location
    "<unable to retrieve location info>"
  end

  def position_info()
    file, line = location
    "#{file}:#{line}, in `#{name}' (last known location)"
  end
end

