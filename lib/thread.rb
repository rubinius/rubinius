begin
  require "rubysl/thread"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-thread", e
end
