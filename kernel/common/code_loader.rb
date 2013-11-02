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
      @path = Rubinius::Type.coerce_to_path path
      @short_path = nil
      @load_path = nil
      @file_path = nil
      @feature = nil
      @stat = nil
      @type = nil
    end

    Lock = Object.new

    class RequireRequest
      def initialize(type, map, key)
        @type = type
        @map = map
        @key = key
        @for = nil
        @loaded = false
        @remove = true
      end

      attr_reader :type

      def take!
        lock
        @for = Thread.current
      end

      def current_thread?
        @for == Thread.current
      end

      def lock
        Rubinius.lock(self)
      end

      def unlock
        Rubinius.unlock(self)
      end

      def wait
        Rubinius.synchronize(Lock) do
          @remove = false
        end

        take!

        Rubinius.synchronize(Lock) do
          if @loaded
            @map.delete @key
          end
        end

        return @loaded
      end

      def passed!
        @loaded = true
      end

      def remove!
        Rubinius.synchronize(Lock) do
          if @loaded or @remove
            @map.delete @key
          end
        end

        unlock
      end
    end

    # Searches for and loads Ruby source files and shared library extension
    # files. See CodeLoader.require for the rest of Kernel#require
    # functionality.
    def require
      wait = false
      req = nil

      reqs = CodeLoader.load_map

      Rubinius.synchronize(Lock) do
        return unless resolve_require_path

        if req = reqs[@load_path]
          return if req.current_thread?
          wait = true
        else
          req = RequireRequest.new(@type, reqs, @load_path)
          reqs[@load_path] = req
          req.take!
        end
      end

      if wait
        if req.wait
          # While waiting the code was loaded by another thread.
          # We need to release the lock so other threads can continue too.
          req.unlock
          return false
        end

        # The other thread doing the lock raised an exception
        # through the require, so we try and load it again here.
      end

      if @type == :ruby
        load_file
      else
        load_library
      end

      return req
    end

    # requires files relative to the current directory. We do one interesting
    # check to make sure it's not called inside of an eval.
    def self.require_relative(name, scope)
      script = scope.current_script
      if script && script.data_path.nil?
        raise LoadError.new "cannot infer basepath from #{script.file_path}"
      elsif script
        require File.expand_path(name, File.dirname(File.realdirpath(script.data_path)))
      else
        raise LoadError.new "Something is wrong in trying to get relative path"
      end
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
      add_feature_to_index
    end

    def add_feature_to_index(feature = @feature)
      self.class.loaded_features_dup = $LOADED_FEATURES.dup
      self.class.loaded_features_index[@short_path] = feature
    end

    # Hook support. This allows code to be told when a file was just compiled,
    # or when it has finished loading.
    @compiled_hook = Rubinius::Hook.new
    @loaded_hook = Rubinius::Hook.new
    @loaded_features_index = Hash.new(false)

    class << self
      attr_reader :compiled_hook
      attr_reader :loaded_hook
      attr_reader :loaded_features_index
      attr_accessor :loaded_features_dup

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

      # Returns true if +name+ includes a recognized extension (e.g. .rb or
      # the platform's shared library extension) and $LOADED_FEATURES includes
      # +name+. Otherwise returns true if $LOADED_FEATURES includes +name+
      # with .rb or the platform's shared library extension appended.
      # Otherwise, returns false. Called by both Kernel#require and when an
      # autoload constant is being registered by Module#autoload.
      def feature_provided?(name)
        if name.suffix?(CodeLoader.source_extension) or name.suffix?(LIBSUFFIX)
          return loaded? name
        elsif name.suffix? ".so"
          # This handles cases like 'require "openssl.so"' on
          # platforms like OS X where the shared library extension
          # is not ".so".
          return loaded?(name[0..-4] + LIBSUFFIX)
        else
          return true if loaded?(name + CodeLoader.source_extension)
        end

        return false
      end

      # Returns true if $LOADED_FEATURES includes +feature+. Otherwise,
      # returns false.
      def loaded?(feature)
        return true if $LOADED_FEATURES.include? feature

        if !features_index_up_to_date?
          loaded_features_index.clear
          return false
        end

        loaded_features_index.include?(feature)
      end

      def features_index_up_to_date?
        loaded_features_dup == $LOADED_FEATURES
      end

      # Loads a Ruby source file or shared library extension. Called by
      # Kernel#require and when an autoload-registered constant is accessed.
      def require(name)
        loader = new name

        req = loader.require
        return false unless req

        case req.type
        when :ruby
          begin
            Rubinius.run_script loader.compiled_code
          else
            req.passed!
          ensure
            req.remove!
          end
        when :library
          req.remove!
        when false
          req.remove!
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
      @stat = File::Stat.stat path
      return false unless @stat
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

    # Main logic for converting a name to an actual file to load. Used by
    # #load and by #require when the file extension is provided.
    #
    # Expands any #home_path? to an absolute path. Then either checks whether
    # an absolute path is #loadable? or searches for a loadable file matching
    # name in $LOAD_PATH.
    #
    # Returns true if a loadable file is found, otherwise returns false.
    def verify_load_path(file, loading=false)
      file = File.expand_path file if home_path? file

      if qualified_path? file
        return false unless loadable? file
      else
        return false unless path = search_load_path(file, loading)
      end

      update_paths(file, path || file)

      return true
    end

    # Called directly by #load. Either resolves the path passed to Kernel#load
    # to a specific file or raises a LoadError.
    def resolve_load_path
      load_error unless verify_load_path @path, true
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
      update_paths(file, path)

      return true
    end

    # Searches $LOAD_PATH for a file named +name+. Does not append any file
    # extension to +name+ while searching. Used by #load to resolve the name
    # to a full path to load. Also used by #require when the file extension is
    # provided.
    def search_load_path(name, loading)
      $LOAD_PATH.each do |dir|
        path = "#{dir}/#{name}"
        return path if loadable? path
      end

      return name if loading and loadable? "./#{name}"

      return nil
    end

    # Searches $LOAD_PATH for a file named +name+, appending ".rb" and returns
    # true if found. If a file with the platform's shared library extension is
    # found, the path is saved. If no file with ".rb" is found but a file with
    # the shared library extension is found, returns true. Otherwise, returns
    # false.
    def search_require_path(name)
      library_found = false

      $LOAD_PATH.each do |dir|
        if check_path(dir, name, CodeLoader.source_extension, :ruby)
          return true
        elsif check_path(dir, name, LIBSUFFIX, :library)
          library_found = true
        end
      end

      library_found
    end

    # Checks that +name+ plus +extension+ is a #loadable? file. If it is, sets
    # +@type+, +@feature+, +@file_path+, +@load_path+ and return true.
    def check_file(name, extension, type)
      file = "#{name}#{extension}"
      return false unless loadable? file

      @type = type
      update_paths(file, file)

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

      return false if CodeLoader.feature_provided?(@path)

      verified = @type ? verify_load_path(@path) : verify_require_path(@path)
      if verified
        return false if CodeLoader.feature_provided?(@feature)
        return true
      end

      return false if CodeLoader.loaded? @path
      return true if rubygems_require

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

      error = LoadError.new(message)
      error.path = @path
      raise error
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

    # Sets +@feature+, +@file_path+, +@load_path+ with the correct format.
    # Used by #verify_load_path, #check_path and #check_file.
    def update_paths(file, path)
      path = File.expand_path path

      @feature    = path
      @short_path = file
      @file_path  = path
      @load_path  = path
    end
  end
end
