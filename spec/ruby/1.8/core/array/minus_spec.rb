require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#-" do
  it "creates an array minus any items from other array" do
    ([] - [ 1, 2, 4 ]).should == []
    ([1, 2, 4] - []).should == [1, 2, 4]
    ([ 1, 2, 3, 4, 5 ] - [ 1, 2, 4 ]).should == [3, 5]
  end

  it "removes multiple items on the lhs equal to one on the rhs" do
    ([1, 1, 2, 2, 3, 3, 4, 5] - [1, 2, 4]).should == [3, 3, 5]
  end

  it "calls to_ary on its argument" do
    obj = mock('[2,3,3,4]')
    def obj.to_ary() [2, 3, 3, 4] end
    ([1, 1, 2, 2, 3, 4] - obj).should == [1, 1]
    
    obj = mock('[2,3,4]')
    obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_ary).and_return([2, 3, 4])
    ([1, 1, 2, 2, 3, 4] - obj).should == [1, 1]
  end

  it "does not return subclass instance for Array subclasses" do
    (ArraySpecs::MyArray[1, 2, 3] - []).class.should == Array
    (ArraySpecs::MyArray[1, 2, 3] - ArraySpecs::MyArray[]).class.should == Array
    ([1, 2, 3] - ArraySpecs::MyArray[]).class.should == Array
  end

  it "does not call to_ary on array subclasses" do
    ([5, 6, 7] - ArraySpecs::ToAryArray[7]).should == [5, 6]
  end
end
