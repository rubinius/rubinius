require 'matrix'
require File.dirname(__FILE__) + '/../../spec_helper'

describe "Matrix#row" do
  before :all do
    @data =  [[1,2],[1,2]]
  end
  it "should return a Vector when called without a block" do
    Matrix[ *@data ].row(0).class.should == Vector
  end
  it "should the correct Vector when called without a block" do
    Matrix[ *@data ].row(0).should == Vector[1,2]
  end
  it "should return an Array when called with a block" do
    Matrix[ *@data ].row(0){|x| x }.class.should == Array
  end
end