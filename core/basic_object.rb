class BasicObject
  def initialize
    # do nothing
  end
  private :initialize

  def equal?(other)
    Rubinius.primitive :object_equal
    raise ::PrimitiveFailure, "BasicObject#equal? primitive failed"
  end

  alias_method :==, :equal?

  def !
    Rubinius.invoke_primitive(:object_equal, self, false) ||
      Rubinius.invoke_primitive(:object_equal, self, nil) ? true : false
  end

  def !=(other)
    self == other ? false : true
  end

  # Send message to object with given arguments.
  #
  # Ignores visibility of method, and may therefore be used to
  # invoke protected or private methods.
  #
  # As denoted by the double-underscore, this method must not
  # be removed or redefined by user code.
  #
  def __send__(message, *args)
    Rubinius.primitive :object_send
    raise ::PrimitiveFailure, "BasicObject#__send__ primitive failed"
  end

  def __id__
    Rubinius.primitive :object_id
    raise ::PrimitiveFailure, "BasicObject#__id__ primitive failed"
  end

  def method_missing(meth, *args)
    error = ::NoMethodError
      .new("Unable to send '#{meth}' on instance of BasicObject", receiver: self)

    ::Kernel.raise(error)
  end
  private :method_missing

  def singleton_method_added(name) end
  private :singleton_method_added

  def singleton_method_removed(name) end
  private :singleton_method_removed

  def singleton_method_undefined(name) end
  private :singleton_method_undefined

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
    if ::ImmediateValue === self
      sc = nil
    else
      sc = ::Rubinius::Type.object_singleton_class(self)
    end

    if prc
      if string
        raise ::ArgumentError, 'cannot pass both a block and a string to evaluate'
      end

      return prc.ruby_method.call(self) if prc.ruby_method

      # Return a copy of the BlockEnvironment with the receiver set to self
      env = prc.block

      if sc
        lexical_scope = env.repoint_scope sc
      else
        lexical_scope = env.disable_scope!
      end

      return env.call_under(self, lexical_scope, true, self)
    elsif string
      string = ::Kernel.StringValue(string)

      lexical_scope = ::Rubinius::LexicalScope.of_sender

      if sc
        lexical_scope = ::Rubinius::LexicalScope.new(sc, lexical_scope)
      else
        lexical_scope = lexical_scope.using_disabled_scope
      end

      binding = ::Binding.setup(::Rubinius::VariableScope.of_sender,
                                ::Rubinius::CompiledCode.of_sender,
                                lexical_scope)

      c = ::Rubinius::ToolSets::Runtime::Compiler
      be = c.construct_block string, binding, filename, line

      be.call_on_instance(self)
    else
      raise ::ArgumentError, 'block not supplied'
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
    raise ::LocalJumpError, "Missing block" unless block_given?
    env = prc.block

    return prc.ruby_method.call(*args) if prc.ruby_method

    lexical_scope = env.lexical_scope
    if ::ImmediateValue === self
      lexical_scope = lexical_scope.using_disabled_scope
    else
      sc = ::Rubinius::Type.object_singleton_class(self)
      lexical_scope = lexical_scope.using_current_as(sc)
    end

    return env.call_under(self, lexical_scope, true, *args)
  end
end
