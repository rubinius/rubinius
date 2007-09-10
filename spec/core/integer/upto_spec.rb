require File.dirname(__FILE__) + '/../../spec_helper'

describe "Integer#upto" do
  it "passes block values from self up to and including other Integer" do
    a = []
    9.upto(13) { |i| a << i }
    2.upto(-1) { |i| a << i }
    a.should == [9, 10, 11, 12, 13]
  end
end
