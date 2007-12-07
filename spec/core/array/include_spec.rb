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
    one = mock('one')
    two = mock('two')
    three = mock('three')
    one.should_receive(:==).with(key).and_return(false)
    one.should_receive(:==).with(two).and_return(false) # this is to work around == being called in should_receive
    two.should_receive(:==).with(key).and_return(true)
    three.should_not_receive(:==)
    
    
    
    # ary = [one, two, three]
    # 
    # ary = []Array.new(3) { Object.new }
    # ary[0].should_receive(:==).with(key).and_return(false)
    # ary[1].should_receive(:==).with(key).and_return(true)
    # ary[2].should_not_receive(:==)
    
    # ary.include?(key).should == true
  end
end
