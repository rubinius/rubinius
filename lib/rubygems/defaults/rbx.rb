# Add a Rubinius platform to the platforms list.
cpu = Gem::Platform.local.cpu
version = Rubinius::RBX_VERSION.split('.')[0, 2].join '.'
Gem.platforms << Gem::Platform.new([cpu, 'rubinius', version])

