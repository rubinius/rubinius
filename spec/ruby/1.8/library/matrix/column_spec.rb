require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#column" do
  before :all do
    @data =  [[1,2],[1,2]]
  end
  it "should return a Vector when called without a block" do
    Matrix[ *@data ].column(1).class.should == Vector
  end
  it "should the correct Vector when called without a block" do
    Matrix[ *@data ].column(1).should == Vector[2,2]
  end
  it "should return each number in the given column called with a block" do
    Matrix[ *@data ].column(1) do |n|
      n.should == 2
    end
  end
end