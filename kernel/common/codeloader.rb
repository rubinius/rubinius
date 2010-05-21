# CodeLoader implements the logic for Kernel#load and Kernel#require. Only the
# implementation-agnostic behavior is provided in this file. That includes
# resolving files according to $LOAD_PATH and updating $LOADED_FEATURES.
#
# Implementation specific code for actually loading the files and shared
# library extensions is not provided here.

module Rubinius
  class CodeLoader
    attr_reader :feature
    attr_reader :path

    # The CodeLoader instance variables have the following uses:
    #
    #   @path : the +path+ converted to a String if necessary
    #   @load_path : the canonical path of the file to load
    #   @file_path : the value of __FILE__ in the file when loaded
    #   @feature : the name added to $LOADED_FEATURES
    #   @stat : a File.stat instance
    #   @type : the kind of file to load, :ruby or :library
    def initialize(path)
      @path = StringValue(path)
      @load_path = nil
      @file_path = nil
      @feature = nil
      @stat = nil
      @type = nil
    end

    # Searches for and loads Ruby source files and shared library extension
    # files. See CodeLoader.require for the rest of Kernel#require
    # functionality.
    def require
      return false unless resolve_require_path
      return false if CodeLoader.loading? @load_path

      if @type == :ruby
        CodeLoader.loading @load_path
        load_file
      else
        load_library
      end

      return @type
    end

    # Searches for and loads Ruby source files only. The files may have any
    # extension. Called by Kernel#load.
    def load(wrap=false)
      resolve_load_path
      load_file(wrap)
    end

    # Adds a feature name to $LOADED_FEATURES, where a feature may be a Ruby
    # source file or a shared library extension. Only Kernel#require and
    # autoloads use this, Kernel#load does not.
    def add_feature
      $LOADED_FEATURES << @feature
    end

    # Finalize the loading process by removing the lock to prevent concurrent
    # loading of the same file.
    def finished
      CodeLoader.loaded @load_path
    end

    # Hook support. This allows code to be told when a file was just loaded
    @loaded_hook = Rubinius::Hook.new

    class << self
      attr_reader :loaded_hook

      attr_writer :source_extension

      # Abstracts the extension for a Ruby source file.
      def source_extension
        @source_extension ||= ".rb"
      end

      # Accessor to abstract and lazily allocate the map of files being
      # loaded.
      def load_map
        @load_map ||= {}
      end

      # Add +path+ to the map of files that are being loaded.
      def loading(path)
        load_map[path] = Thread.current unless load_map.key? path
      end

      # Removes +path+ from the map of files that are in the process of being
      # loaded.
      def loaded(path)
        load_map.delete path
      end

      # Returns true if +path+ is in process of being loaded by any thread.
      def loading?(path)
        while thread = load_map[path]
          return true if thread == Thread.current
          thread.join
        end

        return false
      end

      # Returns true if +name+ includes a recognized extension (e.g. .rb or
      # the platform's shared library extension) and $LOADED_FEATURES includes
      # +name+. Otherwise returns true if $LOADED_FEATURES includes +name+
      # with .rb or the platform's shared library extension appended.
      # Otherwise, returns false. Called by both Kernel#require and when an
      # autoload constant is being registered by Module#autoload.
      def feature_provided?(name)
        if name.suffix?(".rb") or name.suffix?(LIBSUFFIX)
          return loaded? name
        elsif name.suffix? ".so"
          # This handles cases like 'require "openssl.so"' on
          # platforms like OS X where the shared library extension
          # is not ".so".
          return loaded?(name[0..-4] + LIBSUFFIX)
        else
          return true if loaded?(name + ".rb")
          return loaded?(name + LIBSUFFIX)
        end

        return false
      end

      # Returns true if $LOADED_FEATURES includes +feature+. Otherwise,
      # returns false.
      def loaded?(feature)
        $LOADED_FEATURES.include? feature
      end

      # Loads a Ruby source file or shared library extension. Called by
      # Kernel#require and when an autoload-registered constant is accessed.
      def require(name)
        loader = new name

        case loader.require
        when :ruby
          begin
            # HACK we use __send__ here so that the method inliner
            # doesn't accidentally inline a script body into here!
            MAIN.__send__ :__script__
          ensure
            loader.finished
          end
        when :library
        when false
          return false
        else
          raise "received unknown type from #{loader.class}#require"
        end

        loader.add_feature
        @loaded_hook.trigger! loader.path
        return true
      end
    end

    # Returns true if the path exists, is a regular file, and is readable.
    def loadable?(path)
      return false unless File.exists? path

      @stat = File.stat path
      @stat.file? and @stat.readable?
    end

    # Returns true if the +path+ is relative to a user's home directory. This
    # includes both the current user's home (i.e. "~/") and a specific user's
    # home (e.g. "~joe/").
    def home_path?(path)
      path[0] == ?~
    end

    # Returns true if the +path+ is an absolute path or a relative path (e.g.
    # "./" or "../").
    def qualified_path?(path)
      # TODO: fix for Windows
      path[0] == ?/ or path.prefix?("./") or path.prefix?("../")
    end

    # Searches $LOAD_PATH for a file named +name+. Does not append any file
    # extension to +name+ while searching. Used by #load to resolve the name
    # to a full path to load. Also used by #require when the file extension is
    # provided.
    def search_load_path(name)
      $LOAD_PATH.each do |dir|
        path = "#{dir}/#{name}"
        return path if loadable? path
      end

      return nil
    end

    # Main logic for converting a name to an actual file to load. Used by
    # #load and by #require when the file extension is provided.
    #
    # Expands any #home_path? to an absolute path. Then either checks whether
    # an absolute path is #loadable? or searches for a loadable file matching
    # name in $LOAD_PATH.
    #
    # Returns true if a loadable file is found, otherwise returns false.
    def verify_load_path(path)
      path = File.expand_path path if home_path? path

      @feature = path

      if qualified_path? path
        return false unless loadable? path
      else
        return false unless path = search_load_path(path)
        path = "./#{path}" unless qualified_path? path
      end

      @file_path = path
      @load_path = File.expand_path path

      return true
    end

    # Called directly by #load. Either resolves the path passed to Kernel#load
    # to a specific file or raises a LoadError.
    def resolve_load_path
      load_error unless verify_load_path @path
    end

    # Combines +directory+, +name+, and +extension+ to check if the result is
    # #loadable?. If it is, sets +@type+, +@feature+, +@load_path+, and
    # +@file_path+ and returns true. See #intialize for a description of the
    # instance variables.
    def check_path(directory, name, extension, type)
      file = "#{name}#{extension}"
      path = "#{directory}/#{file}"
      return false unless loadable? path

      @type = type
      @feature = file
      @load_path = path
      if qualified_path? path
        @file_path = path
      else
        @file_path = "./#{path}"
      end

      return true
    end

    # Searches $LOAD_PATH for a file named +name+, appending ".rb" and the
    # platform's shared library file extension in that order to locate an
    # existing file. Returns true if a file is found. Used by #require only.
    def search_require_path(name)
      $LOAD_PATH.each do |dir|
        if check_path(dir, name, CodeLoader.source_extension, :ruby) or
           check_path(dir, name, LIBSUFFIX, :library)
          return true
        end
      end

      return false
    end

    # Checks that +name+ plus +extension+ is a #loadable? file. If it is, sets
    # +@type+, +@feature+, +@file_path+, +@load_path+ and return true.
    def check_file(name, extension, type)
      file = "#{name}#{extension}"
      return false unless loadable? file

      @type = type
      @feature = file
      @file_path = file
      @load_path = file

      return true
    end

    # Main logic used by #require to locate a file when the file extension is
    # not provided.
    #
    # Expands any #home_path? to an absolute path. Then either checks whether
    # an absolute path is #loadable? or searches for a loadable file matching
    # name in $LOAD_PATH. In each case, the ".rb" extension or the platform's
    # shared library extension is appended to +name+.
    #
    # Returns true if a loadable file is found, otherwise returns false.
    def verify_require_path(name)
      name = File.expand_path name if home_path? name

      if qualified_path? name
        if !check_file(name, CodeLoader.source_extension, :ruby) and
           !check_file(name, LIBSUFFIX, :library)
          return false
        end
      else
        return false unless search_require_path(name)
      end

      @load_path = File.expand_path @load_path

      return true
    end

    # Called directly by #require. Determines whether a known file type is
    # being loaded or searches for either a Ruby source file or shared
    # extension library to load.
    def resolve_require_path
      if @path.suffix? CodeLoader.source_extension
        @type = :ruby
      elsif @path.suffix? LIBSUFFIX
        @type = :library
      elsif @path.suffix? ".so"
        # This handles cases like 'require "openssl.so"' on
        # platforms like OS X where the shared library extension
        # is not ".so".
        @path[-3..-1] = LIBSUFFIX
        @type = :library
      end

      if @type
        return false if CodeLoader.feature_provided? @path
        return true if verify_load_path @path
      else
        if verify_require_path(@path)
          return false if CodeLoader.feature_provided? @feature
          return true
        end
      end

      load_error
    end

    # Raises a LoadError if the requested file cannot be found or loaded.
    def load_error(message=nil)
      # Crazy special case.
      # MRI does NOT use rb_f_raise(rb_eLoadError, msg),
      # it does rb_exc_raise(rb_exc_new2(rb_eLoadError, msg)).
      #
      # This is significant because some versions of rails change LoadError.new
      # but NOT LoadError.exception, so this code must use LoadError.new
      # directly here to match the behavior.
      message ||= "no such file to load -- #{@path}"
      raise LoadError.new(message)
    end

    # Implementation specific method to load a Ruby source file. Overridden in
    # delta.
    def load_file
      raise NotImplementedError, "loading Ruby source files is not implemented"
    end

    # Implementation specific method to load a shared library extension file.
    # Overridden in delta.
    def load_library
      raise NotImplementedError, "loading extension libraries is not implemented"
    end
  end
end
