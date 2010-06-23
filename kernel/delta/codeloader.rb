# Implementation-specific behavior for Kernel#require and Kernel#load.
#
# In particular, this file implements #load_file for loading a Ruby source
# file and #load_library for loading a shared library extension file.
#
# Also provides #require_compiled which loads a precompiled version of a Ruby
# source file. Several CodeLoader class methods are implemented as a
# convenient way to invoke the CodeLoader.

module Rubinius
  class InvalidRBC < RuntimeError; end

  class CodeLoader

    # Loads compiled files. Performs exactly as Kernel#require except that
    #
    #   require "some_file"
    #
    # will resolve to "some_file.rbc" on the $LOAD_PATH, rather than
    # "some_file.rb".
    #
    # However, if "some_file.rbc" does not exist, then like #require, this
    # method will search for a shared library extension file to load using the
    # platform's file extension for shared libraries. This simplifies loading
    # a set of pre-compiled .rbc files that also may load an extension as in
    # the case of loading the compiler on bootstrap.
    def require_compiled(check_version=true)
      saved_check = CodeLoader.check_version
      saved_extension = CodeLoader.source_extension

      CodeLoader.load_compiled = true
      CodeLoader.check_version = check_version
      CodeLoader.source_extension = ".rbc"

      self.require

      # HACK we use __send__ here so that the method inliner
      # doesn't accidentally inline a script body into here!
      MAIN.__send__ :__script__

      CodeLoader.loaded_hook.trigger!(@path)

      return true
    ensure
      add_feature
      finished

      CodeLoader.check_version = saved_check
      CodeLoader.source_extension = saved_extension
      CodeLoader.load_compiled = false
    end

    # Loads a Ruby source file specified on the command line. There is no
    # search required as the path on the command line must directly refernce a
    # loadable file. Also, the value of __FILE__ in a script loaded on the
    # command line differs from the value in a file loaded by Kernel#require
    # or Kernel#load.
    def load_script(debug)
      @file_path = @path
      @load_path = File.expand_path @path

      load_error unless loadable? @load_path
      load_file

      # HACK we use __send__ here so that the method inliner
      # doesn't accidentally inline a script body into here!
      MAIN.__send__ :__script__
      CodeLoader.loaded_hook.trigger!(@path)
    end

    # Overrides the version of #add_feature provided in kernel/common. When
    # loading precompiled source files via #require, adds ".rb" files to
    # $LOADED_FEATURES.
    def add_feature
      name = @feature.suffix?(".rbc") ? @feature[0..-2] : @feature
      $LOADED_FEATURES << name
    end

    # Default check_version flag to true
    @check_version = true

    class << self
      attr_accessor :load_compiled
      attr_accessor :check_version

      def require_compiled(name, check_version=true)
        new(name).require_compiled(check_version)
      end

      def load_script(name, debug=false)
        new(name).load_script(debug)
      end

      def execute_script(script)
        eval(script, TOPLEVEL_BINDING)
      end

      def save_compiled?
        @save_compiled ||= Config["compiler.no_rbc"] == nil
      end
    end

    # Given a path to a Ruby source file to load (i.e. @load_path), determines
    # whether a compiled version exists and is up-to-date. If it is, loads the
    # compiled version. Otherwise, compiles the Ruby source file.
    #
    # TODO: Make the compiled version checking logic available as a Compiler
    # convenience method.
    def load_file(wrap=false)
      version = CodeLoader.check_version ? Signature : 0

      if CodeLoader.load_compiled
        cm = load_compiled_file @load_path, version
      else
        compiled_name = Compiler.compiled_name @load_path

        if File.exists? compiled_name
          if @stat.mtime > File.mtime(compiled_name)
            cm = compile_file @load_path, compiled_name
          else
            begin
              cm = load_compiled_file compiled_name, version
            rescue InvalidRBC
              cm = compile_file @load_path, compiled_name
            end
          end
        else
          cm = compile_file @load_path, compiled_name
        end
      end

      script = cm.create_script(wrap)
      script.file_path = @file_path
      script.data_path = @load_path
    end

    # Compile a Ruby source file and save the compiled file. Return the
    # internal representation (CompiledMethod) of the Ruby source file.
    def compile_file(file, compiled)
      if CodeLoader.save_compiled?
        Compiler.compile file, compiled
      else
        Compiler.compile_file file
      end
    end

    # Load a compiled version of a Ruby source file.
    def load_compiled_file(path, version)
      Ruby.primitive :compiledfile_load

      raise InvalidRBC, path
    end

    # Load a shared library extension file.
    def load_library
      name = File.basename @load_path, LIBSUFFIX

      NativeMethod.load_extension(@load_path, name)
    end
  end
end
