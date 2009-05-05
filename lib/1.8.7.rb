# All libraries for Ruby 1.8.7+ should be located here or in 1.9
RUBY_VERSION.replace "1.8.7" if RUBY_VERSION == "1.8.6"
require '1.8.7/array'
require '1.8.7/enumerable'