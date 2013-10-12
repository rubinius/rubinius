begin
  require "rubysl/rexml"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-rexml", e
end
