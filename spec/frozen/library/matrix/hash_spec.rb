require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

# Note: Matrix just computes a bitwise OR of the element's .hash values, so
# their position in the matrix isn't taken into account. This means that
# matrices with the same elements in different positions will generate the
# same .hash code...
describe "Matrix#hash" do
  
  it "returns a Fixnum" do
    Matrix[ [1,2] ].hash.should be_an_instance_of(Fixnum)
  end  

  it "returns the same value for the same matrix" do
    data = [ [40,5], [2,7] ]
    Matrix[ *data ].hash.should == Matrix[ *data ].hash
  end

end
