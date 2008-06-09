# Add a Rubinius platform to the platforms list.
cpu = Gem::Platform.local.cpu
version = Rubinius::RBX_VERSION.split('.')[0, 2].join '.'
Gem.platforms << Gem::Platform.new([cpu, 'rubinius', version])

# RBX_PREFIX lets use detect that we're running in dev mode, and use
# special dev paths.
if ENV['RBX_PREFIX']
  def Gem.default_bindir
    File.join ENV['RBX_PREFIX'], "lib", "bin"
  end

  def Gem.default_dir
    File.join ENV['RBX_PREFIX'], "tmp", "gems"
  end
else
  def Gem.default_bindir
    File.join Rubinius::CODE_PATH, "bin"
  end

  def Gem.default_dir
    File.join File.dirname(Rubinius::CODE_PATH), 'gems', Gem::ConfigMap[:ruby_version]
  end

end

def Gem.default_path
  version = Rubinius::RBX_VERSION.split('.')[0, 2].join '.'
  pre_installed = File.join File.dirname(Rubinius::CODE_PATH), "gems",
    "rubinius", version
  [Gem.default_dir, pre_installed]
end
