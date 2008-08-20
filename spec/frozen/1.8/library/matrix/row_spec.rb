require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix#row" do
  before :all do
    @data = [ [1, 2], [1, 2] ]
  end

  it "returns a Vector when called without a block" do
    Matrix[ *@data ].row(0).should == Vector[1,2]
  end

  it "returns an Array when called with a block" do
    Matrix[ *@data ].row(0) { |x| x }.should == [1, 2]
  end
end