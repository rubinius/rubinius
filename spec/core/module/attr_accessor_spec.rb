require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#attr_accessor" do
  it "creates a getter and setter for each given attribute name" do
    c = Class.new do
      attr_accessor :a, :b.to_i, "c"
    end
    
    o = c.new
    
    ['a','c'].each do |x|
      o.respond_to?(x).should == true
      o.respond_to?("#{x}=").should == true
    end
    
    compliant :mri do
      o.respond_to?('b').should == true
      o.respond_to?("b=").should == true
    end

    o.a = "a"
    o.a.should == "a"

    compliant :mri do
      o.b = "b"
      o.b.should == "b"
    end

    o.c = "c"
    o.c.should == "c"
  end
  
  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str).any_number_of_times.and_return("test")
    c = Class.new do
      attr_accessor o
    end
    
    c.new.respond_to?("test").should == true
    c.new.respond_to?("test=").should == true
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    lambda { Class.new { attr_accessor o } }.should raise_error(TypeError)
    (o = Object.new).should_receive(:to_str).and_return(123)
    lambda { Class.new { attr_accessor o } }.should raise_error(TypeError)
  end
end
