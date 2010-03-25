# Add a Rubinius platform to the platforms list.
cpu = Gem::Platform.local.cpu
version = Rubinius::VERSION.split('.')[0, 2].join '.'
Gem.platforms << Gem::Platform.new([cpu, 'rubinius', version])

module Gem

  def self.precompile_install(installer)
    spec = installer.spec
    dir = File.join(installer.gem_home, "gems", spec.full_name)

    files = Dir["#{dir}/**/*.rb"]

    if Gem.configuration.really_verbose
      STDERR.puts "  rbx: Precompiling #{files.size} files..."
    end

    error = 0
    files.each do |file|
      begin
        Rubinius.compile_file file
      rescue Rubinius::CompileError, SyntaxError, LoadError
        error += 1
      end
    end

    if Gem.configuration.really_verbose and error > 0
      STDERR.puts "  rbx: #{error} error(s) precompiling."
    end
  end

  @post_install_hooks << method(:precompile_install)

  def self.default_bindir
    File.join Rubinius::GEMS_PATH, "bin"
  end

  def self.default_dir
    File.join Rubinius::GEMS_PATH, Gem::ConfigMap[:ruby_version]
  end

  def self.default_preinstalled_dir
    pre_installed = File.join Rubinius::GEMS_PATH, "rubinius", "preinstalled"
  end

  def self.default_path
    dirs = [default_dir, default_preinstalled_dir]
    # This is the same test rubygems/defaults.rb uses
    dirs.unshift(Gem.user_dir) if File.exists?(Gem.user_home)
    dirs
  end

  def self.default_exec_format
    exec_format = ConfigMap[:ruby_install_name].sub('rbx', '%s') rescue '%s'

    unless exec_format =~ /%s/ then
      raise Gem::Exception,
        "[BUG] invalid exec_format #{exec_format.inspect}, no %s"
    end

    exec_format
  end
end

