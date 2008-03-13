require File.dirname(__FILE__) + '/capture'
require File.dirname(__FILE__) + '/scratch'

module Kernel
  def pretty_inspect
    inspect
  end
end
