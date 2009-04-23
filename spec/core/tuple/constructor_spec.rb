require File.dirname(__FILE__) + '/../../spec_helper'

describe "Rubinius::Tuple.[]" do
  it "creates a new Tuple containing the elements provided" do
    t = Rubinius::Tuple[:one, 2, 'three']
    t[0].should == :one
    t[1].should == 2
    t[2].should == 'three'
  end
end
