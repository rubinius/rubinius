begin
  require "rubysl/webrick"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-webrick", e
end
