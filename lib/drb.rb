begin
  require "rubysl/drb"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-drb", e
end
