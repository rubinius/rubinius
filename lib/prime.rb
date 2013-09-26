begin
  require "rubysl/prime"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-prime", e
end
