# Add a Rubinius platform to the platforms list.
cpu = Gem::Platform.local.cpu
version = Rubinius::VERSION.split('.')[0, 2].join '.'
Gem.platforms << Gem::Platform.new([cpu, 'rubinius', version])

module Gem

  def self.default_bindir
    File.join Rubinius::GEMS_PATH, Gem::ConfigMap[:ruby_version], "bin"
  end

  def self.default_dir
    File.join Rubinius::GEMS_PATH, Gem::ConfigMap[:ruby_version]
  end

  def self.default_preinstalled_dir
    File.join Rubinius::GEMS_PATH, "rubinius", "preinstalled"
  end

  def self.default_path
    dirs = [default_dir, default_preinstalled_dir]
    # This is the same test rubygems/defaults.rb uses
    dirs.unshift(Gem.user_dir) if File.exists?(Gem.user_home)
    dirs
  end

  def self.default_exec_format
    exec_format = ConfigMap[:ruby_install_name].sub(/rbx|ruby/, '%s') rescue '%s'

    unless exec_format =~ /%s/ then
      raise Gem::Exception,
        "[BUG] invalid exec_format #{exec_format.inspect}, no %s"
    end

    exec_format
  end
end

