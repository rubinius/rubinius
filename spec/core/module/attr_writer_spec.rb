require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#attr_writer" do
  it "creates a setter for each given attribute name" do
    c = Class.new do
      attr_writer :a, :b.to_i, "c"
    end
    
    o = c.new
    %w{a c}.each do |x|
      o.respond_to?(x).should == false
      o.respond_to?("#{x}=").should == true
    end

    compliant :mri do
      o.respond_to?('b').should == false
      o.respond_to?("b=").should == true
    end
    
    o.a = "test"
    o.instance_variable_get(:@a).should == "test"

    compliant :mri do
      o.b = "test2"
      o.instance_variable_get(:@b).should == "test2"
    end

    o.c = "test3"
    o.instance_variable_get(:@c).should == "test3"
  end

  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "test", :count => :any)
    c = Class.new do
      attr_writer o
    end
    
    c.new.respond_to?("test").should == false
    c.new.respond_to?("test=").should == true
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    lambda { Class.new { attr_writer o } }.should raise_error(TypeError)
    (o = Object.new).should_receive(:to_str, :returning => 123)
    lambda { Class.new { attr_writer o } }.should raise_error(TypeError)
  end
end
