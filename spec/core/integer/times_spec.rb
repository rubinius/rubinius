require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer#times" do
  it "passes block values from 0 to self - 1" do
    a = []
    9.times { |i| a << i }
    -2.times { |i| a << i }
    a.should == [0, 1, 2, 3, 4, 5, 6, 7, 8]
  end
end
