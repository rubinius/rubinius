$:.unshift File.dirname(__FILE__)
require 'exception'
require 'tuple'

class Module
  def ivar_as_index(hash)
    # do nothing
  end
  
  public :include
end

module Rubinius
  class AtExit
    def self.unshift(x) end
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
  def disable_long_return!() end
end

require 'kernel/bootstrap/array'
require 'kernel/common/array'
