require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Hash#keys" do
  it "returns an array populated with keys" do
    {}.keys.should == []
    {}.keys.class.should == Array
    Hash.new(5).keys.should == []
    Hash.new { 5 }.keys.should == []
    { 1 => 2, 2 => 4, 4 => 8 }.keys.should == [1, 2, 4]
    { 1 => 2, 2 => 4, 4 => 8 }.keys.class.should == Array
    { nil => nil }.keys.should == [nil]
  end

  it "it uses the same order as #values" do
    h = { 1 => "1", 2 => "2", 3 => "3", 4 => "4" }
    
    h.size.times do |i|
      h[h.keys[i]].should == h.values[i]
    end
  end
end
