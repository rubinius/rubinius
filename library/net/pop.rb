begin
  require "rubysl/net/pop"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-net-pop", e
end
