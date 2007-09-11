require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#== with String" do
  it "returns true if self <=> string returns 0" do
    ('hello' == 'hello').should == true
  end
  
  it "returns false if self <=> string does not return 0" do
    ("more" == "MORE").should == false
    ("less" == "greater").should == false
  end
  
  it "ignores subclass differences" do
    a = "hello"
    b = MyString.new("hello")
    
    (a == b).should == true
    (b == a).should == true
  end  
end

describe "String#==" do
  it "returns false if obj does not respond to to_str" do
    ('hello' == 5).should == false
    ('hello' == :hello).should == false
    ('hello' == Object.new).should == false
  end
  
  it "returns obj == self if obj responds to to_str" do
    obj = Object.new
    def obj.to_str() "world!" end
    def obj.==(other) true end

    ('hello' == obj).should == true
    ('world!' == obj).should == true 
    
    obj = Object.new
    class << obj; undef :==; end
    other = "abc"
    obj.should_receive(:respond_to?, :with => [:to_str], :returning => true)
    obj.should_receive(:method_missing, :with => [:==, other], :returning => true)
    (other == obj).should == true
  end
end
