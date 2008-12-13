class CompiledMethod < Executable

  def self.allocate
    Ruby.primitive :compiledmethod_allocate
    raise PrimitiveFailure, "unable to create a CompiledMethod"
  end

  def compile
    Ruby.primitive :compiledmethod_compile
    raise PrimitiveFailure, "primitive failed"
  end

  def make_machine_method
    Ruby.primitive :compiledmethod_make_machine_method
    raise PrimitiveFailure, "primitive failed"
  end

  def activate(recv, mod, args)
    Ruby.primitive :compiledmethod_activate
    raise PrimitiveFailure, "CompiledMethod#activate failed"
  end

  ##
  # A method's visibility

  class Visibility

    ##
    # Create a MethodVisibility for +method+ with +visibility+

    def initialize(method, visibility)
      @method = method
      @visibility = visibility
    end

    ##
    # Is this method private?

    def private?
      @visibility == :private
    end

    ##
    # Is this method protected?

    def protected?
      @visibility == :protected
    end

    ##
    # Is this method public?

    def public?
      @visibility == :public
    end

    ##
    # Sets a visibility's visibility

    def visibility=(visibility)
      @visibility = visibility
    end

  end

end
