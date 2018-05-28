module Rubinius
  class CodeLoader
    def self.registry
      Rubinius.synchronize(self) { @registry ||= [] }
    end

    def self.loading?(path)
      feature = nil

      resource = Feature.new path
      if resource.loadable? or resource.search
        feature = resource.feature
      end

      unless feature
        resource = Library.new path
        if resource.loadable? or resource.search
          feature = resource.feature
        end
      end

      if feature
        Rubinius.synchronize(registry) do
          return true if registry.include? feature
        end
      end

      false
    end

    class Resource
      attr_reader :feature

      def initialize(stem, ext)
        stem = Type.coerce_to_path stem
        stem = "#{stem}#{ext}" unless stem.suffix? ext

        @relative = false

        if stem[0] == ?~
          @prefix = ""
          stem = File.expand_path stem
        elsif stem[0] == ?/
          @prefix = ""
        elsif stem[0] == ?. and (stem[1] == ?/ or
            (stem[1] == ?. and stem[2] == ?/))
          @relative = true
          @prefix = Dir.pwd
        else
          @prefix = nil
        end

        @stem = stem
        @feature = nil
        @record = true
      end

      private :initialize

      def resolve
        return true if loadable?
        return true if search
        return false
      end

      def search
        return false if @relative

        $LOAD_PATH.each do |dir|
          if dir[-1] == ?/
            @prefix = dir[0...-1]
          else
            @prefix = dir
          end

          return true if loadable?
        end

        return false
      end

      def loadable?
        feature = "#{@prefix}/#{@stem}"

        stat = File::Stat.stat feature

        if stat and stat.file? and stat.readable?
          @feature = File.expand_path feature
          return true
        else
          return false
        end
      end

      def unload_feature
        Rubinius.synchronize($LOADED_FEATURES) do
          $LOADED_FEATURES.delete_if { |f| f == @feature }
        end
      end
    end

    class Source < Resource
      def initialize(stem, ext)
        super stem, ext

        @code = nil
      end

      private :initialize

      def load
        if @code
          @feature = @code.file.to_s
        else
          @code = ToolSets::Runtime::Compiler.compile_file @feature
          CoreDB.store @code, @stem, @prefix, @feature, @record
        end

        run_script

        return true
      end

      def make_script
        script = @code.create_script @wrap
        script.file_path = script.data_path = @code.file.to_s
        script
      end

      def run_script
        make_script
        Rubinius.run_script @code
      rescue Object
        unload_feature
        raise
      end
    end

    class Feature < Source
      def initialize(stem)
        super stem, CodeLoader.source_extension
      end

      private :initialize

      def resolve
        @code = CoreDB.load_feature @stem, CodeLoader.source_extension, false, @record

        return true if @code or false.equal? @code
        return true if loadable?
        return true if search

        return false
      end

      def load
        return false if false.equal? @code

        super()
      end
    end

    class CodeModule < Source
      def initialize(stem, wrap)
        super stem, ""

        @wrap = wrap
        @record = false
      end

      private :initialize

      def resolve
        @code = CoreDB.load_feature @stem, CodeLoader.source_extension, true, @record

        return true if @code
        return true if loadable?
        return true if search

        @prefix = Dir.pwd
        return true if loadable?

        return false
      end
    end

    class Script < Source
      def initialize(stem)
        super File.expand_path(stem), ""

        @file = stem
        @record = false
      end

      private :initialize

      def resolve
        @code = CoreDB.load_feature @stem, "", true, @record

        return true if @code
        return true if loadable?
        return false
      end

      def make_script
        script = super()
        script.file_path = @file
        script.make_main!
      end
    end

    class Library < Resource
      def initialize(stem)
        if stem.suffix? ".so"
          super stem[0...-3], CodeLoader.library_extension
        else
          super stem, CodeLoader.library_extension
        end
      end

      private :initialize

      def load
        CoreDB.store nil, @stem, @prefix, @feature, @record

        name = File.basename @feature, CodeLoader.library_extension
        NativeMethod.load_extension(@feature, name)

        return true
      rescue Object
        unload_feature
        raise
      end
    end

    def self.source_extension
      ".rb"
    end

    def self.library_extension
      LIBSUFFIX
    end

    # Loads rubygems using the bootstrap standard library files.
    def self.load_rubygems
      require "rubygems"
    end

    # Loads the pre-compiled bytecode compiler. Sets up paths needed by the
    # compiler to find dependencies like the parser.
    def self.load_compiler
      begin
        require "rubinius/code/toolset"

        ToolSets.create :runtime do
          begin
            require "rubinius/code/melbourne"
          rescue LoadError
            STDERR.puts "Melbourne failed to load, Ruby source parsing disabled"
          end
          require "rubinius/code/processor"
          require "rubinius/code/compiler"
          require "rubinius/code/ast"
        end
      rescue Object => e
        raise LoadError, "Unable to load the bytecode compiler", e
      end
    end

    def initialize(stem)
      @stem = stem
    end

    private :initialize

    def load_script
      resource = Script.new @stem

      load_error unless resource.resolve
      resource.load
    end

    def load(wrap)
      resource = CodeModule.new @stem, wrap

      load_error unless resource.resolve
      resource.load
    end

    def require
      resource = Feature.new @stem
      if resource.resolve
        register_file resource.feature
        return resource.load
      end

      resource = Library.new @stem
      if resource.resolve
        register_file resource.feature
        return resource.load
      end

      load_error
    ensure
      unregister_file resource.feature if resource
    end

    def require_relative(scope)
      script = scope.current_script
      if script
        if script.data_path
          path = File.dirname(File.realdirpath(script.data_path))
        else
          path = Dir.pwd
        end

        @stem = File.expand_path(@stem, path)

        require
      else
        raise LoadError.new "unable to get relative path"
      end
    end

    def register_file(path)
      registry = self.class.registry

      Rubinius.synchronize(registry) do
        registry << path
      end
    end

    def unregister_file(path)
      registry = self.class.registry

      Rubinius.synchronize(registry) do
        registry.delete_if { |x| x == path }
      end
    end

    def load_error
      # Some versions of Rails override LoadError.new but do NOT override
      # LoadError.exception, so we must use LoadError.new here to match MRI
      # behavior.
      raise LoadError.new("no such file to load -- #{@stem}", path: @stem)
    end
  end
end
