begin
  require "rubysl/irb"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-irb", e
end
