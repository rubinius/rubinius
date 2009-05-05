# All libraries for Ruby 1.9+ should be located here
RUBY_VERSION.replace "1.9.1" if RUBY_VERSION < "1.9.1"
require '1.8.7'
require '1.9/array'