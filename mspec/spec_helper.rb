require File.dirname(__FILE__) + '/capture'
require File.dirname(__FILE__) + '/scratch'

# Remove this when MRI has intelligent warnings
$VERBOSE = nil unless $VERBOSE

module Kernel
  def pretty_inspect
    inspect
  end
end
