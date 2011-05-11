class Rubinius::NativeFunction
  attr_accessor :return_type
  attr_accessor :argument_types
end

module FFI
  def self.generate_function(ptr, name, args, ret)
    Ruby.primitive :nativefunction_generate
    raise PrimitiveFailure, "FFI.generate_function failed"
  end

  def self.generate_trampoline(obj, name, args, ret)
    Ruby.primitive :nativefunction_generate_tramp
    raise PrimitiveFailure, "FFI.generate_function_tramp failed"
  end

  module Library

    case Rubinius::OS_TYPE
    when :windows
      LIBC = "msvcrt.dll"
    when :darwin
      LIBC = "libc.dylib"
    else
      LIBC = "libc#{Rubinius::LIBSUFFIX}.6"
    end

    # Set which library or libraries +attach_function+ should
    # look in. By default it only searches for the function in
    # the current process. If you want to specify this as one
    # of the locations, add FFI::USE_THIS_PROCESS_AS_LIBRARY.
    # The libraries are tried in the order given.
    #
    def ffi_lib(*names)
      @ffi_lib = names.map do |x|
        if x == FFI::CURRENT_PROCESS
          DynamicLibrary::CURRENT_PROCESS

          # When the element is an array, it's an ordered choice,
          # ie, pick the first library that works.
        elsif x.kind_of? Array
          lib = nil

          x.each do |name|
            begin
              lib = DynamicLibrary.new(name)
              break
            rescue LoadError
            end
          end

          # If .new worked, then lib is set and we can use it.
          unless lib
            raise LoadError, "Unable to find libary among: #{x.inspect}"
          end

          lib
        else
          DynamicLibrary.new(x)
        end
      end
    end

    def ffi_libraries
      @ffi_lib or [DynamicLibrary::CURRENT_PROCESS]
    end
    private :ffi_libraries

    # Attach a C function to this module. The arguments can have two forms:
    #
    #   attach_function c_name, [c_arg1, c_arg2], ret
    #   attach_function mod_name, c_name, [c_arg1, c_arg2], ret
    #
    # In the first form, +c_name+ will also be used for the name of the module
    # method. In the second form, the module method name is +mod_name+.
    #
    # The +c_name+ and +mod_name+ can be given as Strings or Symbols.
    #
    # The types of the arguments to the C function, +c_arg1+, +c_arg2+, etc, are
    # given as an array even if there is only one.
    #
    # The final argument, +ret+, is the type of the return value from the C
    # function.
    def attach_function(name, a3, a4, a5=nil)
      if a5
        cname = a3.to_s
        if a3.kind_of? Pointer
          cname = a3
          int_name = name.to_sym
        else
          cname = a3.to_s
          int_name = cname.to_sym
        end

        args = a4
        ret = a5
      else
        cname = name.to_s
        int_name = cname.to_sym
        args = a3
        ret = a4
      end

      mname = name.to_sym

      ffi_libraries.each do |lib|
        if ptr = lib.find_symbol(cname)
          return pointer_as_function(mname, ptr, args, ret)
        end
      end

      raise FFI::NotFoundError, "Unable to find '#{cname}'"
    end

    def pointer_as_function(name, ptr, args, ret)
      args.map! { |a| find_type a }

      if func = FFI.generate_function(ptr, name.to_sym, args, find_type(ret))

        # Make it available as a method callable directly..
        sc = Rubinius::Type.object_singleton_class(self)
        sc.method_table.store name, func, :public

        # and expose it as a private method for people who
        # want to include this module.
        method_table.store name, func, :private

        return func
      end

      raise FFI::NotFoundError, "Unable to attach pointer"
    end

    def callback(a1, a2, a3=nil)
      if a3
        name, params, ret =  a1, a2, a3
      else
        name, params, ret = nil, a1, a2
      end

      args = params.map { |x| find_type(x) }

      func, ptr = FFI.generate_trampoline nil, :ffi_tramp,
                                          args, find_type(ret)

      func.argument_types = params
      func.return_type = ret

      if name
        @ffi_callbacks ||= {}
        @ffi_callbacks[name] = func

        typedef func, name
      end

      return func
    end

    def typedef(old, add)
      @typedefs ||= Rubinius::LookupTable.new

      unless old.kind_of? Rubinius::NativeFunction
        old = find_type(old)
      end

      @typedefs[add] = old
    end

    def find_type(name)
      @typedefs ||= Rubinius::LookupTable.new

      if type = @typedefs[name]
        return type
      end

      if name.kind_of?(Class) and name < FFI::Struct
        name = :pointer
      end

      FFI.find_type(name)
    end
  end

  class DynamicLibrary
    extend FFI::Library

    # Bootstrap dlsym, dlopen, and dlerror
    pointer_as_function :find_symbol, FFI::Pointer::DLSYM, [:pointer, :string], :pointer
    pointer_as_function :open_library, FFI::Pointer::DLOPEN, [:string, :int], :pointer
    pointer_as_function :last_error, FFI::Pointer::DLERROR, [], :string

    RTLD_LAZY   = Rubinius::Config['rbx.platform.dlopen.RTLD_LAZY']
    RTLD_NOW    = Rubinius::Config['rbx.platform.dlopen.RTLD_NOW']
    RTLD_GLOBAL = Rubinius::Config['rbx.platform.dlopen.RTLD_GLOBAL']
    RTLD_LOCAL  = Rubinius::Config['rbx.platform.dlopen.RTLD_LOCAL']

    class << self
      alias_method :open, :new
    end

    def initialize(name, flags=nil)
      # Accept nil and check for ruby-ffi API compat
      flags ||= RTLD_LAZY | RTLD_GLOBAL

      if name
        @name = name
        @handle = DynamicLibrary.open_library name, flags

        unless @handle
          orig_error = last_error

          # Try with suffixes
          FFI::LIB_SUFFIXES.detect do |suffix|
            @name = "#{name}.#{suffix}"
            @handle = DynamicLibrary.open_library @name, flags
          end

          # Try with suffixes and a prefix
          unless @handle
            FFI::LIB_SUFFIXES.detect do |suffix|
              @name = "lib#{name}.#{suffix}"
              @handle = DynamicLibrary.open_library @name, flags
            end
          end

          unless @handle
            orig_error = orig_error.split("\n").first
            # API Compat. LoadError is wrong here.
            raise LoadError, "Could not open library #{name} - #{orig_error}"
          end
        end
      else
        @name = "[current process]"
        @handle = FFI::Pointer::CURRENT_PROCESS
      end
    end

    attr_reader :name

    def find_symbol(name)
      ptr = DynamicLibrary.find_symbol @handle, name
      return nil unless ptr
      # defined in kernel/platform/pointer.rb
      FFI::DynamicLibrary::Symbol.new(self, ptr, name)
    end

    alias_method :find_function, :find_symbol
    alias_method :find_variable, :find_symbol

    def last_error
      DynamicLibrary.last_error
    end

    CURRENT_PROCESS = DynamicLibrary.new(nil)
  end
end
