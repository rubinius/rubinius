begin
  require "rubysl/cgi/session"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-cgi-session", e
end
