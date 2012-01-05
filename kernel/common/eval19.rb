# -*- encoding: us-ascii -*-

class BasicObject
  ##
  # :call-seq:
  #   obj.instance_eval(string [, filename [, lineno]] )   => obj
  #   obj.instance_eval { | | block }                      => obj
  #
  # Evaluates a string containing Ruby source code, or the given block, within
  # the context of the receiver +obj+. In order to set the context, the
  # variable +self+ is set to +obj+ while the code is executing, giving the
  # code access to +obj+'s instance variables. In the version of
  # #instance_eval that takes a +String+, the optional second and third
  # parameters supply a filename and starting line number that are used when
  # reporting compilation errors.
  #
  #   class Klass
  #     def initialize
  #       @secret = 99
  #     end
  #   end
  #   k = Klass.new
  #   k.instance_eval { @secret }   #=> 99

  def instance_eval(string=nil, filename="(eval)", line=1, &prc)
    if ImmediateValue === self
      sc = nil
    else
      sc = Rubinius::Type.object_singleton_class(self)
    end

    if prc
      if string
        raise ArgumentError, 'cannot pass both a block and a string to evaluate'
      end
      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block

      if sc
        static_scope = env.repoint_scope sc
      else
        static_scope = env.disable_scope!
      end

      return env.call_under(self, static_scope, self)
    elsif string
      string = StringValue(string)

      static_scope = Rubinius::StaticScope.of_sender

      if sc
        static_scope = Rubinius::StaticScope.new(sc, static_scope)
      else
        static_scope = static_scope.using_disabled_scope
      end

      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledMethod.of_sender,
                              static_scope)

      be = Rubinius::Compiler.construct_block string, binding,
                                              filename, line

      be.call_on_instance(self)
    else
      raise ArgumentError, 'block not supplied'
    end
  end

  ##
  # :call-seq:
  #   obj.instance_exec(arg, ...) { |var,...| block }  => obj
  #
  # Executes the given block within the context of the receiver +obj+. In
  # order to set the context, the variable +self+ is set to +obj+ while the
  # code is executing, giving the code access to +obj+'s instance variables.
  #
  # Arguments are passed as block parameters.
  #
  #   class Klass
  #     def initialize
  #       @secret = 99
  #     end
  #   end
  #
  #   k = Klass.new
  #   k.instance_exec(5) { |x| @secret+x }   #=> 104

  def instance_exec(*args, &prc)
    raise LocalJumpError, "Missing block" unless block_given?
    env = prc.block

    if prc.kind_of? Proc::Method
      return prc.bound_method.call(*args)
    end

    static_scope = env.static_scope
    if ImmediateValue === self
      static_scope = static_scope.using_disabled_scope
    else
      sc = Rubinius::Type.object_singleton_class(self)
      static_scope = static_scope.using_current_as(sc)
    end

    return env.call_under(self, static_scope, *args)
  end
end

module Kernel

  # Evaluate and execute code given in the String.
  #
  def eval(string, binding=nil, filename=nil, lineno=1)
    filename = StringValue(filename) if filename
    lineno = Type.coerce_to lineno, Fixnum, :to_i

    if binding
      if binding.kind_of? Proc
        raise TypeError, 'wrong argument type Proc (expected Binding)'
      elsif binding.respond_to? :to_binding
        binding = binding.to_binding
      end

      unless binding.kind_of? Binding
        raise ArgumentError, "unknown type of binding"
      end

      filename ||= binding.static_scope.active_path
    else
      binding = Binding.setup(Rubinius::VariableScope.of_sender,
                              Rubinius::CompiledMethod.of_sender,
                              Rubinius::StaticScope.of_sender,
                              self)

      filename ||= "(eval)"
    end

    binding.static_scope = binding.static_scope.dup

    be = Rubinius::Compiler.construct_block string, binding,
                                            filename, lineno

    be.set_eval_binding binding

    be.call_on_instance(binding.self)
  end
  module_function :eval
  private :eval

end
