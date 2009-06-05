require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#initialize" do
  ruby_version_is ""..."1.9" do
    it "is a private method" do
      "".private_methods.should include("initialize")
    end
  end

  ruby_version_is "1.9" do
    it "is a private method" do
      "".private_methods.should include(:initialize)
    end
  end

  it "replaces contents of self with the passed string" do
    s = "some string"
    id = s.object_id
    s.send :initialize, "another string"
    s.should == "another string"
    s.object_id.should == id
  end

  it "does not change self when not passed a string" do
    s = "some string"
    s.send :initialize
    s.should == "some string"
  end

  it "replaces the taint status of self with that of the passed string" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.send :initialize, b
    a.tainted?.should == true
  end

  it "returns an instance of a subclass" do
    a = StringSpecs::MyString.new("blah")
    a.should be_kind_of(StringSpecs::MyString)
    a.should == "blah"
  end

  it "is called on subclasses" do
    s = StringSpecs::SubString.new
    s.special.should == nil
    s.should == ""

    s = StringSpecs::SubString.new "subclass"
    s.special.should == "subclass"
    s.should == ""
  end

  it "converts its argument to a string representation" do
    obj = mock 'foo'
    obj.should_receive(:to_str).and_return('foo')

    String.new obj
  end

  it "raises TypeError on inconvertible object" do
    lambda { String.new 5 }.should raise_error(TypeError)
    lambda { String.new nil }.should raise_error(TypeError)
  end

  ruby_version_is ""..."1.9" do 
    it "raises a TypeError if self is frozen" do
      a = "hello".freeze

      a.send :initialize, a
      lambda { a.send :initialize, "world" }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9" do   
    it "raises a RuntimeError if self is frozen" do
      a = "hello".freeze

      a.send :initialize, a
      lambda { a.send :initialize, "world" }.should raise_error(RuntimeError)
    end
  end  
end
