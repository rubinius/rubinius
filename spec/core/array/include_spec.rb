require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#include?" do
  it "returns true if object is present, false otherwise" do
    [1, 2, "a", "b"].include?("c").should == false
    [1, 2, "a", "b"].include?("a").should == true
  end

  it "determines presence by using element == obj" do
    o = Object.new
  
    [1, 2, "a", "b"].include?(o).should == false

    def o.==(other); other == 'a'; end

    [1, 2, o, "b"].include?('a').should == true
  end

  it "calls == on elements from left to right until success" do
    key = "x"
    ary = Array.new(3) { Object.new }
    ary[0].should_receive(:==).with(key).and_return(false)
    ary[1].should_receive(:==).with(key).and_return(true)
    ary[2].should_not_receive(:==)
    
    ary.include?(key).should == true
  end
end
