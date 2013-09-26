begin
  require "rubysl/tsort"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-tsort", e
end
