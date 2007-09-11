require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#delete" do
  it "removes elements that are #== to object" do
    x = Object.new
    def x.==(other) 3 == other end

    a = [1, 2, 3, x, 4, 3, 5, x]
    a.delete Object.new
    a.should == [1, 2, 3, x, 4, 3, 5, x]

    a.delete 3
    a.should == [1, 2, 4, 5]
  end

  it "returns object or nil if no elements match object" do
    [1, 2, 4, 5].delete(1).should == 1
    [1, 2, 4, 5].delete(3).should == nil
  end

  it "may be given a block that is executed if no element matches object" do
    [].delete('a') {:not_found}.should == :not_found
  end
  
compliant :r18 do
  it "raises TypeError on a frozen array if a modification would take place" do
    should_raise(TypeError) { frozen_array.delete(1) }
  end

  it "does not raise on a frozen array if a modification would not take place" do
    should_raise(TypeError) { frozen_array.delete(0) }
  end
end
  
noncompliant :rubinius do
  it "raises TypeError on a frozen array" do
    should_raise(TypeError) { frozen_array.delete(0) }
    should_raise(TypeError) { frozen_array.delete(1) }
  end
end
end
