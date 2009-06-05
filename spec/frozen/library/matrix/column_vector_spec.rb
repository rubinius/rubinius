require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.column_vector" do

  it "returns a single column Matrix when called with an Array" do
    m = Matrix.column_vector([4,5,6])
    m.should be_an_instance_of(Matrix)
    m.should == Matrix[ [4],[5],[6] ]
  end

  it "returns an empty Matrix when called with an empty Array" do
    m = Matrix.column_vector([])
    m.should be_an_instance_of(Matrix)
    m.should == Matrix[]
  end

  it "returns a single column Matrix when called with a Fixnum" do
    m = Matrix.column_vector(4)
    m.should be_an_instance_of(Matrix)
    m.should == Matrix[ [4] ]
  end
  
  it "returns a single column Matrix when called with a Float" do  
    m = Matrix.column_vector(0.98887)
    m.should be_an_instance_of(Matrix)
    m.should == Matrix[ [0.98887] ]
  end

end
