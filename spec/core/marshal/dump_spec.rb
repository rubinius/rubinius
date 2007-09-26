require File.dirname(__FILE__) + '/../../spec_helper'

describe "Marshal.dump when given a recursion limit" do
  it "raises an ArgumentError when the recursion limit is exceeded" do
    Marshal.dump([], 1)
    Marshal.dump([[]], 2)
    Marshal.dump([[[]]], 3)
    
    should_raise(ArgumentError, "exceed depth limit") do
      Marshal.dump([], 0)
    end

    should_raise(ArgumentError, "exceed depth limit") do
      Marshal.dump([[[]]], 1)
    end
  end
  
  it "ignores the recursion limit if the limit is negative" do
    Marshal.dump([], -1)
    Marshal.dump([[]], -1)
    Marshal.dump([[[]]], -1)
  end
end

describe "Marshal.dump when given an IO-Object" do
  it "should write the serialized data to the IO-Object" do
    (obj = Object.new).should_receive(:write)
    Marshal.dump("test", obj)
  end

  it "returns the IO-Object" do
    (obj = Object.new).should_receive(:write)
    Marshal.dump("test", obj).should == obj
  end
  
  it "raises an Error when the IO-Object does not respond to #write" do
    obj = Object.new
    should_raise(TypeError, "instance of IO needed") do
      Marshal.dump("test", obj)
    end
  end
end

describe "Marshal.dump" do
  it "should be provided" do
    Marshal.respond_to?(:dump).should == true
  end
  
  it "returns a string-serialized version of the given argument" do
    Marshal.dump("test").is_a?(String).should == true
  end

  it "raises an Error when trying to dump an anonymous class/module" do
    klass = Class.new
    mod = Module.new
    
    should_raise(TypeError, "can't dump anonymous class #{klass}") do
      Marshal.dump(klass)
    end

    should_raise(TypeError, "can't dump anonymous module #{mod}") do
      Marshal.dump(mod)
    end
  end
end