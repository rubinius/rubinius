require 'matrix'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Matrix#column_size" do
  
  it "should be correct" do
    data =  [[1,2]]
    Matrix[ *data ].column_size.should == 2
  end
  it "should throw an error on an empty Matrix" do
    lambda{ Matrix[].column_size }.should raise_error    
  end
  
end


