begin
  require "rubysl/rinda"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-rinda", e
end
