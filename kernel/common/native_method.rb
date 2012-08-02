# -*- encoding: us-ascii -*-

##
# A wrapper for a calling a function in a shared library that has been
# attached via rb_define_method().
#
# The primitive slot for a NativeMethod points to the nmethod_call primitive
# which dispatches to the underlying C function.

module Rubinius
  class NativeMethod < Executable

    attr_reader :file
    attr_reader :name
    attr_reader :arity

    #
    # Load a dynamic library at path +library+, having name +extension_name+.
    #
    # +extension_name+ is used to calculate what Init_ function to call.
    #
    def self.load_extension(library, extension_name)
      name = "Init_#{extension_name.sub(/\..*$/, '')}"

      begin
        lib = FFI::DynamicLibrary.new(library, FFI::DynamicLibrary::RTLD_NOW | FFI::DynamicLibrary::RTLD_GLOBAL)
      rescue LoadError => e
        raise LoadError::InvalidExtensionError, e.message, e
      end

      ver = lib.find_symbol("__X_rubinius_version")

      unless ver
        raise LoadError::InvalidExtensionError, "Out-of-date or not compatible. Recompile or reinstall gem (#{library})"
      end

      sym = lib.find_symbol(name)

      unless sym
        raise LoadError::InvalidExtensionError, "Missing Init_ function (#{name})"
      end

      entry_point = load_entry_point(library, name.to_sym, sym)

      entry_point.invoke(:init, Rubinius, self, [], nil)

      true
    end

    #
    # Load extension and generate a NativeMethod for its entry point.
    #
    # +ptr+ is an FFI::Pointer to a C function.
    #
    def self.load_entry_point(library, name, ptr)
      Rubinius.primitive :nativemethod_load_extension_entry_point
      raise PrimitiveFailure, "Rubinius::NativeMethod3load_entry_point primitive failed: #{library}"
    end

    def lines
      nil
    end

    def literals
      nil
    end

    def line_from_ip(i)
      0
    end

    def first_line
      0
    end

    def defined_line
      0
    end

    def active_path
      @file
    end
  end
end
