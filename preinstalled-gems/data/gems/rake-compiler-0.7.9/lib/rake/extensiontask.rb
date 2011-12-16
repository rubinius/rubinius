#!/usr/bin/env ruby

require 'rake/baseextensiontask'
require "rubygems/package_task"

# Define a series of tasks to aid in the compilation of C extensions for
# gem developer/creators.

module Rake
  class ExtensionTask < BaseExtensionTask

    attr_accessor :config_script
    attr_accessor :cross_compile
    attr_accessor :cross_platform
    attr_accessor :cross_config_options
    attr_accessor :no_native
    attr_accessor :config_includes

    def init(name = nil, gem_spec = nil)
      super
      @config_script = 'extconf.rb'
      @source_pattern = "*.c"
      @compiled_pattern = "*.{o,obj,so,bundle,dSYM}"
      @cross_compile = false
      @cross_config_options = []
      @cross_compiling = nil
      @no_native = false
      @config_includes = []
    end

    def cross_platform
      @cross_platform ||= 'i386-mingw32'
    end

    def cross_compiling(&block)
      @cross_compiling = block if block_given?
    end

    def binary(platform = nil)
      if platform == "java"
	warn_once <<-EOF
Compiling a native C extension on JRuby. This is discouraged and a 
Java extension should be preferred.
        EOF
        "#{name}.#{RbConfig::MAKEFILE_CONFIG['DLEXT']}"
      else
        super
      end
    end

    def define
      if (defined?(RUBY_ENGINE) && RUBY_ENGINE == 'ironruby')
        warn_once <<-EOF
WARNING: You're attempting to (cross-)compile C extensions from a platform
(#{RUBY_ENGINE}) that does not support native extensions or mkmf.rb.
Rerun `rake` under MRI Ruby 1.8.x/1.9.x to cross/native compile.
        EOF
        return
      end

      super

      unless compiled_files.empty?
        warn "WARNING: rake-compiler found compiled files in '#{@ext_dir}' directory. Please remove them."
      end

      # only gems with 'ruby' platforms are allowed to define native tasks
      define_native_tasks if !@no_native && (@gem_spec && @gem_spec.platform == 'ruby')

      # only define cross platform functionality when enabled
      return unless @cross_compile

      if cross_platform.is_a?(Array) then
        cross_platform.each { |platf| define_cross_platform_tasks(platf) }
      else
        define_cross_platform_tasks(cross_platform)
      end
    end

    private
    def define_compile_tasks(for_platform = nil, ruby_ver = RUBY_VERSION)
      # platform usage
      platf = for_platform || platform

      # lib_path
      lib_path = lib_dir

      # tmp_path
      tmp_path = "#{@tmp_dir}/#{platf}/#{@name}/#{ruby_ver}"

      # cleanup and clobbering
      CLEAN.include(tmp_path)
      CLOBBER.include("#{lib_path}/#{binary(platf)}")
      CLOBBER.include("#{@tmp_dir}")

      # directories we need
      directory tmp_path
      directory lib_dir

      # copy binary from temporary location to final lib
      # tmp/extension_name/extension_name.{so,bundle} => lib/
      task "copy:#{@name}:#{platf}:#{ruby_ver}" => [lib_path, "#{tmp_path}/#{binary(platf)}"] do
        install "#{tmp_path}/#{binary(platf)}", "#{lib_path}/#{binary(platf)}"
      end

      # binary in temporary folder depends on makefile and source files
      # tmp/extension_name/extension_name.{so,bundle}
      file "#{tmp_path}/#{binary(platf)}" => ["#{tmp_path}/Makefile"] + source_files do
        chdir tmp_path do
          sh make
        end
      end

      # makefile depends of tmp_dir and config_script
      # tmp/extension_name/Makefile
      file "#{tmp_path}/Makefile" => [tmp_path, extconf] do |t|
        options = @config_options.dup

        # include current directory
        include_dirs = ['.'].concat(@config_includes).uniq.join(File::PATH_SEPARATOR)
        cmd = [Gem.ruby, "-I#{include_dirs}"]

        # if fake.rb is present, add to the command line
        if t.prerequisites.include?("#{tmp_path}/fake.rb") then
          cmd << '-rfake'
        end

        # build a relative path to extconf script
        abs_tmp_path = (Pathname.new(Dir.pwd) + tmp_path).realpath
        abs_extconf = (Pathname.new(Dir.pwd) + extconf).realpath

        # now add the extconf script
        cmd << abs_extconf.relative_path_from(abs_tmp_path)

        # rbconfig.rb will be present if we are cross compiling
        if t.prerequisites.include?("#{tmp_path}/rbconfig.rb") then
          options.push(*@cross_config_options)
        end

        # add options to command
        cmd.push(*options)

        chdir tmp_path do
          # FIXME: Rake is broken for multiple arguments system() calls.
          # Add current directory to the search path of Ruby
          # Also, include additional parameters supplied.
          sh cmd.join(' ')
        end
      end

      # compile tasks
      unless Rake::Task.task_defined?('compile') then
        desc "Compile all the extensions"
        task "compile"
      end

      # compile:name
      unless Rake::Task.task_defined?("compile:#{@name}") then
        desc "Compile #{@name}"
        task "compile:#{@name}"
      end

      # Allow segmented compilation by platform (open door for 'cross compile')
      task "compile:#{@name}:#{platf}" => ["copy:#{@name}:#{platf}:#{ruby_ver}"]
      task "compile:#{platf}" => ["compile:#{@name}:#{platf}"]

      # Only add this extension to the compile chain if current
      # platform matches the indicated one.
      if platf == RUBY_PLATFORM then
        # ensure file is always copied
        file "#{lib_path}/#{binary(platf)}" => ["copy:#{name}:#{platf}:#{ruby_ver}"]

        task "compile:#{@name}" => ["compile:#{@name}:#{platf}"]
        task "compile" => ["compile:#{platf}"]
      end
    end

    def define_native_tasks(for_platform = nil, ruby_ver = RUBY_VERSION, callback = nil)
      platf = for_platform || platform

      # tmp_path
      tmp_path = "#{@tmp_dir}/#{platf}/#{@name}/#{ruby_ver}"

      # lib_path
      lib_path = lib_dir

      # create 'native:gem_name' and chain it to 'native' task
      unless Rake::Task.task_defined?("native:#{@gem_spec.name}:#{platf}")
        task "native:#{@gem_spec.name}:#{platf}" do |t|
          # FIXME: workaround Gem::Specification limitation around cache_file:
          # http://github.com/rubygems/rubygems/issues/78
          spec = gem_spec.dup
          spec.instance_variable_set(:"@cache_file", nil) if spec.respond_to?(:cache_file)

          # adjust to specified platform
          spec.platform = Gem::Platform.new(platf)

          # clear the extensions defined in the specs
          spec.extensions.clear

          # add the binaries that this task depends on
          ext_files = []

          # go through native prerequisites and grab the real extension files from there
          t.prerequisites.each do |ext|
            ext_files << ext
          end

          # include the files in the gem specification
          spec.files += ext_files

          # expose gem specification for customization
          if callback
            callback.call(spec)
          end

          # Generate a package for this gem
          Gem::PackageTask.new(spec) do |pkg|
            pkg.need_zip = false
            pkg.need_tar = false
          end
        end
      end

      # add binaries to the dependency chain
      task "native:#{@gem_spec.name}:#{platf}" => ["#{lib_path}/#{binary(platf)}"]

      # ensure the extension get copied
      unless Rake::Task.task_defined?("#{lib_path}/#{binary(platf)}") then
        file "#{lib_path}/#{binary(platf)}" => ["copy:#{@name}:#{platf}:#{ruby_ver}"]
      end

      # Allow segmented packaging by platform (open door for 'cross compile')
      task "native:#{platf}" => ["native:#{@gem_spec.name}:#{platf}"]

      # Only add this extension to the compile chain if current
      # platform matches the indicated one.
      if platf == RUBY_PLATFORM then
        task "native:#{@gem_spec.name}" => ["native:#{@gem_spec.name}:#{platf}"]
        task "native" => ["native:#{platf}"]
      end
    end

    def define_cross_platform_tasks(for_platform)
      if ruby_vers = ENV['RUBY_CC_VERSION']
        ruby_vers = ENV['RUBY_CC_VERSION'].split(':')
      else
        ruby_vers = [RUBY_VERSION]
      end

      multi = (ruby_vers.size > 1) ? true : false

      ruby_vers.each do |version|
        # save original lib_dir
        orig_lib_dir = @lib_dir

        # tweak lib directory only when targeting multiple versions
        if multi then
          version =~ /(\d+.\d+)/
          @lib_dir = "#{@lib_dir}/#{$1}"
        end

        define_cross_platform_tasks_with_version(for_platform, version)

        # restore lib_dir
        @lib_dir = orig_lib_dir
      end
    end

    def define_cross_platform_tasks_with_version(for_platform, ruby_ver)
      config_path = File.expand_path("~/.rake-compiler/config.yml")

      # warn the user about the need of configuration to use cross compilation.
      unless File.exist?(config_path)
        warn "rake-compiler must be configured first to enable cross-compilation"
        return
      end

      config_file = YAML.load_file(config_path)

      # tmp_path
      tmp_path = "#{@tmp_dir}/#{for_platform}/#{@name}/#{ruby_ver}"

      # lib_path
      lib_path = lib_dir

      unless rbconfig_file = config_file["rbconfig-#{ruby_ver}"] then
        warn "no configuration section for specified version of Ruby (rbconfig-#{ruby_ver})"
        return
      end

      # mkmf
      mkmf_file = File.expand_path(File.join(File.dirname(rbconfig_file), '..', 'mkmf.rb'))

      # define compilation tasks for cross platform!
      define_compile_tasks(for_platform, ruby_ver)

      # chain fake.rb, rbconfig.rb and mkmf.rb to Makefile generation
      file "#{tmp_path}/Makefile" => ["#{tmp_path}/fake.rb",
                                      "#{tmp_path}/rbconfig.rb",
                                      "#{tmp_path}/mkmf.rb"]

      # copy the file from the cross-ruby location
      file "#{tmp_path}/rbconfig.rb" => [rbconfig_file] do |t|
        cp t.prerequisites.first, t.name
      end

      # copy mkmf from cross-ruby location
      file "#{tmp_path}/mkmf.rb" => [mkmf_file] do |t|
        cp t.prerequisites.first, t.name
      end

      # genearte fake.rb for different ruby versions
      file "#{tmp_path}/fake.rb" do |t|
        File.open(t.name, 'w') do |f|
          f.write fake_rb(for_platform, ruby_ver)
        end
      end

      # now define native tasks for cross compiled files
      if @gem_spec && @gem_spec.platform == 'ruby' then
        define_native_tasks(for_platform, ruby_ver, @cross_compiling)
      end

      # create cross task
      task 'cross' do
        # clear compile dependencies
        Rake::Task['compile'].prerequisites.reject! { |t| !compiles_cross_platform.include?(t) }

        # chain the cross platform ones
        task 'compile' => ["compile:#{for_platform}"]

        # clear lib/binary dependencies and trigger cross platform ones
        # check if lib/binary is defined (damn bundle versus so versus dll)
        if Rake::Task.task_defined?("#{lib_path}/#{binary(for_platform)}") then
          Rake::Task["#{lib_path}/#{binary(for_platform)}"].prerequisites.clear
        end

        # FIXME: targeting multiple platforms copies the file twice
        file "#{lib_path}/#{binary(for_platform)}" => ["copy:#{@name}:#{for_platform}:#{ruby_ver}"]

        # if everything for native task is in place
        if @gem_spec && @gem_spec.platform == 'ruby' then
          # double check: only cross platform native tasks should be here
          # FIXME: Sooo brittle
          Rake::Task['native'].prerequisites.reject! { |t| !natives_cross_platform.include?(t) }
          task 'native' => ["native:#{for_platform}"]
        end
      end
    end

    def extconf
      "#{@ext_dir}/#{@config_script}"
    end

    def make
      unless @make
        @make =
          if RUBY_PLATFORM =~ /mswin/ then
            'nmake'
          else
            ENV['MAKE'] || %w[gmake make].find { |c|
              system("#{c} -v >> #{dev_null} 2>&1")
            }
          end
      end
      @make
    end

    def dev_null
      windows? ? 'NUL' : '/dev/null'
    end

    def compiled_files
      FileList["#{@ext_dir}/#{@compiled_pattern}"]
    end

    def compiles_cross_platform
      [*@cross_platform].map { |p| "compile:#{p}" }
    end

    def natives_cross_platform
      [*@cross_platform].map { |p| "native:#{p}" }
    end

    def fake_rb(platform, version)
      <<-FAKE_RB
        class Object
          remove_const :RUBY_PLATFORM
          remove_const :RUBY_VERSION
          remove_const :RUBY_DESCRIPTION if defined?(RUBY_DESCRIPTION)
          RUBY_PLATFORM = "#{platform}"
          RUBY_VERSION = "#{version}"
          RUBY_DESCRIPTION = "ruby \#{RUBY_VERSION} (\#{RUBY_RELEASE_DATE}) [\#{RUBY_PLATFORM}]"
        end
        if RUBY_PLATFORM =~ /mswin|bccwin|mingw/
          class File
            remove_const :ALT_SEPARATOR
            ALT_SEPARATOR = "\\\\"
          end
        end

        posthook = proc do
          $ruby = "#{Gem.ruby}"
          untrace_var(:$ruby, posthook)
        end
        trace_var(:$ruby, posthook)
FAKE_RB
    end
  end
end
