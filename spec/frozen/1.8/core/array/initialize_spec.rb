require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#initialize" do
  it "is private" do
    [].private_methods.should include("initialize")
  end
  
  it "replaces self with the other array" do
    o = [2]
    def o.special() size end
    a = [1, o, 3]
    ary = Array.new a
    ary[1].special.should == 1
    
    b = [1, [2], 3]
    ary.send :initialize, b
    
    b.==(ary).should == true
    lambda { b[1].special }.should raise_error(NoMethodError)
    lambda { ary[1].special }.should raise_error(NoMethodError)
  end
  
  it "is called on subclasses" do
    a = ArraySpecs::SubArray.new 10
    a.special.should == 10
    a.should == []
    
    b = ArraySpecs::SubArray.new [1,2,3]
    b.special.should == [1,2,3]
    b.should == []
  end
  
  it "does nothing when passed self" do
    ary = [1, 2, 3]
    ary.instance_eval { initialize(ary) }
    ary.should == [1, 2, 3]
  end
  
  it "sets the array to size and fills with the object when passed size, object" do
    a = []
    a.instance_eval { initialize(2, [3]) }
    a.should == [[3], [3]]
    a[0].__id__.should == a[1].__id__
    [].instance_eval { initialize(1) }.should == [nil]
  end
  
  it "raises an ArgumentError if size is negative" do
    lambda { [].instance_eval { initialize(-1, :a) } }.should raise_error(ArgumentError)
    lambda { [1, 2, 3].instance_eval { initialize(-1) } }.should raise_error(ArgumentError)
  end

  it "tries to convert the passed size argument to an Integer using #to_int" do
    obj = mock('1')
    obj.should_receive(:to_int).and_return(1)
    [1, 2].send(:initialize, obj, :a).should == [:a]
  end

  it "checks whether the passed size argument responds to #to_int" do
    obj = mock('1')
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).and_return(1)
    [1, 2].send(:initialize, obj, :a).should == [:a]
  end

  compliant_on :ruby, :jruby do
    it "raises a TypeError on frozen arrays even if the array would not be 'modified'" do
      # This is true at least 1.8.6p111 onwards 
      lambda { ArraySpecs.frozen_array.instance_eval { initialize } }.should raise_error(TypeError)

      lambda { ArraySpecs.frozen_array.instance_eval { initialize(1) } }.should raise_error(TypeError)
      lambda { ArraySpecs.frozen_array.instance_eval { initialize([1, 2, 3]) } }.should raise_error(TypeError)
    end
  end
end
