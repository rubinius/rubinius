require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#unshift" do
  it "prepends object to the original array" do
    a = [1, 2, 3]
    a.unshift("a").should equal(a)
    a.should == ['a', 1, 2, 3]
    a.unshift().should equal(a)
    a.should == ['a', 1, 2, 3]
    a.unshift(5, 4, 3)
    a.should == [5, 4, 3, 'a', 1, 2, 3]

    # shift all but one element
    a = [1, 2]
    a.shift
    a.unshift(3, 4)
    a.should == [3, 4, 2]

    # now shift all elements
    a.shift
    a.shift
    a.shift
    a.unshift(3, 4)
    a.should == [3, 4]
  end
  
  it "quietly ignores unshifting nothing" do
    [].unshift().should == []
    [].unshift(*[]).should == []
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on a frozen array" do
      lambda { ArraySpecs.frozen_array.unshift(1) }.should raise_error(TypeError)
    end  
    
    it "does not raise TypeError on a frozen array if no modification takes place" do
      ArraySpecs.frozen_array.unshift.should == [1, 2, 3]
    end
  end
end
