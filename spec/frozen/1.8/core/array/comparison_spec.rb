require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#<=>" do
  it "calls <=> left to right and return first non-0 result" do
    [-1, +1, nil, "foobar"].each do |result|
      lhs = Array.new(3) { mock("#{result}") }
      rhs = Array.new(3) { mock("#{result}") }
    
      lhs[0].should_receive(:<=>).with(rhs[0]).and_return(0)
      lhs[1].should_receive(:<=>).with(rhs[1]).and_return(result)
      lhs[2].should_not_receive(:<=>)

      (lhs <=> rhs).should == result
    end
  end
  
  it "returns 0 if the arrays are equal" do
    ([] <=> []).should == 0
    ([1, 2, 3, 4, 5, 6] <=> [1, 2, 3, 4, 5.0, 6.0]).should == 0
  end
  
  it "returns -1 if the array is shorter than the other array" do
    ([] <=> [1]).should == -1
    ([1, 1] <=> [1, 1, 1]).should == -1
  end

  it "returns +1 if the array is longer than the other array" do
    ([1] <=> []).should == +1
    ([1, 1, 1] <=> [1, 1]).should == +1
  end

  it "tries to convert the passed argument to an Array using #to_ary" do
    obj = mock('to_ary')
    obj.stub!(:to_ary).and_return([1, 2, 3])
    ([4, 5] <=> obj).should == ([4, 5] <=> obj.to_ary)
  end
  
  it "checks whether the passed argument responds to #to_ary" do
    obj = mock('method_missing to_ary')
    obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_ary).and_return([4, 5])
    ([4, 5] <=> obj).should == 0
  end  

  it "does not call #to_ary on Array subclasses" do
    obj = ArraySpecs::ToAryArray[5, 6, 7]
    obj.should_not_receive(:to_ary)
    ([5, 6, 7] <=> obj).should == 0
  end
end
