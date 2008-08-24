class CompiledMethod
  def self.load_from_file(path, version)
    Ruby.primitive :load_file
    raise PrimitiveFailure, "primitive failed"
  end

  def activate_as_script
    Ruby.primitive :activate_as_script
    raise PrimitiveFailure, "primitive failed"
  end

  def compile
    Ruby.primitive :iseq_compile
    raise PrimitiveFailure, "primitive failed"
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
