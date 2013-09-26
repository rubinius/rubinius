begin
  require "rubysl/delegate"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-delegate", e
end
