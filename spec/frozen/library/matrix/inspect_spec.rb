require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#inspect" do
  
  it "returns a stringified representation of the Matrix" do
    Matrix[ [1,2], [2,1] ].inspect.should == "Matrix[[1, 2], [2, 1]]"
  end

end
