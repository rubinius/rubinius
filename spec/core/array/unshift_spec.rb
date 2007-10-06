require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#unshift" do
  it "prepends object to the original array" do
    a = [1, 2, 3]
    a.unshift("a").equal?(a).should == true
    a.should == ['a', 1, 2, 3]
    a.unshift().equal?(a).should == true
    a.should == ['a', 1, 2, 3]
    a.unshift(5, 4, 3)
    a.should == [5, 4, 3, 'a', 1, 2, 3]
  end

  compliant :mri do
    it "raises TypeError on a frozen array" do
      should_raise(TypeError) { @frozen_array.unshift(1) }
    end  
    
    it "does not raise TypeError on a frozen array if no modification takes place" do
      @frozen_array.unshift() # ok
    end
  end
end
