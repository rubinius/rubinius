require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#<<" do
  it "concatenates the given argument to self and returns self" do
    str = 'hello '
    (str << 'world').equal?(str).should == true
    str.should == "hello world"
  end
  
  it "converts the given argument to a String using to_str" do
    obj = Object.new
    def obj.to_str() "world!" end
    a = 'hello ' << obj
    a.should == 'hello world!'
    
    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_str], :count => :any, :returning => true)
    obj.should_receive(:method_missing, :with => [:to_str], :returning => "world!")
    a = 'hello ' << obj
    a.should == 'hello world!'
  end
  
  it "raises a TypeError if the given argument can't be converted to a String" do
    should_raise(TypeError) do
      a = 'hello ' << :world
    end

    should_raise(TypeError) do
      a = 'hello ' << Object.new
    end
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze

    should_raise(TypeError) { a << "" }
    should_raise(TypeError) { a << "test" }
  end
  
  it "works when given a subclass instance" do
    a = "hello"
    a << MyString.new(" world")
    a.should == "hello world"
  end
  
  it "taints self if other is tainted" do
    x = "x"
    (x << "".taint).tainted?.should == true

    x = "x"
    (x << "y".taint).tainted?.should == true
  end
end

describe "String#<< with Fixnum" do
  it "converts the given Fixnum to a char before concatenating" do
    b = 'hello ' << 'world' << 33
    b.should == "hello world!"
    b << 0
    b.should == "hello world!\x00"
  end

  it "doesn't use fixnum % 256" do
    should_raise(TypeError) { "" << (256 * 3 + 64) }
    should_raise(TypeError) { "" << -200 }
  end
  
  it "raises a TypeError when the given Fixnum is not between 0 and 255" do
    should_raise(TypeError) do
      "hello world" << 333
    end
  end

  it "doesn't call to_int on its argument" do
    x = Object.new
    x.should_not_receive(:to_int)
    
    should_raise(TypeError) { "" << x }
  end

  it "raises a TypeError when self is frozen" do
    a = "hello"
    a.freeze

    should_raise(TypeError) { a << 0 }
    should_raise(TypeError) { a << 33 }
  end
end
