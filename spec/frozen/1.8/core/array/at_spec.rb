require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Array#at" do
  it "returns the element at the passed index" do
    a = [1, 2, 3, 4, 5, 6]
    a.at(0).should  == 1
    a.at(-2).should == 5
    a.at(10).should == nil
    a.at(-7).should == nil
  end
  
  it "tries to convert the passed argument to an Integer using #to_int" do
    a = ["a", "b", "c"]
    a.at(0.5).should == "a"
  
    obj = mock('to_int')
    obj.should_receive(:to_int).and_return(2)
    a.at(obj).should == "c"
  end
  
  it "checks whether the passed argument responds to #to_int" do
    obj = mock('method_missing to_int')
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).and_return(2)
    ["a", "b", "c"].at(obj).should == "c"
  end
  
  it "raises a TypeError when the passed argument can't be coerced to Integer" do
    lambda { [].at("cat") }.should raise_error(TypeError)
  end
end
