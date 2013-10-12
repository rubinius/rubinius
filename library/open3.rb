begin
  require "rubysl/open3"
rescue LoadError => e
  Rubinius::CodeLoader.missing_standard_library "rubysl-open3", e
end
