begin
  require "rubysl/strscan"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-strscan", e
end
