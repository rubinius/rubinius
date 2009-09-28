require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/eql'

describe "Method#==" do
  it_behaves_like(:method_equal, :==)
end
