begin
  require "rubysl/ostruct"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-ostruct", e
end
