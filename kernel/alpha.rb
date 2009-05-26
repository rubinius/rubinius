##
# This is the beginning of loading Ruby code. At this point, the VM
# is bootstrapped and the fundamental data structures and primitive
# functions are available.
#
# The classes, modules, and methods defined here provide basic
# functionality for loading the bootstrap directory. By the end of
# this file, the following methods are functioal:
#
#   attr_reader :sym
#   attr_writer :sym
#   attr_accessor :sym
#   private :sym
#   protected :sym
#   module_function :sym
#
# These forms should be used in the rest of the kernel. In delta,
# more complete forms of these methods are provided

def opened_class(created)
end

class Rubinius::VM
  def self.write_error(str)
    Ruby.primitive :vm_write_error
    raise PrimitiveFailure, "Rubinius::VM.write_error primitive failed"
  end

  def self.show_backtrace
    Ruby.primitive :vm_show_backtrace
    raise PrimitiveFailure, "Rubinius::VM.show_backtrace primitive failed"
  end

  def self.reset_method_cache(sym)
    Ruby.primitive :vm_reset_method_cache
    raise PrimitiveFailure, "Rubinius::VM.reset_method_cache primitive failed"
  end
end

class Object
  def __show__
    Ruby.primitive :object_show
  end
end

class Class
  def allocate
    Ruby.primitive :class_allocate
    raise RuntimeError, "Class#allocate primitive failed on #{self.inspect}"
  end

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

    return obj
  end
end

module Kernel
  def __send__(*args)
    Ruby.primitive :object_send
    raise RuntimeError, "Kernel#__send__ primitive failed"
  end

  def class
    Ruby.primitive :object_class
    raise PrimitiveFailure, "Kernel#class primitive failed."
  end

  def to_s
    "#<#{self.class.name}>"
  end

  def raise(cls, str, junk=nil)
    Rubinius::VM.write_error "Fatal error loading runtime kernel:\n  "
    Rubinius::VM.write_error str
    Rubinius::VM.write_error "\n"
    Rubinius::VM.show_backtrace
    Process.exit 1
  end

  # Returns true if the given Class is either the class or superclass of the
  # object or, when given a Module, if the Module has been included in
  # object's class or one of its superclasses. Returns false otherwise. If the
  # argument is not a Class or Module, a TypeError is raised.
  def kind_of?(cls)
    Ruby.primitive :object_kind_of
    raise TypeError, 'kind_of? requires a Class or Module argument'
  end

  def method_missing(meth, *args)
    raise NoMethodError, "Unable to send '#{meth}' on '#{self}' (#{self.class})"
  end

  # reimplemented in kernel/common/kernel.rb
  def initialize_copy(other)
  end

  def copy_object(other)
    Ruby.primitive :object_copy_object
    raise PrimitiveFailure, "Kernel#copy_object primitive failed"
  end

  def copy_metaclass(other)
    Ruby.primitive :object_copy_metaclass
    raise PrimitiveFailure, "Kernel#copy_metaclass primitive failed"
  end

  def dup
    copy = self.class.allocate
    copy.copy_object self
    copy.send :initialize_copy, self
    copy
  end

  def clone
    copy = dup
    copy.copy_metaclass self
    copy.freeze if frozen?
    copy
  end
end

module Rubinius
  class CompiledMethod < Executable
    class Visibility
      # Create a MethodVisibility for +method+ with +visibility+
      def initialize(method, visibility)
        @method = method
        @visibility = visibility
      end
    end
  end

  class AccessVariable
    def self.allocate
      Ruby.primitive :accessvariable_allocate
      raise PrimitiveFailure, "AccessVariable.allocate primitive failed"
    end

    def initialize(variable, write)
      @primitive = nil
      @serial = 0
      @name = "@#{variable}".to_sym
      @write = write
    end

    def self.get_ivar(name)
      new(name, false)
    end

    def self.set_ivar(name)
      new(name, true)
    end
  end

  class LookupTable
    def [](key)
      Ruby.primitive :lookuptable_aref
      raise PrimitiveFailure, "LookupTable#[] primitive failed"
    end

    def []=(key, val)
      Ruby.primitive :lookuptable_store
      raise PrimitiveFailure, "LookupTable#[]= primitive failed"
    end
  end
end

class Symbol
  def to_s
    Ruby.primitive :symbol_to_s
    raise PrimitiveFailure, "Symbol#to_s primitive failed."
  end

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
    Ruby.primitive :symbol_lookup
    raise PrimitiveFailure, "Unable to symbolize: #{self.dump}"
  end

  def to_s
    self
  end
end

class Process
  def self.exit(code)
    Ruby.primitive :vm_exit
    raise PrimitiveFailure, "exit() failed. Wow, something is screwed."
  end
end

class Module
  def method_table   ; @method_table ; end
  def constant_table ; @constants    ; end
  def encloser       ; @encloser     ; end
  def name           ; @name.to_s    ; end

  def self.allocate
    Ruby.primitive :module_allocate
    raise PrimitiveFailure, "Module.allocate primitive failed"
  end

  def __find_method(namesym)
    Ruby.primitive :find_method
    raise PrimitiveFailure, "primitive failed"
  end

  # We don't use #raise to prevent infinite recursion. A proper
  # version of #const_missing is provided in kernel/common.
  def const_missing(name)
    Rubinius::VM.write_error "Missing or uninitialized constant: \n"
    Rubinius::VM.write_error name.to_s
    Rubinius::VM.write_error "\n"
  end

  # 'superclass' method defined in class.rb,
  # because it is more complex than a mere accessor
  def superclass=(other)
    @superclass = other
  end

  # This may be either an included Module or then
  # an inherited Class.
  def direct_superclass
    @superclass
  end

  def append_features(mod)
    im = Rubinius::IncludedModule.new(self)
    im.attach_to mod
  end

  def included(mod); end

  def include(mod)
    mod.append_features(self)
    mod.included(self)
    self
  end

  def attr_reader(name)
    meth = Rubinius::AccessVariable.get_ivar name
    @method_table[name] = meth
    return nil
  end

  def attr_writer(name)
    meth = Rubinius::AccessVariable.set_ivar name
    @method_table["#{name}=".to_sym] = meth
    return nil
  end

  def attr_accessor(name)
    attr_reader(name)
    attr_writer(name)
    return true
  end

  def private(name)
    if entry = @method_table[name]
      unless entry.kind_of? Rubinius::Executable
        entry.visibility = :private
      else
        cmv = Rubinius::CompiledMethod::Visibility.new entry, :private
        @method_table[name] = cmv
      end
    end
  end

  def protected(name)
    if entry = @method_table[name]
      unless entry.kind_of? Rubinius::Executable
        entry.visibility = :protected
      else
        cmv = Rubinius::CompiledMethod::Visibility.new entry, :protected
        @method_table[name] = cmv
      end
    end
  end

  def alias_method(new_name, current_name)
    unless meth = @method_table[current_name]
      mod = direct_superclass()
      while !meth and mod
        meth = mod.method_table[current_name]
        mod = mod.direct_superclass
      end
    end

    unless meth
      raise NoMethodError, "No method '#{current_name}' to alias to '#{new_name}'"
    end
    @method_table[new_name] = meth
    Rubinius::VM.reset_method_cache(new_name)
  end

  def module_function(name)
    if cm = @method_table[name]
      if cm.kind_of? Rubinius::Tuple
        cm = cm[1]
      end
      meta = class << self; self; end
      meta.method_table[name] = cm
      private name
    end
  end
end

module Rubinius
  class IncludedModule < Module
    attr_reader :superclass
    attr_reader :module

    def self.allocate
      Ruby.primitive :included_module_allocate
      raise PrimitiveFailure, "IncludedModule.allocate primitive failed"
    end

    def initialize(mod)
      @method_table = mod.method_table
      @method_cache = nil
      @name = nil
      @constants = mod.constant_table
      @encloser = mod.encloser
      @module = mod
    end

    def attach_to(cls)
      @superclass = cls.direct_superclass
      cls.superclass = self
    end

    def name
      @module.name
    end

    def to_s
      @module.to_s
    end
  end
end

module Kernel
  alias_method :__class__, :class
end

class Object
  include Kernel

  # TODO - Improve this check for metaclass support
  def __verify_metaclass__
    if self.__kind_of__(Fixnum) or self.__kind_of__(Symbol)
      raise TypeError, "no virtual class for #{self.class}"
    end
  end
end
