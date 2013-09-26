begin
  require "rubysl/ripper"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-ripper", e
end
