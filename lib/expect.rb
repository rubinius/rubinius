begin
  require "rubysl/expect"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-expect", e
end
