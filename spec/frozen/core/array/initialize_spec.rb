require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#initialize" do
  it "is private" do
    [].private_methods.map{|name| name.to_s}.should include("initialize")
  end

  it "raise an ArgumentError if 3 or more arguments passed and no block given" do
    lambda { [1, 2].send(:initialize) }.should_not raise_error(ArgumentError)
    lambda { [1, 2].send(:initialize, 1) }.should_not raise_error(ArgumentError)
    lambda { [1, 2].send(:initialize, 1, 'x') }.should_not raise_error(ArgumentError)
    lambda { [1, 2].send(:initialize, 1, 'x', true) }.should raise_error(ArgumentError)
  end

  it "raise an ArgumentError if 3 or more arguments passed and a block given" do
    lambda { [1, 2].send(:initialize){} }.should_not raise_error(ArgumentError)
    lambda { [1, 2].send(:initialize, 1){} }.should_not raise_error(ArgumentError)
    lambda { [1, 2].send(:initialize, 1, 'x'){} }.should_not raise_error(ArgumentError)
    lambda { [1, 2].send(:initialize, 1, 'x', true){} }.should raise_error(ArgumentError)
  end
  
  compliant_on :ruby, :jruby, :ir do
    ruby_version_is '' ... '1.9' do
      it "raises a TypeError on frozen arrays even if the array would not be 'modified'" do
        # This is true at least 1.8.6p111 onwards 
        lambda { ArraySpecs.frozen_array.send(:initialize) }.should raise_error(TypeError)

        lambda { ArraySpecs.frozen_array.send(:initialize, 1) }.should raise_error(TypeError)
        lambda { ArraySpecs.frozen_array.send(:initialize, [1, 2, 3]) }.should raise_error(TypeError)
      end
    end
    ruby_version_is '1.9' do
      it "raises a RuntimeError on frozen arrays even if the array would not be 'modified'" do
        lambda { ArraySpecs.frozen_array.send(:initialize) }.should raise_error(RuntimeError)

        lambda { ArraySpecs.frozen_array.send(:initialize, 1) }.should raise_error(RuntimeError)
        lambda { ArraySpecs.frozen_array.send(:initialize, [1, 2, 3]) }.should raise_error(RuntimeError)
      end
    end
  end
end

describe "Array#initialize with no arguments" do
  it "returns self" do
    a = [1, 2, 3]
    a.send(:initialize).should equal(a)
  end

  it "makes the array empty" do
    [1, 2, 3].send(:initialize).should be_empty
  end

  it "does not use the given block" do
    lambda{ [1, 2, 3].send(:initialize) { raise } }.should_not raise_error
  end
end

describe "Array#initialize with (size, object)" do
  it "is called on subclasses" do
    a = ArraySpecs::SubArray.new 10
    a.special.should == 10
    a.should == []
  end

  it "sets the array to size and fills with the object" do
    a = []
    a.send(:initialize, 2, [3])
    a.should == [[3], [3]]
    a[0].should equal(a[1])
  end

  it "sets the array to size and fills with nil when object is omitted" do
    [].send(:initialize, 3).should == [nil, nil, nil]
  end
  
  it "raises an ArgumentError if size is negative" do
    lambda { [].send(:initialize, -1, :a) }.should raise_error(ArgumentError)
    lambda { [1, 2, 3].send(:initialize, -1) }.should raise_error(ArgumentError)
  end

  platform_is :wordsize => 32 do
    it "raises an ArgumentError if size is too large" do
      lambda { [].send(:initialize, 2**32/4+1) }.should raise_error(ArgumentError, /size/)
    end
  end
  platform_is :wordsize => 64 do
    it "raises an ArgumentError if size is too large" do
      lambda { [].send(:initialize, 2**64/8+1) }.should raise_error(ArgumentError, /size/)
    end
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

  it "raises a TypeError if the passed size is no numeric" do
    obj = mock('nonnumeric')
    obj.should_receive(:respond_to?).with(:to_int).and_return(false)
    lambda{ [1, 2].send(:initialize, obj) }.should raise_error(TypeError)
  end

  it "yields the given block size times passing an index and fills self with values of the block" do
    [1, 2].send(:initialize, 5){|i| "#{i}"}.should == ['0', '1', '2', '3', '4']
  end

  it "yields the given block size times passing an index and fills self with values of the block even if a filler value passed" do
    [1, 2].send(:initialize, 5, 'filler'){|i| "#{i}"}.should == ['0', '1', '2', '3', '4']
  end

  it "returns the specified value if it would break in the given block" do
    [1, 2].send(:initialize, 5){ break :a }.should == :a
  end

  it "makes the array contain values the given block would yield even if it would break in the block" do
    ary = [1, 2, 3, 4, 5]
    ary.send(:initialize, 7) {|i|
      break :a if i == 2
      "#{i}"
    }
    ary.should == ['0', '1']
  end
end

describe "Array#initialize with (array)" do
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
    b = ArraySpecs::SubArray.new [1,2,3]
    b.special.should == [1,2,3]
    b.should == []
  end
  
  it "does nothing when passed self" do
    ary = [1, 2, 3]
    ary.send(:initialize, ary)
    ary.should == [1, 2, 3]
  end

  it "does not use the given block" do
    lambda{ [1, 2, 3].send(:initialize) { raise } }.should_not raise_error
  end
end
