require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#row_size" do
  
  it "should be correct" do
    n = 10
    data = [ ]
    1.upto n do |x|
      data << [x,x]
      Matrix[ *data ].row_size.should == x
    end
  end
  
end


