begin
  require "rubysl/stringio"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-stringio", e
end
