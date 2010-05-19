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

    #
    # Returns true if library loads successfully.
    #
    # TODO: Fix up to properly add method, clear cache etc.
    #
    def self.load_extension(library_path, extension_name)
      library = library_path.sub /#{Rubinius::LIBSUFFIX}$/, ''
      name = "Init_#{extension_name}"

      entry_point = load_entry_point library, name

      symbol = name.to_sym

      Rubinius.object_metaclass(self).method_table.store symbol, entry_point, :public
      Rubinius::VM.reset_method_cache(symbol)
      __send__ symbol

      true
    end

    #
    # Load extension and generate a NativeMethod for its entry point.
    #
    def self.load_entry_point(library_path, name)
      Ruby.primitive :nativemethod_load_extension_entry_point
      raise PrimitiveFailure, "Unable to load #{library_path}"
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
  end
end
