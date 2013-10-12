begin
  require "rubysl/win32ole"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-win32ole", e
end
