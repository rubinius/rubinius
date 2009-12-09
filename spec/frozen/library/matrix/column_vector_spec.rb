require File.dirname(__FILE__) + '/../../spec_helper'
require 'matrix'

describe "Matrix.column_vector" do

  it "returns a single column Matrix when called with an Array" do
    m = Matrix.column_vector([4,5,6])
    m.should be_an_instance_of(Matrix)
    m.should == Matrix[ [4],[5],[6] ]
  end

  ruby_bug "redmine:1532", "1.8.7" do
    it "returns an empty Matrix when called with an empty Array" do
      m = Matrix.column_vector([])
      m.should be_an_instance_of(Matrix)
      m.row_size.should == 0
      m.column_size.should == 1
    end
  end
end
