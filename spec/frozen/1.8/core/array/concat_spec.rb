require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#concat" do
  it "appends the elements in the other array" do
    ary = [1, 2, 3]
    ary.concat([9, 10, 11]).should equal(ary)
    ary.should == [1, 2, 3, 9, 10, 11]
    ary.concat([])
    ary.should == [1, 2, 3, 9, 10, 11]
  end
  
  it "does not loop endlessly when argument is self" do
    ary = ["x", "y"]
    ary.concat(ary).should == ["x", "y", "x", "y"]
  end  

  it "tries to convert the passed argument to an Array using #to_ary" do
    obj = mock('to_ary')
    obj.should_receive(:to_ary).and_return(["x", "y"])
    [4, 5, 6].concat(obj).should == [4, 5, 6, "x", "y"]
  end

  it "checks whether the passed argument responds to #to_ary" do
    obj = mock('method_missing to_ary')
    obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_ary).and_return([:x])
    [].concat(obj).should == [:x]
  end

  it "does not call #to_ary on Array subclasses" do
    obj = ArraySpecs::ToAryArray[5, 6, 7]
    obj.should_not_receive(:to_ary)
    [].concat(obj).should == [5, 6, 7]
  end
  
  compliant_on :ruby, :jruby do
    it "raises a TypeError when Array is frozen and modification occurs" do
      lambda { ArraySpecs.frozen_array.concat [1] }.should raise_error(TypeError)
    end

    it "does not raise a TypeError when Array is frozen but no modification occurs" do
      ArraySpecs.frozen_array.concat([]).should == [1, 2, 3]
    end
  end
end
