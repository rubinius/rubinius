require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#sum" do
  it "returns a basic n-bit checksum of the characters in self" do
    "ruby".sum.should == 450
    "ruby".sum(8).should == 194
    "rubinius".sum(23).should == 881
  end
  
  it "tries to convert n to an integer using to_int" do
    obj = mock('8')
    def obj.to_int() 8 end
    
    "hello".sum(obj).should == "hello".sum(obj.to_int)

    obj = mock('8')
    obj.should_receive(:respond_to?).with(:to_int).any_number_of_times.and_return(true)
    obj.should_receive(:method_missing).with(:to_int).and_return(8)

    "hello".sum(obj).should == "hello".sum(8)
  end
end
