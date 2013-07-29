# -*- encoding: us-ascii -*-

#
# This is the beginning of loading Ruby code. At this point, the VM
# is bootstrapped and the fundamental data structures, primitive
# functions and the basic classes and objects are available.
#
# The classes, modules, and methods defined here provide basic
# functionality needed to load the bootstrap directory. By the
# end of this file, the following methods are available:
#
#   attr_reader :sym
#   attr_writer :sym
#   attr_accessor :sym
#
#   private :sym
#   protected :sym
#
#   module_function :sym
#
# These forms should be used in the rest of the kernel. In delta,
# more complete forms of these methods are provided for user code.
#
# NOTE: The order of these definitions is important. Do not
#       change it without consultation.


# This class encapsulates primitives that involve the VM
# itself rather than something in Ruby-land.
#
# See kernel/bootstrap/vm.rb
#
class Rubinius::VM

  # Write given string to stderr.
  #
  # Used to support error reporting where IO is not reliable yet.
  #
  def self.write_error(str)
    Rubinius.primitive :vm_write_error
    raise PrimitiveFailure, "Rubinius::VM.write_error primitive failed"
  end

  # Prints Ruby backtrace at point of call.
  #
  def self.show_backtrace
    Rubinius.primitive :vm_show_backtrace
    raise PrimitiveFailure, "Rubinius::VM.show_backtrace primitive failed"
  end

  # Reset the method cache globally for given method name.
  #
  def self.reset_method_cache(mod, sym)
    Rubinius.primitive :vm_reset_method_cache
    raise PrimitiveFailure, "Rubinius::VM.reset_method_cache primitive failed"
  end
end


class Object
  # Prints basic information about the object to stdout.
  #
  def __show__
    Rubinius.primitive :object_show
    "<unknown>"
  end
end


class Class

  # Allocate memory for an instance of the class without initialization.
  #
  # The object returned is valid to use, but its #initialize
  # method has not been called. In almost all cases, .new is
  # the correct method to use instead.
  #
  # See .new
  #
  def allocate
    Rubinius.primitive :class_allocate
    raise RuntimeError, "Class#allocate primitive failed on #{self.inspect}"
  end

  # Allocate and initialize an instance of the class.
  #
  # Default implementation: merely allocates the instance, and
  # then calls the #initialize method on the object with the
  # given arguments and block, if provided.
  #
  # See .allocate
  #
  def new(*args)
    obj = allocate()

    Rubinius.asm(args, obj) do |args, obj|
      run obj
      run args
      push_block
      send_with_splat :initialize, 0, true
      # no pop here, as .asm blocks imply a pop as they're not
      # allowed to leak a stack value
    end

    obj
  end
end


module Kernel

  # Return the Class object this object is an instance of.
  #
  # Note that this method must always be called with an
  # explicit receiver, since class by itself is a keyword.
  #
  def class
    Rubinius.primitive :object_class
    raise PrimitiveFailure, "Kernel#class primitive failed."
  end

  # Return the class object of self.
  #
  # This is the same as #class, but named to always be available.
  def __class__
    Rubinius.primitive :object_class
    raise PrimitiveFailure, "Kernel#class primitive failed."
  end

  # String representation of an object.
  #
  # By default, the representation is the name of the object's
  # class preceded by a # to indicate the object is an instance
  # thereof.
  #
  def to_s
    "#<#{self.class.name}>"
  end

  # :internal:
  #
  # Lowest-level implementation of raise, used internally by
  # kernel code until a more sophisticated version is loaded.
  #
  # Redefined later.
  #
  def raise(cls, str, junk=nil)
    Rubinius::VM.write_error "Fatal error loading runtime kernel:\n  "
    Rubinius::VM.write_error str
    Rubinius::VM.write_error "\n"
    Rubinius::VM.show_backtrace
    Process.exit! 1
  end

  # Returns true if the given Class is either the class or superclass of the
  # object or, when given a Module, if the Module has been included in object's
  # class or one of its superclasses. Returns false otherwise.
  #
  # If the argument is not a Class or Module, a TypeError is raised.
  #
  def kind_of?(cls)
    Rubinius.primitive :object_kind_of
    raise TypeError, "Kernel#kind_of? requires a Class or Module argument"
  end

  # Hook method invoked when object is sent a message it cannot handle.
  #
  # The default implementation will merely raise a NoMethodError with
  # information about the message.
  #
  # This method may be overridden, and is often used to provide dynamic
  # behaviour. An overriding version should call super if it fails to
  # resolve the message. This practice ensures that the default version
  # will be called if all else fails.
  #
  def method_missing(meth, *args)
    raise NoMethodError, "Unable to send '#{meth}' on '#{self}' (#{self.class})"
  end

  # :internal:
  #
  # Backend method for Object#dup and Object#clone.
  #
  # Redefined in kernel/common/kernel.rb
  #
  def initialize_copy(other)
  end

  # Generic shallow copy of object.
  #
  # Copies instance variables, but does not recursively copy the
  # objects they reference. Copies taintedness.
  #
  # In contrast to .clone, .dup can be considered as creating a
  # new object of the same class and populating it with data from
  # the object.
  #
  # If class-specific behaviour is desired, the class should
  # define #initialize_copy and implement the behaviour there.
  # #initialize_copy will automatically be called on the new
  # object - the copy - with the original object as argument
  # if defined.
  #
  def dup
    copy = Rubinius::Type.object_class(self).allocate

    Rubinius.invoke_primitive :object_copy_object, copy, self

    Rubinius::Type.object_initialize_dup self, copy
    copy
  end

  # Direct shallow copy of object.
  #
  # Copies instance variables, but does not recursively copy the
  # objects they reference. Copies taintedness and frozenness.
  #
  # In contrast to .dup, .clone can be considered to actually
  # clone the existing object, including its internal state
  # and any singleton methods.
  #
  # If class-specific behaviour is desired, the class should
  # define #initialize_copy and implement the behaviour there.
  # #initialize_copy will automatically be called on the new
  # object - the copy - with the original object as argument
  # if defined.
  #
  def clone
    # Do not implement in terms of dup. It breaks rails.
    #
    copy = Rubinius::Type.object_class(self).allocate

    Rubinius.invoke_primitive :object_copy_object, copy, self
    Rubinius.invoke_primitive :object_copy_singleton_class, copy, self

    Rubinius::Type.object_initialize_clone self, copy

    copy.freeze if frozen?
    copy
  end
end


# Module for internals.
#
# See kernel/bootstrap/rubinius.rb
#
module Rubinius

  # Executable abstraction for accessors.
  #
  class AccessVariable

    # Specialised allocation.
    #
    def self.allocate
      Rubinius.primitive :accessvariable_allocate
      raise PrimitiveFailure, "AccessVariable.allocate primitive failed"
    end

    # Set up the executable.
    #
    # Name of variable provided without leading @, the
    # second parameter should be true if the attr is
    # writable.
    #
    def initialize(variable, write)
      @primitive = nil
      @serial = 0
      @name = "@#{variable}".to_sym
      @write = write
    end

    # Get the arity for this reader or writr
    #
    def arity
      @write ? 1 : 0
    end

    # Create a getter for named instance var, without leading @.
    #
    def self.get_ivar(name)
      new(name, false)
    end

    # Create a setter for named instance var, without leading @.
    #
    def self.set_ivar(name)
      new(name, true)
    end
  end

  # Simplified lookup table.
  #
  # See kernel/bootstrap/lookuptable.rb.
  #
  class LookupTable

    # Retrieve value for given key.
    #
    def [](key)
      Rubinius.primitive :lookuptable_aref
      raise PrimitiveFailure, "LookupTable#[] primitive failed"
    end

    # Store value under key.
    #
    def []=(key, val)
      Rubinius.primitive :lookuptable_store
      raise PrimitiveFailure, "LookupTable#[]= primitive failed"
    end
  end

  # Constant table for storing methods.
  #
  # See kernel/bootstrap/constant_table.rb and
  #     kernel/common/constant_table.rb
  #
  class ConstantTable

    # Perform lookup for constant name.
    #
    def lookup(name)
      Rubinius.primitive :constant_table_lookup
      raise PrimitiveFailure, "ConstantTable#lookup primitive failed"
    end

    # Store Constant under name, with given visibility.
    #
    def store(name, constant, visibility)
      Rubinius.primitive :constant_table_store
      raise PrimitiveFailure, "ConstantTable#store primitive failed"
    end
  end

  # Lookup table for storing methods.
  #
  # See kernel/bootstrap/methodtable.rb and
  #     kernel/common/method_table.rb
  #
  class MethodTable

    # Perform lookup for method name.
    #
    def lookup(name)
      Rubinius.primitive :methodtable_lookup
      raise PrimitiveFailure, "MethodTable#lookup primitive failed"
    end

    # Store Executable under name, with given visibility.
    #
    def store(name, exec, visibility)
      Rubinius.primitive :methodtable_store
      raise PrimitiveFailure, "MethodTable#store primitive failed"
    end

    # Make an alias from +original_name+ in +original_mod+ to +name+
    # with visibility +vis+
    def alias(name, visibility, original_name, original_exec, original_mod)
      Rubinius.primitive :methodtable_alias
      raise PrimitiveFailure, "MethodTable#alias primitive failed"
    end
  end
end


class Symbol
  # Produce String representation of this Symbol.
  #
  def to_s
    Rubinius.primitive :symbol_to_s
    raise PrimitiveFailure, "Symbol#to_s primitive failed"
  end

  # For completeness, returns self.
  #
  def to_sym
    self
  end
end


class String
  # Returns the <code>Symbol</code> corresponding to <i>self</i>, creating the
  # symbol if it did not previously exist. See <code>Symbol#id2name</code>.
  #
  #   "Koala".intern         #=> :Koala
  #   s = 'cat'.to_sym       #=> :cat
  #   s == :cat              #=> true
  #   s = '@cat'.to_sym      #=> :@cat
  #   s == :@cat             #=> true
  #
  # This can also be used to create symbols that cannot be represented using the
  # <code>:xxx</code> notation.
  #
  #   'cat and dog'.to_sym   #=> :"cat and dog"
  #--
  # TODO: Add taintedness-check
  #++
  def to_sym
    Rubinius.primitive :symbol_lookup
    raise PrimitiveFailure, "String#to_sym primitive failed: #{self.dump}"
  end

  # For completeness, returns self.
  #
  def to_s
    self
  end
end


module Process
  # Terminate with given status code.
  #
  def self.exit(code=0)
    case code
    when true
      code = 0
    when false
      code = 1
    else
      code = Rubinius::Type.coerce_to code, Integer, :to_int
    end

    raise SystemExit.new(code)
  end

  def self.exit!(code=1)
    Rubinius.primitive :vm_exit

    case code
    when true
      exit! 0
    when false
      exit! 1
    else
      exit! Rubinius::Type.coerce_to(code, Integer, :to_int)
    end
  end
end


class Module
  def method_table   ; @method_table ; end
  def constant_table ; @constant_table ; end
  def name           ; @module_name.to_s ; end
  def origin         ; @origin ; end

  # Specialised allocator.
  #
  def self.allocate
    Rubinius.primitive :module_allocate
    raise PrimitiveFailure, "Module.allocate primitive failed"
  end

  # :internal:
  #
  # Hook called when a constant cannot be located.
  #
  # Default implementation 'raises', but we don't use #raise
  # to prevent infinite recursion.
  #
  # Redefined in kernel/common/module.rb
  #
  def const_missing(name)
    Rubinius::VM.write_error "Missing or uninitialized constant: \n"
    Rubinius::VM.write_error name.to_s
    Rubinius::VM.write_error "\n"
  end

  # Set Module's direct superclass.
  #
  # The corresponding 'getter' #superclass method is defined
  # in class.rb, because it is more complex than a mere
  # accessor
  #
  def superclass=(other)
    Rubinius.check_frozen
    @superclass = other
  end

  # Module's stored superclass.
  #
  # This may be either an included Module or an inherited Class.
  #
  def direct_superclass
    @superclass
  end

  # :internal:
  #
  # Perform actual work for including a Module in this one.
  #
  # Redefined in kernel/delta/module.rb.
  #
  def append_features(mod)
    im = Rubinius::IncludedModule.new(self)
    im.attach_to mod
  end

  # Hook method called on Module when another Module is .include'd into it.
  #
  # Override for module-specific behaviour.
  #
  def included(mod); end

  # :internal:
  #
  # Basic version of .include used in kernel code.
  #
  # Redefined in kernel/delta/module.rb.
  #
  def include(mod)
    mod.append_features(self)
    Rubinius.privately do
      mod.included self
    end
    self
  end

  # :internal:
  #
  # Basic version used in kernel code.
  #
  # Redefined in kernel/common/module.rb.
  #
  def attr_reader(name)
    meth = Rubinius::AccessVariable.get_ivar name
    @method_table.store name, meth, :public
    Rubinius::VM.reset_method_cache self, name
    nil
  end

  def attr_reader_specific(name, method_name)
    meth = Rubinius::AccessVariable.get_ivar name
    @method_table.store method_name, meth, :public
    Rubinius::VM.reset_method_cache self, method_name
    nil
  end

  # :internal:
  #
  # Basic version used in kernel code.
  #
  # Redefined in kernel/common/module.rb.
  #
  def attr_writer(name)
    meth = Rubinius::AccessVariable.set_ivar name
    writer_name = "#{name}=".to_sym
    @method_table.store writer_name, meth, :public
    Rubinius::VM.reset_method_cache self, writer_name
    nil
  end

  # :internal:
  #
  # Basic version used in kernel code.
  #
  # Redefined in kernel/common/module.rb.
  #
  def attr_accessor(name)
    attr_reader(name)
    attr_writer(name)
    nil
  end

  # :internal:
  #
  # Basic version used in kernel code.
  # Cannot be used as a toggle, and only
  # takes a single method name.
  #
  # Redefined in kernel/delta/module.rb.
  #
  def private(name)
    if entry = @method_table.lookup(name)
      entry.visibility = :private
    end
  end

  # :internal:
  #
  # Basic version used in kernel code.
  # Cannot be used as a toggle, and only
  # takes a single method name.
  #
  # Redefined in kernel/common/module.rb.
  #
  def protected(name)
    if entry = @method_table.lookup(name)
      entry.visibility = :protected
    end
  end

  # :internal:
  #
  # Basic version used in kernel code. Creates a copy
  # of current method and stores it under the new name.
  # The two are independent.
  #
  # Redefined in kernel/delta/module.rb.
  #
  def alias_method(new_name, current_name)
    # If we're aliasing a method we contain, just reference it directly, no
    # need for the alias wrapper
    if entry = @method_table.lookup(current_name)
      @method_table.store new_name, entry.method, entry.visibility
    else
      mod = direct_superclass()
      while mod
        entry = mod.method_table.lookup(current_name)
        break if entry
        mod = mod.direct_superclass
      end

      unless entry
        raise NoMethodError, "No method '#{current_name}' to alias to '#{new_name}'"
      end

      @method_table.alias new_name, entry.visibility, current_name,
                          entry.method, mod
    end

    Rubinius::VM.reset_method_cache self, new_name
  end

  # :internal:
  #
  # Basic version used in kernel code. Only
  # takes a single method name.
  #
  # Redefined in kernel/common/module.rb.
  #
  def module_function(name)
    if entry = @method_table.lookup(name)
      sc = class << self; self; end
      sc.method_table.store name, entry.method, :public
      Rubinius::VM.reset_method_cache self, name
      private name
    end
  end

  def track_subclass(cls)
    Rubinius.primitive :module_track_subclass
    raise PrimitiveFailure, "Module.track_subclass primitive failed"
  end
end


module Rubinius

  class AccessVariable
    attr_reader :name
  end

  # Visibility handling for MethodTables.
  #
  # See kernel/bootstrap/methodtable.rb and
  #     kernel/common/method_table.rb
  #
  class MethodTable::Bucket
    attr_accessor :visibility
    attr_accessor :method

    def public?
      @visibility == :public
    end

    def private?
      @visibility == :private
    end

    def protected?
      @visibility == :protected
    end
  end

  # :internal:
  #
  # Internal representation of a Module's inclusion in another.
  #
  # Abstracts the injection of the included Module into the
  # ancestor hierarchy for method- and constant lookup in a
  # roughly transparent fashion.
  #
  # This class is known to the VM.
  #
  class IncludedModule < Module
    attr_reader :superclass
    attr_reader :module

    # :internal:
    #
    # Specialised allocator.
    #
    def self.allocate
      Rubinius.primitive :included_module_allocate
      raise PrimitiveFailure, "IncludedModule.allocate primitive failed"
    end

    # :internal:
    #
    # Created referencing the Module that is being included.
    #
    def initialize(mod)
      @method_table = mod.method_table
      @constant_table = mod.constant_table
      @module = mod
    end

    def module
      @module
    end

    # :internal:
    #
    # Inject self inbetween class and its previous direct
    # superclass.
    #
    def attach_to(cls)
      @superclass = cls.direct_superclass
      cls.superclass = self
      @module.track_subclass(cls)
      self
    end

    # :internal:
    #
    # Name of the included Module.
    #
    def name
      @module.name
    end

    # :internal:
    #
    # String representation of the included Module.
    #
    def to_s
      @module.to_s
    end

    def inspect
      "#<IncludedModule #{@module.to_s}>"
    end

    # Returns true if +other+ is the same object as self or if +other+
    # is the module this IncludedModule is for.
    #
    def ==(other)
      if other.kind_of? IncludedModule
        super || other.module == @module
      else
        super || other == @module
      end
    end
  end
end

class Object
  include Kernel
end

