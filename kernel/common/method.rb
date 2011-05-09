##
# Method objects are essentially detached, freely passed-around methods. The
# Method is a copy of the method on the object at the time of extraction, so
# if the method itself is changed, overridden, aliased or removed from the
# object, the Method object still contains the old functionality. In addition,
# the call itself is not in any way stored so it will reflect the state of the
# object at the time of calling.
#
# Methods are normally bound to a particular object but it is possible to use
# Method#unbind to create an UnboundMethod object for the purpose of
# re-binding to a different object.

class Method

  ##
  # Takes and stores the receiver object, the method's bytecodes and the
  # Module that the method is defined in.

  def initialize(receiver, defined_in, executable, name)
    @receiver    = receiver
    @defined_in  = defined_in
    @executable  = executable
    @name        = name
  end

  attr_reader :receiver
  attr_reader :defined_in
  attr_reader :executable

  def name
    @name.to_s
  end

  ##
  # Method objects are equal if they have the same body and are bound to the
  # same object.

  def ==(other)
    other.class == Method and
      @receiver.equal?(other.receiver) and
      @executable == other.executable
  end

  ##
  # Indication of how many arguments this method takes. It is defined so that
  # a non-negative Integer means the method takes that fixed amount of
  # arguments (up to 1024 currently.) A negative Integer is used to indicate a
  # variable argument count. The number is ((-n) - 1), where n is the number
  # of required args. Blocks are not counted.
  #
  #   def foo();             end   # arity => 0
  #   def foo(a, b);         end   # arity => 2
  #   def foo(a, &b);        end   # arity => 1
  #   def foo(a, b = nil);   end   # arity => ((-1) -1) => -2
  #   def foo(*a);           end   # arity => ((-0) -1) => -1
  #   def foo(a, b, *c, &d); end   # arity => ((-2) -1) => -3

  def arity
    @executable.arity
  end

  ##
  # Execute the method. This works exactly like calling a method with the same
  # code on the receiver object. Arguments and a block can be supplied
  # optionally.

  def call(*args, &block)
    @executable.invoke(@name, @defined_in, @receiver, args, block)
  end

  alias_method :[], :call

  ##
  # String representation of this Method includes the method name, the Module
  # it is defined in and the Module that it was extracted from.

  def inspect
    file, line = source_location()

    if @executable.respond_to? :eval_source
      if src = @executable.eval_source
        src_line = src.split("\n")[line - 1]
        return "#<#{self.class}: #{@receiver.class}##{@name} (defined in #{@defined_in} from \"#{src_line}\")>"
      end
    end

    if file
      "#<#{self.class}: #{@receiver.class}##{@name} (defined in #{@defined_in} at #{file}:#{line})>"
    else
      "#<#{self.class}: #{@receiver.class}##{@name} (defined in #{@defined_in})>"
    end
  end

  alias_method :to_s, :inspect

  ##
  # Location gives the file and line number of the start of this method's
  # definition.

  def source_location
    if @executable.respond_to? :file
      [@executable.file.to_s, @executable.defined_line]
    elsif @executable.respond_to? :source_location
      @executable.source_location
    else
      nil
    end
  end

  def source
    if @executable.respond_to? :eval_source
      return @executable.eval_source
    end

    return nil
  end

  def parameters
    @executable.local_names.each_with_index.map do |name, i|
      if i < @executable.required_args
        [:req, name]
      elsif i < @executable.total_args
        [:opt, name]
      elsif @executable.splat == i
        [:rest, name]
      else
        [:block, name]
      end
    end
  end

  def owner
    if @defined_in.class == Rubinius::IncludedModule
      @defined_in.module
    else
      @defined_in
    end
  end

  ##
  # Returns a Proc object corresponding to this Method.

  def to_proc
    Proc::Method.new self
  end

  ##
  # Detach this Method from the receiver object it is bound to and create an
  # UnboundMethod object. Populates the UnboundMethod with the method data as
  # well as the Module it is defined in and the Module it was extracted from.
  #
  # See UnboundMethod for more information.

  def unbind
    UnboundMethod.new(@defined_in, @executable, @receiver.class, @name)
  end

end

##
# UnboundMethods are similar to Method objects except that they are not
# connected to any particular object. They cannot be used standalone for this
# reason, and must be bound to an object first. The object must be kind_of?
# the Module in which this method was originally defined.
#
# UnboundMethods can be created in two ways: first, any existing Method object
# can be sent #unbind to detach it from its current object and return an
# UnboundMethod instead. Secondly, they can be directly created by calling
# Module#instance_method with the desired method's name.
#
# The UnboundMethod is a copy of the method as it existed at the time of
# creation. Any subsequent changes to the original will not affect any
# existing UnboundMethods.

class UnboundMethod

  ##
  # Accepts and stores the Module where the method is defined in as well as
  # the CompiledMethod itself. Class of the object the method was extracted
  # from can be given but will not be stored. This is always used internally
  # only.

  def initialize(mod, executable, pulled_from, name)
    @defined_in  = mod
    @executable  = executable
    @pulled_from = pulled_from
    @name        = name
  end

  attr_reader :executable
  attr_reader :defined_in

  def name
    @name.to_s
  end

  ##
  # UnboundMethod objects are equal if and only if they refer to the same
  # method. One may be an alias for the other or both for a common one. Both
  # must have been extracted from the same class or subclass. Two from
  # different subclasses will not be considered equal.

  def ==(other)
    other.kind_of? UnboundMethod and
      @defined_in == other.defined_in and
      @executable == other.executable
  end

  ##
  # See Method#arity.

  def arity
    @executable.arity
  end

  ##
  # Creates a new Method object by attaching this method to the supplied
  # receiver. The receiver must be kind_of? the Module that the method in
  # question is defined in.
  #
  # Notably, this is a difference from MRI which requires that the object is
  # of the exact Module the method was extracted from. This is safe because
  # any overridden method will be identified as being defined in a different
  # Module anyway.

  def bind(receiver)
    unless Rubinius::Type.object_kind_of? receiver, @defined_in
      if Rubinius::Type.object_kind_of?(@defined_in, Class) and
         Rubinius::Type.singleton_class_object(@defined_in)
        raise TypeError, "illegal attempt to rebind a singleton method to another object"
      end

      raise TypeError, "Must be bound to an object of kind #{@defined_in}"
    end

    Method.new receiver, @defined_in, @executable, @name
  end

  ##
  # Convenience method for #binding to the given receiver object and calling
  # it with the optionally supplied arguments.

  def call_on_instance(obj, *args, &block)
    unless Rubinius::Type.object_kind_of? obj, @defined_in
      if Rubinius::Type.object_kind_of?(@defined_in, Class) and
         Rubinius::Type.singleton_class_object(@defined_in)
        raise TypeError, "illegal attempt to rebind a singleton method to another object"
      end

      raise TypeError, "Must be bound to an object of kind #{@defined_in}"
    end

    @executable.invoke(@name, @defined_in, obj, args, block)
  end

  ##
  # String representation for UnboundMethod includes the method name, the
  # Module it is defined in and the Module that it was extracted from.

  def inspect
    "#<#{self.class}: #{@pulled_from}##{@name} (defined in #{@defined_in})>"
  end

  alias_method :to_s, :inspect

  def source_location
    if @executable.respond_to? :file
      [@executable.file.to_s, @executable.defined_line]
    elsif @executable.respond_to? :source_location
      @executable.source_location
    else
      nil
    end
  end

  def parameters
    @executable.local_names.each_with_index.map do |name, i|
      if i < @executable.required_args
        [:req, name]
      elsif i < @executable.total_args
        [:opt, name]
      elsif @executable.splat == i
        [:rest, name]
      else
        [:block, name]
      end
    end
  end

  def owner
    if @defined_in.class == Rubinius::IncludedModule
      @defined_in.module
    else
      @defined_in
    end
  end
end
