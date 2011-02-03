# Stub file to conform gem name (rb-readline)
# It forces require of bundled readline instead of any already existing
# in your Ruby installation. It will avoid any possible warning caused
# by double require.
unless defined?(RbReadline)
  require File.expand_path('../readline', __FILE__)
end
