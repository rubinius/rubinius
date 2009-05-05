# All libraries for Ruby 1.8.7+ should be located here or in 1.9
RUBY_VERSION.replace "1.8.7" if RUBY_VERSION == "1.8.6"
require 'enumerator'

require '1.8.7/array'
require '1.8.7/enumerable'

if RUBY_VERSION <= "1.9"
  # Changes proper to 1.8.7 
  Enumerator = Enumerable::Enumerator
end

