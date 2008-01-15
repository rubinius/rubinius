require File.dirname(__FILE__) + '/../../../spec_helper'

describe "Rubinius::VM#coerce_to_array" do
  it "returns the Array passed to it" do
    array = [1]
    Rubinius::VM.coerce_to_array(array).equal?(array).should == true
  end

  it "returns the Array provided by an Object that implements #to_a and returns an Array" do
    obj = mock('[1,2,3]')
    def obj.to_a
      [1, 2, 3]
    end
    Rubinius::VM.coerce_to_array(obj).should == [1, 2, 3]
  end

  it "raises a TypeError when passed an Object that implements #to_a and does not return an Array" do
    obj = mock('x')
    def obj.to_a
      nil
    end
    type_error_raised = false
    begin
      Rubinius::VM.coerce_to_array(obj)
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
#    Rubinius::VM.coerce_to_array(obj).should == [obj]
#  end

  it "returns an Array containing that Object when passed an Object that uses Kernel#to_a" do
    obj = mock('x')
    # TODO: NS/BT - Make Method#initialize take a Module instead of a IncludedModule
    obj.method(:to_a).module.module.should == Kernel
    Rubinius::VM.coerce_to_array(obj).should == [obj]
  end
end
