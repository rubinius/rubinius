begin
  require "rubysl/curses"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-curses", e
end
