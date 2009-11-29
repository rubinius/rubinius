# Add a Rubinius platform to the platforms list.
cpu = Gem::Platform.local.cpu
version = Rubinius::VERSION.split('.')[0, 2].join '.'
Gem.platforms << Gem::Platform.new([cpu, 'rubinius', version])

module Gem
  def self.default_bindir
    File.join Rubinius::GEMS_PATH, "bin"
  end

  def self.default_dir
    File.join Rubinius::GEMS_PATH, Gem::ConfigMap[:ruby_version]
  end

  def self.default_preinstalled_dir
    version = Rubinius::VERSION.split('.')[0, 2].join '.'
    pre_installed = File.join Rubinius::GEMS_PATH, "rubinius", version
  end

  def self.default_path
    dirs = [default_dir, default_preinstalled_dir]
    dirs.unshift(Gem.user_home) if File.exist?(Gem.user_home)
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

