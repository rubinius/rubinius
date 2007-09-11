require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#attr_reader" do
  it "creates a getter for each given attribute name" do
    c = Class.new do
      attr_reader :a, :b.to_i, "c"
      
      def initialize
        @a = "test"
        @b = "test2"
        @c = "test3"
      end
    end
    
    o = c.new
    ('a'..'c').each do |x|
      o.respond_to?(x).should == true
      o.respond_to?("#{x}=").should == false
    end
    
    o.a.should == "test"
    o.b.should == "test2"
    o.c.should == "test3"
  end

  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = Object.new).should_receive(:to_str, :returning => "test")
    c = Class.new do
      attr_reader o
    end
    
    c.new.respond_to?("test").should == true
    c.new.respond_to?("test=").should == false
  end

  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = Object.new
    should_raise(TypeError, "#{o} is not a symbol") do
      Class.new { attr_reader o }
    end
    
    (o = Object.new).should_receive(:to_str, :returning => 123)
    should_raise(TypeError, "Object#to_str should return String") do
      Class.new { attr_reader o }
    end
  end
end
