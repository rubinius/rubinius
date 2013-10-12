begin
  require "rubysl/test/unit"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-test-unit", e
end
