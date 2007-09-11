require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#concat" do
  it "is an alias of String#<<" do
    str = 'hello '
    (str.concat('world')).equal?(str).should == true
    str.should == "hello world"
    str2 = 'hello '
    str2 << 'world'
    str.should == str2
  end

  it "taints self if other is tainted" do
    a = "x"
    (a << "".taint).tainted?.should == true

    a = "x"
    (a << "y".taint).tainted?.should == true
  end

  it "doesn't use fixnum % 256" do
    should_raise(TypeError) { "".concat(256 * 3 + 64) }
    should_raise(TypeError) { "".concat(-200) }
  end
  
  it "raises a TypeError when self is frozen" do
    s = "hello"
    s.freeze
    
    should_raise(TypeError) { s.concat("") }
    should_raise(TypeError) { s.concat("foo") }
    should_raise(TypeError) { s.concat(0) }
  end
end
