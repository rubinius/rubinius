require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/kind_of'

describe "Kernel#is_a?" do
  it_behaves_like(@kernel_kind_of , :is_a?) 
end
