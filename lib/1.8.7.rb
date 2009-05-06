# All libraries for Ruby 1.8.7+ should be located here or in 1.9
Rubinius.target_ruby "1.8.7"
require 'enumerator'

require '1.8.7/array'
require '1.8.7/enumerable'

unless Rubinius::TARGET_IS_19
  # Changes proper to 1.8.7 
  Enumerator = Enumerable::Enumerator
end

