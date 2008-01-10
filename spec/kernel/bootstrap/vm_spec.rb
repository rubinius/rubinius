require File.dirname(__FILE__) + '/../../spec_helper'

describe "VM#coerce_to_array" do
  it "when passed an Array, returns it" do
    array = [1]
    VM.coerce_to_array(array).equal?(array).should == true
  end

  it "when passed an Object that implements #to_a and returns an Array, returns the Array" do
    obj = mock('[1,2,3]')
    def obj.to_a
      [1, 2, 3]
    end
    VM.coerce_to_array(obj).should == [1, 2, 3]
  end

  it "when passed an Object that implements #to_a and does not return an Array, raises a TypeError" do
    obj = mock('x')
    def obj.to_a
      nil
    end
    type_error_raised = false
    begin
      VM.coerce_to_array(obj)
    rescue TypeError => e
      type_error_raised = true
    end
    type_error_raised.should == true
  end

#  it "when passed an Object that uses implement to_a, returns an Array containing that Object" do
#    obj = mock('x')
#    class << obj
#      remove_method :to_a
#    end
#    obj.respond_to?(:to_a).should == false
#    VM.coerce_to_array(obj).should == [obj]
#  end

  it "when passed an Object that uses Kernel#to_a, returns an Array containing that Object" do
    obj = mock('x')
    # TODO: NS/BT - Make Method#initialize take a Module instead of a IncludedModule 
    obj.method(:to_a).module.module.should == Kernel
    VM.coerce_to_array(obj).should == [obj]
  end
end
