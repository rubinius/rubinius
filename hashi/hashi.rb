$:.unshift File.dirname(__FILE__)
require 'exception'
require 'tuple'

class Module
  def ivar_as_index(hash)
    # do nothing
  end
end

# -- special logic for Array --

class Array
  alias :old_inspect :inspect
  
  def inspect()
    "RUBY_LITERAL" + old_inspect
  end
end

class Object
  remove_const :Array
end

require 'kernel/bootstrap/array'
require 'kernel/core/array'
