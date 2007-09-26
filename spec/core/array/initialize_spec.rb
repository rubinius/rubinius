require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#initialize" do
  it "is private" do
    [].private_methods.map { |m| m.to_s }.include?("initialize").should == true
  end
  
  it "does nothing when passed an object equal to self" do
    ary = [1, 2, 3]
    ary.instance_eval { initialize([1, 2, 3]) }
    ary.should == [1, 2, 3]
  end
  
  it "does nothing when passed self" do
    ary = [1, 2, 3]
    ary.instance_eval { initialize(ary) }
    ary.should == [1, 2, 3]
  end
  
  it "sets the array to size objects when passed size, object" do
    [].instance_eval { initialize(2, [3]) }.should == [[3], [3]]
    [].instance_eval { initialize(1) }.should == [nil]
  end
  
  it "raises ArgumentError if size is negative" do
    should_raise(ArgumentError) { [].instance_eval { initialize(-1, :a) } }
    should_raise(ArgumentError) { [1, 2, 3].instance_eval { initialize(-1) } }
  end
  
  it "calls to_int on array size" do
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 1)
    
    [1, 2].instance_eval { initialize(obj, :a) }
  end
  
  it "does not raise TypeError on a frozen array if it would not change the array" do
    @frozen_array.instance_eval { initialize() }.should == @frozen_array
  end

  it "raises TypeError on frozen arrays" do
    should_raise(TypeError) { @frozen_array.instance_eval { initialize(1) } }
    should_raise(TypeError) { @frozen_array.instance_eval { initialize([1, 2, 3]) } }
  end
end
