require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#empty?" do
  it "returns true if the hash has no entries" do
    {}.empty?.should == true
    {1 => 1}.empty?.should == false
    Hash.new(5).empty?.should == true
    Hash.new { 5 }.empty?.should == true
  end
end
