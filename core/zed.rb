# TODO: Finish adding some layer structure to the core library. This file is a
# complete mess at the moment.
#
# A proper layered structure for the core library would enable and encourage
# composition and make visible where pieces are loosely or tightly coupled. A
# reasonable layer for something as complex as the Ruby core library would be:
#
# 1. Simple values like Fixnum, Bignum, Float;
# 2. Basic data structures like Tuple, ByteArray, Array, Hash;
# 3. More complicated values like String, Regexp, Complex, Rational;
# 4. Fairly simple system-dependent facilities like IO, File, Process;
# 5. Even more complex facilities like FFI;
# 6. Something like Socket that composes multiple lower layers;
# 7. A complex facility like CodeLoader or RubyGems.


# The virtual concatenation file of the files given on command line (or
# from $stdin if no files were given.) Usable like an IO.
ARGF = Rubinius::ARGFClass.new

class Module
  def alias_method(new_name, current_name)
    new_name = Rubinius::Type.coerce_to_symbol(new_name)
    current_name = Rubinius::Type.coerce_to_symbol(current_name)
    mod, entry = lookup_method(current_name, true, false)

    if entry
      Rubinius::VM.reset_method_cache self, new_name

      method_visibility = visibility_for_aliased_method(new_name, entry.visibility)

      # If we're aliasing a method we contain, just reference it directly, no
      # need for the alias wrapper
      #
      # The 'and entry.get_method' is there to force us to use the alias code
      # when the original method exists only to change the visibility of a
      # parent method.
      if mod == self and entry.get_method
        @method_table.store new_name, entry.method_id, entry.method,
          entry.scope, entry.serial, method_visibility
      else
        @method_table.alias new_name, method_visibility, current_name,
                            entry.get_method, mod
      end

      if ai = Rubinius::Type.singleton_class_object(self)
        Rubinius.privately do
          ai.singleton_method_added new_name
        end
      else
        method_added new_name
      end

      self
    else
      if ai = Rubinius::Type.singleton_class_object(self)
        raise NameError, "Unable to find '#{current_name}' for object #{ai.inspect}"
      else
        thing = Rubinius::Type.object_kind_of?(self, Class) ? "class" : "module"
        raise NameError, "undefined method `#{current_name}' for #{thing} `#{self.name}'"
      end
    end
  end
  private :alias_method

  def const_set(name, value)
    name = Rubinius::Type.coerce_to_constant_name name
    Rubinius.check_frozen

    if Rubinius::Type.object_kind_of? value, Module
      Rubinius::Type.set_module_name value, name, self

      if Object.equal? self
        value.constant_table.each do |n, c, _|
          if Rubinius::Type.object_kind_of? c, Module and !c.name
            Rubinius::Type.set_module_name c, n, value
          end
        end
      end
    end

    @constant_table.store(name, value, :public)
    Rubinius.inc_global_serial

    return value
  end

  def module_function(*args)
    if kind_of? Class
      raise TypeError, "invalid receiver class #{__class__}, expected Module"
    end

    if args.empty?
      vs = Rubinius::VariableScope.of_sender
      vs.method_visibility = :module
    else
      sc = Rubinius::Type.object_singleton_class(self)
      args.each do |meth|
        method_name = Rubinius::Type.coerce_to_symbol meth

        Rubinius::VM.reset_method_cache sc, method_name

        mod, entry = lookup_method(method_name)
        sc.method_table.store method_name, entry.method_id, entry.method,
          entry.scope, entry.serial, :public
        set_visibility method_name, :private
      end
    end

    return self
  end
  private :module_function

  def public(*args)
    if args.empty?
      vs = Rubinius::VariableScope.of_sender
      vs.method_visibility = nil
    else
      args.each { |meth| set_visibility(meth, :public) }
    end

    self
  end
  private :public

  def private(*args)
    if args.empty?
      vs = Rubinius::VariableScope.of_sender
      vs.method_visibility = :private
    else
      args.each { |meth| set_visibility(meth, :private) }
    end

    self
  end
  private :private

  def protected(*args)
    if args.empty?
      vs = Rubinius::VariableScope.of_sender
      vs.method_visibility = :protected
    else
      args.each { |meth| set_visibility(meth, :protected) }
    end

    self
  end
  private :protected

  def private_class_method(*args)
    args.each do |meth|
      set_class_visibility(meth, :private)
    end
    self
  end

  def public_class_method(*args)
    args.each do |meth|
      set_class_visibility(meth, :public)
    end
    self
  end

  # Invokes <code>Module#append_features</code> and
  # <code>Module#included</code> on each argument, passing in self.
  #
  def include(*modules)
    modules.reverse_each do |mod|
      if !mod.kind_of?(Module) or mod.kind_of?(Class)
        raise TypeError, "wrong argument type #{mod.class} (expected Module)"
      end

      Rubinius.privately do
        mod.append_features self
      end

      Rubinius.privately do
        mod.included self
      end
    end
    self
  end

  # Add all constants, instance methods and module variables
  # of this Module and all Modules that this one includes to +klass+
  #
  # This method is aliased as append_features as the default implementation
  # for that method. Kernel#extend calls this method directly through
  # Module#extend_object, because Kernel#extend should not use append_features.
  def include_into(klass)
    unless klass.kind_of? Module
      raise TypeError, "invalid argument class #{klass.class}, expected Module"
    end

    if kind_of? Class
      raise TypeError, "invalid receiver class #{__class__}, expected Module"
    end

    Rubinius::Type.include_modules_from(self, klass.origin)
    Rubinius::Type.infect(klass, self)

    self
  end

  # Called when this Module is being included in another Module.
  # This may be overridden for custom behaviour. The default
  # is to add constants, instance methods and module variables
  # of this Module and all Modules that this one includes to +klass+.
  #
  # See also #include.
  #
  alias_method :append_features, :include_into
  private :append_features

  def attr_reader(*names)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    names.each { |name| Rubinius.add_reader name, self, vis }

    return nil
  end
  private :attr_reader

  def attr_writer(*names)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    names.each { |name| Rubinius::add_writer name, self, vis }

    return nil
  end
  private :attr_writer

  def attr_accessor(*names)
    vis = Rubinius::VariableScope.of_sender.method_visibility

    names.each do |name|
      Rubinius.add_reader name, self, vis
      Rubinius.add_writer name, self, vis
    end

    return nil
  end
  private :attr_accessor

  def undef_method(*names)
    return self if names.empty?
    names.map!{ |name| Rubinius::Type.coerce_to_symbol name }
    Rubinius.check_frozen

    names.each do |name|
      # Will raise a NameError if the method doesn't exist.
      instance_method name
      undef_method! name
    end

    self
  end
  private :undef_method

  # Like undef_method, but doesn't even check that the method exists. Used
  # mainly to implement rb_undef_method.
  def undef_method!(name)
    Rubinius.check_frozen
    name = Rubinius::Type.coerce_to_symbol(name)

    Rubinius::VM.reset_method_cache self, name

    @method_table.store name, nil, nil, nil, 0, :undef

    if obj = Rubinius::Type.singleton_class_object(self)
      Rubinius.privately do
        obj.singleton_method_undefined(name)
      end
    else
      method_undefined(name)
    end

    name
  end

  def remove_method(*names)
    names.each do |name|
      name = Rubinius::Type.coerce_to_symbol(name)
      Rubinius.check_frozen

      unless @method_table.lookup(name)
        raise NameError.new("method `#{name}' not defined in #{self.name}", name)
      end

      Rubinius::VM.reset_method_cache self, name
      @method_table.delete name

      if obj = Rubinius::Type.singleton_class_object(self)
        Rubinius.privately do
          obj.singleton_method_removed(name)
        end
      else
        method_removed(name)
      end
    end

    self
  end
  private :remove_method

  def name
    Rubinius::Type.module_name self
  end

  def to_s
    Rubinius::Type.module_inspect self
  end

  def inspect
    to_s
  end
end

module Rubinius
  Config = Configuration.new
  Globals = GlobalVariables.new

  @add_method_hook = Rubinius::Hook.new

  def self.open_class(name, sup, scope)
    if scope
      under = scope.module
    else
      under = Object
    end

    open_class_under name, sup, under
  end

  def self.open_class_under(name, sup, mod)
    unless Type.object_kind_of? mod, Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constant_table
    if !tbl.has_name?(name)
      # Create the class
      sup = Object unless sup
      obj = Class.new sup, name, mod
    else
      entry = tbl.lookup(name)
      obj = entry.constant
      if Type.object_kind_of? obj, Autoload
        obj = begin
          obj.call(mod, true)
        rescue NameError # Constant not found in loaded file
          nil
        end

        # nil is returned if the autoload was abort, usually because
        # the file to be required has already been loaded. In which case
        # act like the autoload wasn't there.
        unless obj
          supr = sup ? sup : Object
          obj = Class.new supr, name, mod
        end
      end

      if Type.object_kind_of? obj, Class
        if sup and obj.superclass != sup
          raise TypeError, "Superclass mismatch: #{obj.superclass} != #{sup}"
        end
      else
        raise TypeError, "#{name} is not a class"
      end
    end

    return obj
  end

  def self.open_module(name, scope)
    if scope
      under = scope.module
    else
      under = Object
    end

    open_module_under name, under
  end

  def self.open_module_under(name, mod)
    unless Type.object_kind_of? mod, Module
      raise TypeError, "'#{mod.inspect}' is not a class/module"
    end

    tbl = mod.constant_table
    found = tbl.has_name?(name)

    # Object has special behavior, we check it's included
    # modules also
    if !found and mod.equal? Object
      check = mod.direct_superclass

      while check
        tbl = check.constant_table
        found = tbl.has_name?(name)
        break if found
        check = check.direct_superclass
      end
    end

    if !found
      # Create the module
      obj = Module.new
      mod.const_set name, obj
    else
      entry = tbl.lookup(name)
      obj = entry.constant

      if Type.object_kind_of? obj, Autoload
        obj = obj.call(mod, true)

        # See comment above about autoload returning nil
        unless obj
          obj = Module.new
          mod.const_set name, obj
        end
      end

      if Type.object_kind_of?(obj, Class) || !Type.object_kind_of?(obj, Module)
        raise TypeError, "#{name} is not a module, but a #{obj.class}"
      end
    end
    return obj
  end

  def self.add_function(name, executable, lexical_scope)
    scope = lexical_scope
    while scope.parent
      scope = scope.parent
    end

    unless scope.functions.kind_of? Rubinius::LookupTable
      scope.functions = Rubinius::LookupTable.new
    end

    scope.functions[name] = executable
  end

  def self.add_defn_method(name, executable, lexical_scope, vis)
    # TODO: puts serial on MethodTable entry
    unless Type.object_kind_of? executable, String
    executable.serial = 1
    if executable.respond_to? :scope=
      # If we're adding a method inside ane eval, dup it so that
      # we don't share the CompiledCode with the eval, since
      # we're going to mutate it.
      if lexical_scope and script = lexical_scope.current_script
        if script.eval?
          executable = executable.dup
        end
      end
      executable.scope = lexical_scope
    end
    end

    mod = lexical_scope.for_method_definition

    if ai = Type.singleton_class_object(mod)
      if Type.object_kind_of? ai, Numeric

        # Such a weird protocol. If :singleton_method_added exists, allow this.
        # le sigh.
        unless Type.object_respond_to? ai, :singleton_method_added
          raise TypeError, "Unable to define singleton methods on Numerics"
        end
      end
    end

    add_method name, executable, mod, lexical_scope, 1, vis
    name
  end

  # Must be AFTER add_method, because otherwise we'll run this attach_method to add
  # add_method itself and fail.
  def self.attach_method(name, executable, lexical_scope, recv)
    # TODO: puts serial on MethodTable entry
    unless Type.object_kind_of? executable, String
    executable.serial = 1
    if executable.respond_to? :scope=
      # If we're adding a method inside ane eval, dup it so that
      # we don't share the CompiledCode with the eval, since
      # we're going to mutate it.
      if lexical_scope and script = lexical_scope.current_script
        if script.eval?
          executable = executable.dup
        end
      end
      executable.scope = lexical_scope
    end
    end

    mod = Rubinius::Type.object_singleton_class recv

    add_method name, executable, mod, lexical_scope, 1, :public
    name
  end

  def self.add_reader(name, mod, vis)
    normalized = Rubinius::Type.coerce_to_symbol(name)
    add_method normalized, AccessVariable.get_ivar(normalized), mod, nil, 0, vis
  end

  def self.add_writer(name, mod, vis)
    normalized = Rubinius::Type.coerce_to_symbol(name)
    writer_name = "#{normalized}=".to_sym
    add_method writer_name, AccessVariable.set_ivar(normalized), mod, nil, 0, vis
  end

  class << self
    attr_reader :add_method_hook
  end

  module CAPI
    INTEGER_PACK_MSWORD_FIRST       = 0x01
    INTEGER_PACK_LSWORD_FIRST       = 0x02
    INTEGER_PACK_MSBYTE_FIRST       = 0x10
    INTEGER_PACK_LSBYTE_FIRST       = 0x20
    INTEGER_PACK_NATIVE_BYTE_ORDER  = 0x40
    INTEGER_PACK_2COMP              = 0x80
    INTEGER_PACK_FORCE_BIGNUM       = 0x100
    INTEGER_PACK_NEGATIVE           = 0x200

    INTEGER_PACK_LITTLE_ENDIAN  = INTEGER_PACK_LSWORD_FIRST|INTEGER_PACK_LSBYTE_FIRST
    INTEGER_PACK_BIG_ENDIAN     = INTEGER_PACK_MSWORD_FIRST|INTEGER_PACK_MSBYTE_FIRST

    INTEGER_PACK_DIGITS = [
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    ]
  end

  module CType
    Printed = Rubinius::Tuple.new 256
    i = 0
    while i < 256
      Printed[i] = toprint(i)
      i += 1
    end

    Lowered = Rubinius::Tuple.new 256

    i = 0
    while i < 256
      Lowered[i] = tolower(i).chr
      i += 1
    end
  end

  module FFI
    # Shorthand for the current process, i.e. all code that
    # the process image itself contains. In addition to the
    # Rubinius codebase, this also includes libc etc.
    #
    # Use this constant instead of nil directly.
    #
    CURRENT_PROCESS = nil

    TypeDefs = Rubinius::LookupTable.new

    # Converts a char
    add_typedef TYPE_CHAR,    :char

    # Converts an unsigned char
    add_typedef TYPE_UCHAR,   :uchar

    # The C++ boolean type
    add_typedef TYPE_BOOL,    :bool

    # Converts a short
    add_typedef TYPE_SHORT,   :short

    # Converts an unsigned short
    add_typedef TYPE_USHORT,  :ushort
    add_typedef TYPE_USHORT,  :mode_t
    add_typedef TYPE_USHORT,  :nlink_t

    # Converts an int
    add_typedef TYPE_INT,     :int
    add_typedef TYPE_INT,     :dev_t
    add_typedef TYPE_INT,     :blksize_t
    add_typedef TYPE_INT,     :time_t

    # Converts an unsigned int
    add_typedef TYPE_UINT,    :uint
    add_typedef TYPE_UINT,    :uid_t
    add_typedef TYPE_UINT,    :gid_t
    add_typedef TYPE_UINT,    :id_t

    # Converts a long
    add_typedef TYPE_LONG,    :long
    add_typedef TYPE_LONG,    :ssize_t

    # Converts an unsigned long
    add_typedef TYPE_ULONG,   :ulong

    # Converts a size_t
    add_typedef TYPE_ULONG,   :size_t

    # Converts a long long
    add_typedef TYPE_LL,      :long_long
    add_typedef TYPE_LL,      :blkcnt_t
    add_typedef TYPE_LL,      :off_t

    # Converts an unsigned long long
    add_typedef TYPE_ULL,     :ulong_long
    add_typedef TYPE_ULL,     :ino64_t

    # Converts a float
    add_typedef TYPE_FLOAT,   :float

    # Converts a double
    add_typedef TYPE_DOUBLE,  :double

    # Converts a pointer to opaque data
    add_typedef TYPE_PTR,     :pointer

    # For when a function has no return value
    add_typedef TYPE_VOID,    :void

    # Converts NULL-terminated C strings
    add_typedef TYPE_STRING,  :string

    # Use strptr when you need to free the result of some operation.
    add_typedef TYPE_STRPTR,  :strptr
    add_typedef TYPE_STRPTR,  :string_and_pointer

    # Use for a C struct with a char [] embedded inside.
    add_typedef TYPE_CHARARR, :char_array

    # A set of unambigious integer types
    add_typedef TYPE_CHAR,   :int8
    add_typedef TYPE_UCHAR,  :uint8
    add_typedef TYPE_SHORT,  :int16
    add_typedef TYPE_USHORT, :uint16
    add_typedef TYPE_INT,    :int32
    add_typedef TYPE_UINT,   :uint32

    # Converts a varargs argument
    add_typedef TYPE_VARARGS, :varargs

    if Rubinius::L64
      add_typedef TYPE_LONG,  :int64
      add_typedef TYPE_ULONG, :uint64
    else
      add_typedef TYPE_LL,    :int64
      add_typedef TYPE_ULL,   :uint64
    end

    TypeSizes = Rubinius::LookupTable.new
    TypeSizes[1] = :char
    TypeSizes[2] = :short
    TypeSizes[4] = :int
    TypeSizes[8] = Rubinius::L64 ? :long : :long_long

    # Load all the platform dependent types

    Rubinius::Config.section("rbx.platform.typedef.") do |key, value|
      add_typedef(find_type(value.to_sym), key.substring(21, key.length).to_sym)
    end

    class DynamicLibrary
      extend FFI::Library

      # Bootstrap dlsym, dlopen, and dlerror
      pointer_as_function :find_symbol, FFI::Pointer::DLSYM, [:pointer, :string], :pointer
      pointer_as_function :open_library, FFI::Pointer::DLOPEN, [:string, :int], :pointer
      pointer_as_function :last_error, FFI::Pointer::DLERROR, [], :string

      if Rubinius.windows?
        RTLD_LAZY   = 0
        RTLD_NOW    = 0
        RTLD_GLOBAL = 0
        RTLD_LOCAL  = 0
      else
        RTLD_LAZY   = Rubinius::Config['rbx.platform.dlopen.RTLD_LAZY']
        RTLD_NOW    = Rubinius::Config['rbx.platform.dlopen.RTLD_NOW']
        RTLD_GLOBAL = Rubinius::Config['rbx.platform.dlopen.RTLD_GLOBAL']
        RTLD_LOCAL  = Rubinius::Config['rbx.platform.dlopen.RTLD_LOCAL']
      end

      class << self
        alias_method :open, :new
      end

      def initialize(name, flags=nil)
        # Accept nil and check for ruby-ffi API compat
        flags ||= RTLD_LAZY | RTLD_GLOBAL

        unless name
          @name = "[current process]"
          @handle = FFI::Pointer::CURRENT_PROCESS
          return
        end

        @name = name
        @handle = DynamicLibrary.open_library name, flags

        unless @handle
          orig_error = last_error

          libnames = FFI::LIB_SUFFIXES.map {|suffix| "#{name}.#{suffix}" }
          libnames << "lib#{name}"
          libnames += FFI::LIB_SUFFIXES.map {|suffix| "lib#{name}.#{suffix}" }

          libnames.detect do |libname|
            @name = libname
            @handle = DynamicLibrary.open_library libname, flags
          end
        end

        unless @handle
          orig_error = orig_error.split("\n").first
          # API Compat. LoadError is wrong here.
          raise LoadError, "Could not open library #{name} - #{orig_error}"
        end
      end

      private :initialize

      attr_reader :name

      def find_symbol(name)
        ptr = DynamicLibrary.find_symbol @handle, name
        return nil unless ptr
        # defined in core/pointer.rb
        FFI::DynamicLibrary::Symbol.new(self, ptr, name)
      end

      alias_method :find_function, :find_symbol
      alias_method :find_variable, :find_symbol

      def last_error
        DynamicLibrary.last_error
      end

      CURRENT_PROCESS = DynamicLibrary.new(nil)
    end

    module Platform
      ARCH = Rubinius::CPU

      # ruby-ffi compatible
      LONG_SIZE = Rubinius::SIZEOF_LONG * 8
      ADDRESS_SIZE = Rubinius::WORDSIZE

      case
      when Rubinius.windows?
        LIBSUFFIX = "dll"
        IS_WINDOWS = true
        OS = 'windows'
      when Rubinius.darwin?
        LIBSUFFIX = "dylib"
        IS_WINDOWS = false
        OS = 'darwin'
      else
        LIBSUFFIX = "so"
        IS_WINDOWS = false
        OS = 'linux'
      end

      module File
        SEPARATOR = '/'
        ALT_SEPARATOR = nil
        PATH_SEPARATOR = ':'
      end

      module Math
        extend FFI::Library

        attach_function :fabs,  [:double], :double
        attach_function :atan2, [:double, :double], :double
        attach_function :cos,   [:double], :double
        attach_function :sin,   [:double], :double
        attach_function :tan,   [:double], :double
        attach_function :acos,  [:double], :double
        attach_function :asin,  [:double], :double
        attach_function :atan,  [:double], :double
        attach_function :cosh,  [:double], :double
        attach_function :sinh,  [:double], :double
        attach_function :tanh,  [:double], :double
        attach_function :acosh, [:double], :double
        attach_function :asinh, [:double], :double
        attach_function :atanh, [:double], :double
        attach_function :exp,   [:double], :double
        attach_function :log,   [:double], :double
        attach_function :log10, [:double], :double
        attach_function :sqrt,  [:double], :double
        attach_function :frexp, [:double, :pointer], :double
        attach_function :ldexp, [:double, :int], :double
        attach_function :hypot, [:double, :double], :double
        attach_function :erf,   [:double], :double
        attach_function :erfc,  [:double], :double
        attach_function :cbrt, [:double], :double
        attach_function :tgamma, [:double], :double
        attach_function :lgamma_r, [:double, :pointer], :double

        # Rubinius-specific, used in Marshal
        attach_function :modf,  [:double, :pointer], :double

        # TODO: we need a way to determine whether a function
        # is defined for a particular platform.
        #
        # attach_function :log2,  [:double], :double
        def self.log2(x)
          log10(x) / log10(2.0)
        end
      end

      module POSIX
        extend FFI::Library

        # errors
        attach_function :errno,    'ffi_errno',     [],     :int
        attach_function :errno=,   'ffi_set_errno', [:int], :void

        # memory
        attach_function :malloc,   [:size_t], :pointer
        attach_function :realloc,  [:pointer, :size_t], :pointer
        attach_function :free,     [:pointer], :void
        attach_function :memset,   [:pointer, :int, :size_t], :pointer
        attach_function :memcpy,   [:pointer, :pointer, :size_t], :pointer

        # file system
        attach_function :access,   [:string, :int], :int
        attach_function :chmod,    [:string, :mode_t], :int
        attach_function :fchmod,   [:int, :mode_t], :int
        attach_function :lchmod,   [:string, :mode_t], :int
        attach_function :chown,    [:string, :uid_t, :gid_t], :int
        attach_function :fchown,   [:int, :uid_t, :gid_t], :int
        attach_function :lchown,   [:string, :uid_t, :gid_t], :int
        attach_function :unlink,   [:string], :int
        attach_function :getcwd,   [:string, :size_t], :string
        attach_function :umask,    [:mode_t], :int
        attach_function :link,     [:string, :string], :int
        attach_function :symlink,  [:string, :string], :int
        attach_function :readlink, [:string, :pointer, :size_t], :ssize_t
        attach_function :rename,   [:string, :string], :int
        attach_function :utimes,   [:string, :pointer], :int
        attach_function :mkfifo,   [:string, :mode_t], :int

        # directories
        attach_function :chdir,     [:string], :int
        attach_function :mkdir,     [:string, :mode_t], :int
        attach_function :rmdir,     [:string], :int
        attach_function :chroot,    [:string], :int

        # File/IO
        attach_function :fcntl,       [:int, :int, :long], :int
        attach_function :ioctl,       [:int, :ulong, :long], :int
        attach_function :fsync,       [:int], :int
        attach_function :dup,         [:int], :int
        attach_function :dup2,        [:int, :int], :int
        attach_function :open,        [:string, :int, :mode_t], :int
        attach_function :close,       [:int], :int
        attach_function :lseek,       [:int, :off_t, :int], :off_t
        attach_function :read,        [:int, :pointer, :size_t], :ssize_t
        attach_function :ftruncate,   [:int, :off_t], :int
        attach_function :truncate,    [:string, :off_t], :int
        attach_function :write,       [:int, :pointer, :size_t], :ssize_t
        attach_function :select,      [:int, :pointer, :pointer, :pointer, :pointer], :int

        # Other I/O
        attach_function :pipe,        [:pointer], :int
        attach_function :mmap,        [:pointer, :size_t, :int, :int, :int, :off_t], :pointer
        attach_function :msync,       [:pointer, :size_t, :int], :int
        attach_function :munmap,      [:pointer, :size_t], :int
        attach_function :getpagesize, [], :int
        attach_function :shutdown,    [:int, :int], :int
        attach_function :posix_fadvise, [:int, :off_t, :off_t, :int], :int

        #   inspecting
        attach_function :isatty,   [:int], :int
        attach_function :ttyname,  [:int], :string

        #   locking
        attach_function :flock, [:int, :int], :int

        # UID/GID
        attach_function :getuid,  [], :uid_t
        attach_function :getgid,  [], :gid_t
        attach_function :geteuid, [], :uid_t
        attach_function :getegid, [], :gid_t

        attach_function :setgid,  [:gid_t], :int
        attach_function :setuid,  [:uid_t], :int
        attach_function :setegid, [:gid_t], :int
        attach_function :seteuid, [:uid_t], :int

        attach_function :setregid, [:gid_t, :gid_t], :int
        attach_function :setreuid, [:uid_t, :uid_t], :int

        attach_function :setresgid, [:gid_t, :gid_t, :gid_t], :int
        attach_function :setresuid, [:uid_t, :uid_t, :uid_t], :int

        attach_function :getpriority, [:int, :id_t], :int
        attach_function :setpriority, [:int, :id_t, :int], :int

        attach_function :getgroups,  [:int, :pointer], :int
        attach_function :setgroups,  [:int, :pointer], :int
        attach_function :initgroups, [:string, :gid_t], :int

        attach_function :setrlimit, [:int, :pointer], :int
        attach_function :getrlimit, [:int, :pointer], :int

        # password and group file access
        attach_function :getpwnam, [:string], :pointer
        attach_function :getpwuid, [:uid_t], :pointer
        attach_function :setpwent, [], :void
        attach_function :getpwent, [], :pointer
        attach_function :endpwent, [], :void

        attach_function :getgrnam, [:string], :pointer
        attach_function :getgrgid, [:gid_t], :pointer
        attach_function :setgrent, [], :void
        attach_function :getgrent, [], :pointer
        attach_function :endgrent, [], :void

        # processes and signals
        attach_function :kill,    [:pid_t, :int], :int
        attach_function :getpgid, [:pid_t], :pid_t
        attach_function :setpgid, [:pid_t, :pid_t], :int
        attach_function :getpid,  [], :pid_t
        attach_function :getppid, [], :pid_t
        attach_function :getpgrp, [], :pid_t
        attach_function :setsid,  [], :pid_t

        # related to stat()
        attach_function :major, 'ffi_major', [:dev_t], :dev_t
        attach_function :minor, 'ffi_minor', [:dev_t], :dev_t

        # time
        attach_function :gettimeofday, [:pointer, :pointer], :int

        #--
        # Internal class for accessing timevals
        #++
        class TimeVal < FFI::Struct
          config 'rbx.platform.timeval', :tv_sec, :tv_usec
        end
      end
    end
  end

  module EnvironmentAccess
    extend FFI::Library

    attach_function :getenv,   [:string], :string
    attach_function :putenv,   [:string], :int
    attach_function :setenv,   [:string,  :string, :int], :int
    attach_function :unsetenv, [:string], :int
    attach_function :environ,  'ffi_environ', [], :pointer
  end

  class EnvironmentVariables
    include EnvironmentAccess
  end

  ::ENV = EnvironmentVariables.new

  module Metrics
    class Data
      MetricsHash = Hash.new
    end
  end

  class Stat
    S_IRUSR  = Rubinius::Config['rbx.platform.file.S_IRUSR']
    S_IWUSR  = Rubinius::Config['rbx.platform.file.S_IWUSR']
    S_IXUSR  = Rubinius::Config['rbx.platform.file.S_IXUSR']
    S_IRGRP  = Rubinius::Config['rbx.platform.file.S_IRGRP']
    S_IWGRP  = Rubinius::Config['rbx.platform.file.S_IWGRP']
    S_IXGRP  = Rubinius::Config['rbx.platform.file.S_IXGRP']
    S_IROTH  = Rubinius::Config['rbx.platform.file.S_IROTH']
    S_IWOTH  = Rubinius::Config['rbx.platform.file.S_IWOTH']
    S_IXOTH  = Rubinius::Config['rbx.platform.file.S_IXOTH']

    S_IRUGO  = S_IRUSR | S_IRGRP | S_IROTH
    S_IWUGO  = S_IWUSR | S_IWGRP | S_IWOTH
    S_IXUGO  = S_IXUSR | S_IXGRP | S_IXOTH

    S_IFMT   = Rubinius::Config['rbx.platform.file.S_IFMT']
    S_IFIFO  = Rubinius::Config['rbx.platform.file.S_IFIFO']
    S_IFCHR  = Rubinius::Config['rbx.platform.file.S_IFCHR']
    S_IFDIR  = Rubinius::Config['rbx.platform.file.S_IFDIR']
    S_IFBLK  = Rubinius::Config['rbx.platform.file.S_IFBLK']
    S_IFREG  = Rubinius::Config['rbx.platform.file.S_IFREG']
    S_IFLNK  = Rubinius::Config['rbx.platform.file.S_IFLNK']
    S_IFSOCK = Rubinius::Config['rbx.platform.file.S_IFSOCK']
    S_IFWHT  = Rubinius::Config['rbx.platform.file.S_IFWHT']
    S_ISUID  = Rubinius::Config['rbx.platform.file.S_ISUID']
    S_ISGID  = Rubinius::Config['rbx.platform.file.S_ISGID']
    S_ISVTX  = Rubinius::Config['rbx.platform.file.S_ISVTX']
  end
end

code = Class.method_table.lookup(:new).get_method
code.serial = Rubinius::CompiledCode::KernelMethodSerial

class Class
  undef_method :append_features
  undef_method :extend_object
  undef_method :module_function
  undef_method :prepend_features
end

class Complex < Numeric
  I = Complex(0, 1)
end

class NilClass
  include ImmediateValue
end

class FalseClass
  include ImmediateValue
end

class TrueClass
  include ImmediateValue
end

class Symbol
  include ImmediateValue
end

class Fixnum
  include ImmediateValue
end

# Default Ruby Record Separator
# Used in this file and by various methods that need to ignore $/
DEFAULT_RECORD_SEPARATOR = "\n"

class Dir
  # This seems silly, I know. But we do this to make Dir more resistent to people
  # screwing with ::File later (ie, fakefs)
  PrivateFile = ::File

  FFI = Rubinius::FFI

  module Glob
    total = Rubinius::Config['glob.cache']

    case total
    when Fixnum
      if total == 0
        @glob_cache = nil
      else
        @glob_cache = Rubinius::LRUCache.new(total)
      end
    when false
      @glob_cache = nil
    else
      @glob_cache = Rubinius::LRUCache.new(50)
    end
  end
end

class Encoding
  class Converter
    class TranscodingPath
      @paths = {}
      @load_cache = true
      @cache_valid = false
      @transcoders_count = TranscodingMap.size
    end
  end
end

module Errno
  FFI = Rubinius::FFI
end

class File < IO
  # these will be necessary when we run on Windows
  DOSISH = false # !!(RUBY_PLATFORM =~ /mswin/)
  CASEFOLD_FILESYSTEM = DOSISH
  FNM_SYSCASE = CASEFOLD_FILESYSTEM ? FNM_CASEFOLD : 0

  module Constants
    F_GETFL  = Rubinius::Config['rbx.platform.fcntl.F_GETFL']
    F_SETFL  = Rubinius::Config['rbx.platform.fcntl.F_SETFL']

    # O_ACCMODE is /undocumented/ for fcntl() on some platforms
    ACCMODE  = Rubinius::Config['rbx.platform.fcntl.O_ACCMODE']
    O_ACCMODE = Rubinius::Config['rbx.platform.fcntl.O_ACCMODE']

    F_GETFD  = Rubinius::Config['rbx.platform.fcntl.F_GETFD']
    F_SETFD  = Rubinius::Config['rbx.platform.fcntl.F_SETFD']
    FD_CLOEXEC = Rubinius::Config['rbx.platform.fcntl.FD_CLOEXEC']
    O_CLOEXEC = Rubinius::Config['rbx.platform.file.O_CLOEXEC']
    O_NONBLOCK = Rubinius::Config['rbx.platform.file.O_NONBLOCK']

    RDONLY   = Rubinius::Config['rbx.platform.file.O_RDONLY']
    WRONLY   = Rubinius::Config['rbx.platform.file.O_WRONLY']
    RDWR     = Rubinius::Config['rbx.platform.file.O_RDWR']

    CREAT    = Rubinius::Config['rbx.platform.file.O_CREAT']
    EXCL     = Rubinius::Config['rbx.platform.file.O_EXCL']
    NOCTTY   = Rubinius::Config['rbx.platform.file.O_NOCTTY']
    TRUNC    = Rubinius::Config['rbx.platform.file.O_TRUNC']
    APPEND   = Rubinius::Config['rbx.platform.file.O_APPEND']
    NONBLOCK = Rubinius::Config['rbx.platform.file.O_NONBLOCK']
    SYNC     = Rubinius::Config['rbx.platform.file.O_SYNC']

    # TODO: these flags should probably be imported from Platform
    LOCK_SH  = 0x01
    LOCK_EX  = 0x02
    LOCK_NB  = 0x04
    LOCK_UN  = 0x08
    BINARY   = 0x04

    # TODO: "OK" constants aren't in File::Constants in MRI
    F_OK = 0 # test for existence of file
    X_OK = 1 # test for execute or search permission
    W_OK = 2 # test for write permission
    R_OK = 4 # test for read permission

    FNM_NOESCAPE = 0x01
    FNM_PATHNAME = 0x02
    FNM_DOTMATCH = 0x04
    FNM_CASEFOLD = 0x08
    FNM_EXTGLOB  = 0x10
    FNM_SYSCASE  = File::FNM_SYSCASE

    if Rubinius.windows?
      NULL = 'NUL'
    else
      NULL = '/dev/null'
    end
  end

  Stat = Rubinius::Stat

  class Stat
    @module_name = :"File::Stat"

    def world_readable?
      if mode & S_IROTH == S_IROTH
        tmp = mode & (S_IRUGO | S_IWUGO | S_IXUGO)
        return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
      end
    end

    def world_writable?
      if mode & S_IWOTH == S_IWOTH
        tmp = mode & (S_IRUGO | S_IWUGO | S_IXUGO)
        return Rubinius::Type.coerce_to tmp, Fixnum, :to_int
      end
    end
  end

  FFI = Rubinius::FFI

  SEPARATOR = FFI::Platform::File::SEPARATOR
  Separator = FFI::Platform::File::SEPARATOR
  ALT_SEPARATOR = FFI::Platform::File::ALT_SEPARATOR
  PATH_SEPARATOR = FFI::Platform::File::PATH_SEPARATOR
  POSIX = FFI::Platform::POSIX
end

class Float
  FFI = Rubinius::FFI
end

class IO
  include File::Constants

  FFI = Rubinius::FFI

  SEEK_SET = Rubinius::Config['rbx.platform.io.SEEK_SET']
  SEEK_CUR = Rubinius::Config['rbx.platform.io.SEEK_CUR']
  SEEK_END = Rubinius::Config['rbx.platform.io.SEEK_END']

  # Not available on all platforms, so these constants may be nil
  POSIX_FADV_NORMAL     = Rubinius::Config['rbx.platform.advise.POSIX_FADV_NORMAL']
  POSIX_FADV_SEQUENTIAL = Rubinius::Config['rbx.platform.advise.POSIX_FADV_SEQUENTIAL']
  POSIX_FADV_RANDOM     = Rubinius::Config['rbx.platform.advise.POSIX_FADV_RANDOM']
  POSIX_FADV_WILLNEED   = Rubinius::Config['rbx.platform.advise.POSIX_FADV_WILLNEED']
  POSIX_FADV_DONTNEED   = Rubinius::Config['rbx.platform.advise.POSIX_FADV_DONTNEED']
  POSIX_FADV_NOREUSE    = Rubinius::Config['rbx.platform.advise.POSIX_FADV_NOREUSE']

  class FileDescriptor
    @@max_descriptors = Rubinius::AtomicReference.new(2)

    include File::Constants

    O_RDONLY   = Rubinius::Config['rbx.platform.file.O_RDONLY']
    O_WRONLY   = Rubinius::Config['rbx.platform.file.O_WRONLY']
    O_RDWR     = Rubinius::Config['rbx.platform.file.O_RDWR']
  end

  class Select
    FD_SETSIZE = Rubinius::Config['rbx.platform.select.FD_SETSIZE']
  end
end

class NilClass
  include ImmediateValue
end

class FalseClass
  include ImmediateValue
end

class TrueClass
  include ImmediateValue
end

class Symbol
  include ImmediateValue
end

class Fixnum
  include ImmediateValue
end

module Rubinius
  class Mirror
    module Process
      class Execute
        # Mapping of string open modes to integer oflag versions.
        OFLAGS = {
          "r"  => ::File::RDONLY,
          "r+" => ::File::RDWR   | ::File::CREAT,
          "w"  => ::File::WRONLY | ::File::CREAT  | ::File::TRUNC,
          "w+" => ::File::RDWR   | ::File::CREAT  | ::File::TRUNC,
          "a"  => ::File::WRONLY | ::File::APPEND | ::File::CREAT,
          "a+" => ::File::RDWR   | ::File::APPEND | ::File::CREAT
        }
      end
    end
  end
end

class Regexp
  IGNORECASE         = 1
  EXTENDED           = 2
  MULTILINE          = 4
  FIXEDENCODING      = 16
  NOENCODING         = 32
  DONT_CAPTURE_GROUP = 128
  CAPTURE_GROUP      = 256

  KCODE_NONE = (1 << 9)
  KCODE_EUC  = (2 << 9)
  KCODE_SJIS = (3 << 9)
  KCODE_UTF8 = (4 << 9)
  KCODE_MASK = KCODE_NONE | KCODE_EUC | KCODE_SJIS | KCODE_UTF8

  OPTION_MASK = IGNORECASE | EXTENDED | MULTILINE | FIXEDENCODING | NOENCODING | DONT_CAPTURE_GROUP | CAPTURE_GROUP

  ESCAPE_TABLE = Rubinius::Tuple.new(256)

  # Seed it with direct replacements
  i = 0
  while i < 256
    ESCAPE_TABLE[i] = i.chr
    i += 1
  end

  # The character literals (?x) are Fixnums in 1.8 and Strings in 1.9
  # so we use literal values instead so this is 1.8/1.9 compatible.
  ESCAPE_TABLE[9]   = '\\t'
  ESCAPE_TABLE[10]  = '\\n'
  ESCAPE_TABLE[11]  = '\\v'
  ESCAPE_TABLE[12]  = '\\f'
  ESCAPE_TABLE[13]  = '\\r'
  ESCAPE_TABLE[32]  = '\\ '
  ESCAPE_TABLE[35]  = '\\#'
  ESCAPE_TABLE[36]  = '\\$'
  ESCAPE_TABLE[40]  = '\\('
  ESCAPE_TABLE[41]  = '\\)'
  ESCAPE_TABLE[42]  = '\\*'
  ESCAPE_TABLE[43]  = '\\+'
  ESCAPE_TABLE[45]  = '\\-'
  ESCAPE_TABLE[46]  = '\\.'
  ESCAPE_TABLE[63]  = '\\?'
  ESCAPE_TABLE[91]  = '\\['
  ESCAPE_TABLE[92]  = '\\\\'
  ESCAPE_TABLE[93]  = '\\]'
  ESCAPE_TABLE[94]  = '\\^'
  ESCAPE_TABLE[123] = '\\{'
  ESCAPE_TABLE[124] = '\\|'
  ESCAPE_TABLE[125] = '\\}'

  class SourceParser
    class Part
      OPTIONS_MAP = {
        'm' => Regexp::MULTILINE,
        'i' => Regexp::IGNORECASE,
        'x' => Regexp::EXTENDED
      }
    end

    PossibleOptions = [[MULTILINE, "m"], [IGNORECASE, "i"], [EXTENDED, "x"]]
  end
end

class String
  class Rationalizer
    SPACE = "\\s*"
    DIGITS = "(?:[0-9](?:_[0-9]|[0-9])*)"
    NUMERATOR = "(?:#{DIGITS}?\\.)?#{DIGITS}(?:[eE][-+]?#{DIGITS})?"
    DENOMINATOR = DIGITS
    RATIONAL = "\\A#{SPACE}([-+])?(#{NUMERATOR})(?:\\/(#{DENOMINATOR}))?#{SPACE}"
    PATTERN = Regexp.new RATIONAL
  end

  class Complexifier
    SPACE = Rationalizer::SPACE
    NUMERATOR = Rationalizer::NUMERATOR
    DENOMINATOR = Rationalizer::DENOMINATOR
    NUMBER = "[-+]?#{NUMERATOR}(?:\\/#{DENOMINATOR})?"
    NUMBERNOS = "#{NUMERATOR}(?:\\/#{DENOMINATOR})?"
    PATTERN0 = Regexp.new "\\A#{SPACE}(#{NUMBER})@(#{NUMBER})#{SPACE}"
    PATTERN1 = Regexp.new "\\A#{SPACE}([-+])?(#{NUMBER})?[iIjJ]#{SPACE}"
    PATTERN2 = Regexp.new "\\A#{SPACE}(#{NUMBER})(([-+])(#{NUMBERNOS})?[iIjJ])?#{SPACE}"
  end
end

module Signal
  Names = {
    "EXIT" => 0
  }

  Numbers = {
    0 => "EXIT"
  }

  @handlers = {}

  Rubinius::Config.section 'rbx.platform.signal.' do |key, value|
    name = key[23, key.length]
    number = value.to_i
    Names[name] = number
    Numbers[number] = name
  end

  # special case of signal.c
  if Names["CHLD"]
    Names["CLD"] = Names["CHLD"]
  end
end

class Integer < Numeric
  alias_method :magnitude, :abs
end

module Kernel
  def extend(*modules)
    raise ArgumentError, "wrong number of arguments (0 for 1+)" if modules.empty?
    Rubinius.check_frozen

    modules.reverse_each do |mod|
      Rubinius.privately do
        mod.extend_object self
      end

      Rubinius.privately do
        mod.extended self
      end
    end
    self
  end

  alias_method :__extend__, :extend

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
      msg << " on nil:NilClass"

      if $rbx_show_nil_location
        m = Rubinius::Mirror::Object.reflect self
        code_id = m.nil_code_id

        # TODO: implement in CodeDB
        code = nil

        ObjectSpace.each_object(Rubinius::CompiledCode).each do |c|
          code = c if c.code_id.start_with? code_id
        end


        if code
          ip = m.nil_ip
          msg << " originating in '#{code.name}' at #{code.file}:#{code.line_from_ip(ip)}+#{ip}"
        end
      end

      msg << "."
    elsif ImmediateValue === self
      msg << " on #{self}:#{object_class}."
    else
      msg << " on an instance of #{object_class}."
    end

    Kernel.raise(cls.new(msg, meth, args, receiver: self))
  end

  private :method_missing

  def raise(exc=undefined, msg=undefined, ctx=nil, cause: nil)
    if undefined.equal? exc
      exc = $!
      exc = RuntimeError.new("No current exception") unless exc
      exc.cause = cause unless exc.cause or exc == cause
    else
      if exc.respond_to? :exception
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

      exc.cause = cause unless exc.cause or exc == cause
      exc.cause = $! unless exc.cause or exc == $!
    end

    if ctx.kind_of? Array
      exc.set_backtrace ctx
    else
      exc.capture_backtrace!(2) unless exc.backtrace?
    end

    if $DEBUG and $VERBOSE != nil
      if loc = exc.locations and loc[1]
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

  def to_s
    Rubinius::Type.infect("#<#{self.class}:0x#{self.__id__.to_s(16)}>", self)
  end

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

  get = proc do
    warn "$KCODE is unused in Ruby 1.9 and later"
    nil
  end

  set = proc do |key, io|
    warn "$KCODE is unused in Ruby 1.9 and later, changes are ignored"
    nil
  end

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

module Math
  FFI = Rubinius::FFI

  # Constants
  PI = 3.14159_26535_89793_23846
  E  = 2.71828_18284_59045_23536

  FactorialTable = [
    1.0,
    1.0,
    2.0,
    6.0,
    24.0,
    120.0,
    720.0,
    5040.0,
    40320.0,
    362880.0,
    3628800.0,
    39916800.0,
    479001600.0,
    6227020800.0,
    87178291200.0,
    1307674368000.0,
    20922789888000.0,
    355687428096000.0,
    6402373705728000.0,
    121645100408832000.0,
    2432902008176640000.0,
    51090942171709440000.0,
    1124000727777607680000.0
  ]

  if Errno.const_defined? :EDOM
    DomainError = Errno::EDOM
  elsif Errno.const_defined? :ERANGE
    DomainError = Errno::ERANGE
  else
    class DomainError < SystemCallError
    end
  end
end

module Process
  module Constants
    EXIT_SUCCESS = Rubinius::Config['rbx.platform.process.EXIT_SUCCESS'] || 0
    EXIT_FAILURE = Rubinius::Config['rbx.platform.process.EXIT_FAILURE'] || 1

    PRIO_PGRP    = Rubinius::Config['rbx.platform.process.PRIO_PGRP']
    PRIO_PROCESS = Rubinius::Config['rbx.platform.process.PRIO_PROCESS']
    PRIO_USER    = Rubinius::Config['rbx.platform.process.PRIO_USER']

    RLIM_INFINITY  = Rubinius::Config['rbx.platform.process.RLIM_INFINITY']
    RLIM_SAVED_MAX = Rubinius::Config['rbx.platform.process.RLIM_SAVED_MAX']
    RLIM_SAVED_CUR = Rubinius::Config['rbx.platform.process.RLIM_SAVED_CUR']

    RLIMIT_AS      = Rubinius::Config['rbx.platform.process.RLIMIT_AS']
    RLIMIT_CORE    = Rubinius::Config['rbx.platform.process.RLIMIT_CORE']
    RLIMIT_CPU     = Rubinius::Config['rbx.platform.process.RLIMIT_CPU']
    RLIMIT_DATA    = Rubinius::Config['rbx.platform.process.RLIMIT_DATA']
    RLIMIT_FSIZE   = Rubinius::Config['rbx.platform.process.RLIMIT_FSIZE']
    RLIMIT_MEMLOCK = Rubinius::Config['rbx.platform.process.RLIMIT_MEMLOCK']
    RLIMIT_NOFILE  = Rubinius::Config['rbx.platform.process.RLIMIT_NOFILE']
    RLIMIT_NPROC   = Rubinius::Config['rbx.platform.process.RLIMIT_NPROC']
    RLIMIT_RSS     = Rubinius::Config['rbx.platform.process.RLIMIT_RSS']
    RLIMIT_SBSIZE  = Rubinius::Config['rbx.platform.process.RLIMIT_SBSIZE']
    RLIMIT_STACK   = Rubinius::Config['rbx.platform.process.RLIMIT_STACK']

    RLIMIT_RTPRIO     = Rubinius::Config['rbx.platform.process.RLIMIT_RTPRIO']
    RLIMIT_RTTIME     = Rubinius::Config['rbx.platform.process.RLIMIT_RTTIME']
    RLIMIT_SIGPENDING = Rubinius::Config['rbx.platform.process.RLIMIT_SIGPENDING']
    RLIMIT_MSGQUEUE   = Rubinius::Config['rbx.platform.process.RLIMIT_MSGQUEUE']
    RLIMIT_NICE       = Rubinius::Config['rbx.platform.process.RLIMIT_NICE']

    WNOHANG = 1
    WUNTRACED = 2
  end
  include Constants

  FFI = Rubinius::FFI

  class Rlimit < FFI::Struct
    config "rbx.platform.rlimit", :rlim_cur, :rlim_max
  end

  Rubinius::Globals.read_only :$?
  Rubinius::Globals.set_hook(:$?) { Thread.current[:$?] }
end


STDIN  = Rubinius::IOUtility.redefine_io(0, :read_only)
STDOUT = Rubinius::IOUtility.redefine_io(1, :write_only)
STDERR = Rubinius::IOUtility.redefine_io(2, :write_only)

Rubinius::Globals.set!(:$stdin, STDIN)
Rubinius::Globals.set!(:$stdout, STDOUT)
Rubinius::Globals.set!(:$stderr, STDERR)

module Rubinius
  begin
    is_tty = STDIN.tty?
  rescue IOError
    is_tty = false
  end

  Terminal = is_tty
  AtExit = []

  class Sprinter
    total = Rubinius::Config['printf.cache']

    case total
    when Fixnum
      if total == 0
        @cache = nil
      else
        @cache = Rubinius::LRUCache.new(total)
      end
    when false
      @cache = nil
    else
      @cache = Rubinius::LRUCache.new(50)
    end

    class Builder
      RADIX = { 'u' => 10, 'x' => 16, 'X' => 16, 'o' => 8, 'b' => 2, 'B' => 2 }
      PREFIX = { 'o' => '0', 'x' => '0x', 'X' => '0X', 'b' => '0b', 'B' => '0B' }

      AtomMap = Rubinius::LookupTable.new

      AtomMap[?c] = CharAtom
      AtomMap[?s] = StringAtom
      AtomMap[?p] = InspectAtom
      AtomMap[?e] = FloatAtom
      AtomMap[?E] = FloatAtom
      AtomMap[?g] = FloatAtom
      AtomMap[?G] = FloatAtom
      AtomMap[?f] = FloatAtom
      AtomMap[?d] = IntegerAtom
      AtomMap[?i] = IntegerAtom
      AtomMap[?u] = IntegerAtom
      AtomMap[?b] = ExtIntegerAtom
      AtomMap[?B] = ExtIntegerAtom
      AtomMap[?x] = ExtIntegerAtom
      AtomMap[?X] = ExtIntegerAtom
      AtomMap[?o] = ExtIntegerAtom

      RE = /
        ([^%]+|%(?:[\n\0]|\z)) # 1
        |
        %
        ( # 2
          (<[^>]+>)? # 3
          ([0# +-]*) # 4
          (?:([0-9]+)\$)? # 5
          ([0# +-]*) # 6
          (?:
            (\*(?:([0-9]+)\$)?|([1-9][0-9]*))? # 7 8 9
            (?:\.(\*(?:([0-9]+)\$)?|([0-9][0-9]*))?)? # 10 11 12
          )
          (?:([0-9]+)\$)? # 13
          ([BbcdEefGgiopsuXx]) # 14
        )
        |
        (%)(?:%|[-+0-9# *.$]+\$[0-9.]*\z) # 15
        |
        (%{[^}]+}) #16
        |
        (%) # 17
      /x
    end
  end
end

class Struct
  Struct.new 'Tms', :utime, :stime, :cutime, :cstime, :tutime, :tstime

  class Tms
    def initialize(utime=nil, stime=nil, cutime=nil, cstime=nil,
                   tutime=nil, tstime=nil)
      @utime = utime
      @stime = stime
      @cutime = cutime
      @cstime = cstime
      @tutime = tutime
      @tstime = tstime
    end

    private :initialize
  end

  def self._specialize(attrs)
    # Because people are crazy, they subclass Struct directly, ie.
    #  class Craptastic < Struct
    #
    # NOT
    #
    #  class Fine < Struct.new(:x, :y)
    #
    # When they do this craptastic act, they'll sometimes define their
    # own #initialize and super into Struct#initialize.
    #
    # When they do this and then do Craptastic.new(:x, :y), this code
    # will accidentally shadow their #initialize. So for now, only run
    # the specialize if we're trying new Struct's directly from Struct itself,
    # not a craptastic Struct subclass.

    return unless superclass.equal? Struct

    # To allow for optimization, we generate code with normal ivar
    # references for all attributes whose names can be written as
    # tIVAR tokens. For example, of the following struct attributes
    #
    #   Struct.new(:a, :@b, :c?, :'d-e')
    #
    # only the first, :a, can be written as a valid tIVAR token:
    #
    #   * :a can be written as @a
    #   * :@b becomes @@b and would be interpreted as a tCVAR
    #   * :c? becomes @c? and be interpreted as the beginning of
    #     a ternary expression
    #   * :'d-e' becomes @d-e and would be interpreted as a method
    #     invocation
    #
    # Attribute names that cannot be written as tIVAR tokens will
    # fall back to using #instance_variable_(get|set).

    args, assigns, hashes, vars = [], [], [], []

    attrs.each_with_index do |name, i|
      name = "@#{name}"

      if name =~ /^@[a-z_]\w*$/i
        assigns << "#{name} = a#{i}"
        vars    << name
      else
        assigns << "instance_variable_set(:#{name.inspect}, a#{i})"
        vars    << "instance_variable_get(:#{name.inspect})"
      end

      args   << "a#{i} = nil"
      hashes << "#{vars[-1]}.hash"
    end

    code = <<-CODE
      def initialize(#{args.join(", ")})
        #{assigns.join(';')}
        self
      end

      private :initialize

      def hash
        hash = #{hashes.size}

        return hash if Thread.detect_outermost_recursion(self) do
          hash = hash ^ #{hashes.join(' ^ ')}
        end

        hash
      end

      def to_a
        [#{vars.join(', ')}]
      end

      def length
        #{vars.size}
      end
    CODE

    begin
      mod = Module.new do
        module_eval code
      end
      include mod
    rescue SyntaxError
      # SyntaxError means that something is wrong with the
      # specialization code. Just eat the error and don't specialize.
    end
  end
end

class Thread
  MUTEX_FOR_THREAD_EXCLUSIVE = Mutex.new

  Thread.initialize_main_thread(Thread.current)
  ThreadGroup::Default.add Thread.current

  sm = Rubinius::Mirror.reflect Rubinius.signal_thread
  sm.group = ThreadGroup::Default
end

class Time
  MonthValue = {
    'JAN' => 1, 'FEB' => 2, 'MAR' => 3, 'APR' => 4, 'MAY' => 5, 'JUN' => 6,
    'JUL' => 7, 'AUG' => 8, 'SEP' => 9, 'OCT' =>10, 'NOV' =>11, 'DEC' =>12
  }
end

class Random
  DEFAULT = new
end

module Rubinius
  class Prediction
    def valid?
      Rubinius.primitive :prediction_valid_p
      raise PrimitiveFailure, "Rubinius::Prediction#valid? primitive failed"
    end
  end

  class CodeDB
    class << self
      undef :new
    end
  end

  module FFI
    module Library
      # Once the core library is loaded, we want to raise an error if
      # attempting to attach to a non-existent function.
      def ffi_function_missing(name, *args)
        raise FFI::NotFoundError, "Unable to find foreign function '#{name}'"
      end
    end
  end
end

