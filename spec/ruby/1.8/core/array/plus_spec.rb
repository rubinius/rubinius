require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#+" do
  it "concatenates two arrays" do
    ([ 1, 2, 3 ] + [ 3, 4, 5 ]).should == [1, 2, 3, 3, 4, 5]
    ([ 1, 2, 3 ] + []).should == [1, 2, 3]
    ([] + [ 1, 2, 3 ]).should == [1, 2, 3]
    ([] + []).should == []
  end

  it "calls to_ary on its argument" do
    obj = mock('["x", "y"]')
    def obj.to_ary() ["x", "y"] end
    ([1, 2, 3] + obj).should == [1, 2, 3] + obj.to_ary
    
    obj = mock('[:x]')
    obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_ary).and_return([:x])
    ([1, 2, 3] + obj).should == [1, 2, 3, :x]
  end

  it "does return subclass instances with Array subclasses" do
    (ArraySpecs::MyArray[1, 2, 3] + []).class.should == Array
    (ArraySpecs::MyArray[1, 2, 3] + ArraySpecs::MyArray[]).class.should == Array
    ([1, 2, 3] + ArraySpecs::MyArray[]).class.should == Array
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6] + ArraySpecs::ToAryArray[1, 2]).should == [5, 6, 1, 2]
  end
end
