require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#attr" do
  it "creates a getter for the given attribute name" do
    c = Class.new do
      attr :attr
      attr "attr3"
      
      def initialize
        @attr, @attr2, @attr3 = "test", "test2", "test3"
      end
    end
    
    o = c.new
    
    %w{attr attr3}.each do |a|
      o.respond_to?(a).should == true
      o.respond_to?("#{a}=").should == false
    end

    compliant_on :ruby do
      o.respond_to?('attr2').should == true
      o.respond_to?("attr2=").should == false
    end

    o.attr.should  == "test"

    compliant_on :ruby do
      o.attr2.should == "test2"
    end

    o.attr3.should == "test3"
  end
  
  it "creates a setter for the given attribute name if writable is true" do
    c = Class.new do
      attr :attr, true
      attr "attr3", true
      
      def initialize
        @attr, @attr2, @attr3 = "test", "test2", "test3"
      end
    end
    
    o = c.new
    
    %w{attr attr3}.each do |a|
      o.respond_to?(a).should == true
      o.respond_to?("#{a}=").should == true
    end
    
    compliant_on :ruby do
      o.respond_to?('attr2').should == true
      o.respond_to?("attr2=").should == true
    end

    o.attr = "test updated"

    compliant_on :ruby do
      o.attr2 = "test2 updated"
    end

    o.attr3 = "test3 updated"
  end
  
  it "creates a getter and setter for the given attribute name if called with and without writeable is true" do
    c = Class.new do
      attr :attr, true
      attr :attr

      attr "attr3", true
      attr "attr3"
      
      def initialize
        @attr, @attr2, @attr3 = "test", "test2", "test3"
      end
    end
    
    o = c.new

    %w{attr attr3}.each do |a|
      o.respond_to?(a).should == true
      o.respond_to?("#{a}=").should == true
    end
    
    compliant_on :ruby do
      o.respond_to?('attr2').should == true
      o.respond_to?("attr2=").should == true
    end

    o.attr.should == "test"
    o.attr = "test updated"
    o.attr.should == "test updated"

    compliant_on :ruby do
      o.attr2.should == "test2"
      o.attr2 = "test2 updated"
      o.attr2.should == "test2 updated"
    end

    o.attr3.should == "test3"
    o.attr3 = "test3 updated"
    o.attr3.should == "test3 updated"
  end

  it "converts non string/symbol/fixnum names to strings using to_str" do
    (o = mock('test')).should_receive(:to_str).any_number_of_times.and_return("test")
    Class.new { attr o }.new.respond_to?("test").should == true
  end
  
  it "raises a TypeError when the given names can't be converted to strings using to_str" do
    o = mock('o')
    lambda { Class.new { attr o } }.should raise_error(TypeError)
    (o = mock('123')).should_receive(:to_str).and_return(123)
    lambda { Class.new { attr o } }.should raise_error(TypeError)
  end
end
