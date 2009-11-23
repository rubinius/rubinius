# Add a Rubinius platform to the platforms list.
cpu = Gem::Platform.local.cpu
version = Rubinius::RBX_VERSION.split('.')[0, 2].join '.'
Gem.platforms << Gem::Platform.new([cpu, 'rubinius', version])

module Gem
  def self.default_bindir
    File.join Rubinius::CODE_PATH, "gems", "bin"
  end

  def self.default_dir
    File.join Rubinius::CODE_PATH, 'gems', Gem::ConfigMap[:ruby_version]
  end

  def self.default_preinstalled_dir
    version = Rubinius::RBX_VERSION.split('.')[0, 2].join '.'
    pre_installed = File.join Rubinius::CODE_PATH, "gems",
                              "rubinius", version
  end

  def self.default_path
    [default_dir, default_preinstalled_dir]
  end
end

