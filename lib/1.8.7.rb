# All libraries for Ruby 1.8.7+ should be located here or in 1.9
RUBY_VERSION.replace "1.8.7" unless RUBY_VERSION > "1.8.7"

require 'enumerator'

require '1.8.7/array'
require '1.8.7/dir'
require '1.8.7/binding'
require '1.8.7/enumerable'
require '1.8.7/enumerator'
require '1.8.7/fixnum'
require '1.8.7/float'
require '1.8.7/hash'
require '1.8.7/io'