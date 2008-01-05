require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#delete" do
  it "removes elements that are #== to object" do
    x = mock('delete')
    def x.==(other) 3 == other end

    a = [1, 2, 3, x, 4, 3, 5, x]
    a.delete mock('not contained')
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
  
  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array if a modification would take place" do
      lambda { [1, 2, 3].freeze.delete(1) }.should raise_error(TypeError)
    end

    it "returns false on a frozen array if a modification does not take place" do
      [1, 2, 3].freeze.delete(0).should == nil
    end
  end
end
