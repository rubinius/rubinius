begin
  require "rubysl/thwait"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-thwait", e
end
