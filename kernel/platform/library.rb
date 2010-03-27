module FFI
  module Library

    # Set which library or libraries +attach_function+ should
    # look in. By default it only searches for the function in
    # the current process. If you want to specify this as one
    # of the locations, add FFI::USE_THIS_PROCESS_AS_LIBRARY.
    # The libraries are tried in the order given.
    #
    def ffi_lib(*names)
      @ffi_lib = names
    end

    def ffi_libraries
      @ffi_lib or [FFI::USE_THIS_PROCESS_AS_LIBRARY]
    end
    private :ffi_libraries

    def ffi_library_names
      libraries = ffi_libraries
      this_process = "this process"

      names = libraries[0] || this_process

      i = 1
      size = libraries.size
      while i < size
        names.append ", "
        names.append libraries[i] || this_process
        i += 1
      end

      names
    end
    private :ffi_library_names

    def create_backend(library, name, args, ret)
      Ruby.primitive :nativefunction_bind
      # Do not raise an exception if this primitive fails to bind an external
      # function because #attach_function may be attempting to bind to a
      # function in more than one library
      raise PrimitiveFailure, "create_backend failed"
    end
    private :create_backend

    # Setup the LD_LIBRARY_PATH
    # @todo   Not using LTDL currently.
    def setup_ld_library_path(library)
      # If we have a specific reference to the library, we load it here
      specific_library = FFI.config("ld_library_path.#{library}")
      library = specific_library if specific_library

      # This adds general paths to the search
      if path = FFI.config("ld_library_path.default")
        ENV['LTDL_LIBRARY_PATH'] = [ENV['LTDL_LIBRARY_PATH'], path].compact.join(":")
      end

      library
    end
    private :setup_ld_library_path

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
        args = a4
        ret = a5
      else
        cname = name.to_s
        args = a3
        ret = a4
      end
      mname = name.to_sym

      args.map! { |a| FFI.find_type a }
      ret = FFI.find_type ret

      ffi_libraries.each do |lib|
        lib = setup_ld_library_path lib if lib

        if func = create_backend(lib, cname, args, ret)

          # Make it available as a method callable directly..
          Rubinius.object_metaclass(self).method_table.store mname, func, :public

          # and expose it as a private method for people who
          # want to include this module.
          method_table.store mname, func, :private


          return func
        end
      end

      raise FFI::NotFoundError, "Unable to find FFI '#{cname}' in: #{ffi_library_names}"
    end
  end
end
