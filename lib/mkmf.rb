begin
  require "rubysl/mkmf"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-mkmf", e
end
