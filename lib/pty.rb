begin
  require "rubysl/pty"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-pty", e
end
