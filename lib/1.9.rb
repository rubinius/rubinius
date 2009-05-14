# All libraries for Ruby 1.9+ should be located here
RUBY_VERSION.replace "1.9" unless RUBY_VERSION > "1.9"
class Enumerator ; end
module Enumerable
  Enumerator = ::Enumerator # So that libraries defining Enumerable::Enumerator work
end
require '1.8.7'
require '1.9/array'
require '1.9/enumerable'
require '1.9/enumerator'
require '1.9/hash'
require '1.9/kernel'


module Enumerable
  remove_const(:Enumerator)
end
$:.unshift File.dirname(__FILE__) + '/1.9' # take precedence for libraries, e.g. require 'stringio'


