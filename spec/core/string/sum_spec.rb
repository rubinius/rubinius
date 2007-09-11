require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#sum" do
  it "returns a basic n-bit checksum of the characters in self" do
    "ruby".sum.should == 450
    "ruby".sum(8).should == 194
    "rubinius".sum(23).should == 881
  end
  
  it "tries to convert n to an integer using to_int" do
    obj = Object.new
    def obj.to_int() 8 end
    
    "hello".sum(obj).should == "hello".sum(obj.to_int)

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 8)

    "hello".sum(obj).should == "hello".sum(8)
  end
end
