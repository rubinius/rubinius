require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#zip" do
  it "returns an array of arrays containing corresponding elements of each array" do
    [1, 2, 3, 4].zip(["a", "b", "c", "d", "e"]).should ==
      [[1, "a"], [2, "b"], [3, "c"], [4, "d"]]
  end
  
  it "fills in missing values with nil" do
    [1, 2, 3, 4, 5].zip(["a", "b", "c", "d"]).should ==
      [[1, "a"], [2, "b"], [3, "c"], [4, "d"], [5, nil]]
  end

  it "properly handles recursive arrays" do
    a = []; a << a
    b = [1]; b << b

    a.zip(a).should == [ [a[0], a[0]] ]
    a.zip(b).should == [ [a[0], b[0]] ]
    b.zip(a).should == [ [b[0], a[0]], [b[1], a[1]] ]
    b.zip(b).should == [ [b[0], b[0]], [b[1], b[1]] ]
  end

  # MRI 1.8.6 uses to_ary, but it's been fixed in 1.9
  compliant_on(:ruby, :jruby) do
    it "tries to convert the passed argument to an Array using #to_ary" do
      obj = mock('[3,4]')
      obj.should_receive(:to_ary).and_return([3, 4])
      [1, 2].zip(obj).should == [[1, 3], [2, 4]]
    end

    it "checks whether the passed argument responds to #to_ary" do
      obj = mock('[3,4]')
      obj.should_receive(:respond_to?).with(:to_ary).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with(:to_ary).and_return([3, 4])
      [1, 2].zip(obj).should == [[1, 3], [2, 4]]
    end
  end
  
  compliant_on(:r19) do
    it "calls to_a on its arguments" do
      [1, 2, 3].zip("f" .. "z", 1 .. 9).should ==
        [[1, "f", 1], [2, "g", 2], [3, "h", 3]]
      
      obj = mock('[3,4]')
      obj.should_receive(:respond_to?).with(:to_a).any_number_of_times.and_return(true)
      obj.should_receive(:method_missing).with([:to_a]).and_return([3, 4])
    
      [1, 2].zip(obj).should == [[1, 3], [2, 4]]
    end
  end

  it "calls block if supplied" do
    values = []
    [1, 2, 3, 4].zip(["a", "b", "c", "d", "e"]) { |value|
      values << value
    }.should == nil
    
    values.should == [[1, "a"], [2, "b"], [3, "c"], [4, "d"]]
  end
  
  it "does not return subclass instance on Array subclasses" do
    ArraySpecs::MyArray[1, 2, 3].zip(["a", "b"]).class.should == Array
  end
end
