# All libraries for Ruby 1.8.7+ should be located here or in 1.9
Rubinius.target_ruby "1.8.7"
require 'enumerator'

require '1.8.7/array'
require '1.8.7/binding'
require '1.8.7/enumerable'
require '1.8.7/enumerator'

unless Rubinius::TARGET_IS_19
  # Changes proper to 1.8.7 
  Enumerator = Enumerable::Enumerator

  # Hide #sample and create the dubiously named #choice instead:
  class Array
    def choice
      sample
    end
    private :sample
  end
end

