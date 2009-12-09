require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/shared/equal_value'
require 'matrix'

describe "Matrix#==" do
  it_behaves_like(:equal, :==)

  it "returns true if some elements are == but not eql?" do
    Matrix[[1, 2],[3, 4]].should == Matrix[[1, 2],[3, 4.0]]
  end
end
