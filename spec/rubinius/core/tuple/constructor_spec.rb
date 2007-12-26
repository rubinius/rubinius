require File.dirname(__FILE__) + '/../../../spec_helper'

extension :rubinius do
  describe "Tuple.[]" do
    it "creates a new Tuple containing the elements provided" do
      t = Tuple[:one, 2, 'three']
      t[0].should == :one
      t[1].should == 2
      t[2].should == 'three'
    end
  end
end
