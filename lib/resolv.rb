begin
  require "rubysl/resolv"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-resolv", e
end
