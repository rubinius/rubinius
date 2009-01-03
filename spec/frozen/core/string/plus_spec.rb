require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#+" do
  it "returns a new string containing the given string concatenated to self" do
    ("" + "").should == ""
    ("" + "Hello").should == "Hello"
    ("Hello" + "").should == "Hello"
    ("Ruby !" + "= Rubinius").should == "Ruby != Rubinius"
  end

  it "converts any non-String argument with #to_str" do
    c = mock 'str'
    c.should_receive(:to_str).any_number_of_times.and_return(' + 1 = 2')

    ("1" + c).should == '1 + 1 = 2'
  end

  it "raises a TypeError when given any object that fails #to_str" do
    lambda { "" + Object.new }.should raise_error(TypeError)
    lambda { "" + 65 }.should raise_error(TypeError)
  end

  it "doesn't return subclass instances" do
    (StringSpecs::MyString.new("hello") + "").class.should == String
    (StringSpecs::MyString.new("hello") + "foo").class.should == String
    (StringSpecs::MyString.new("hello") + StringSpecs::MyString.new("foo")).class.should == String
    (StringSpecs::MyString.new("hello") + StringSpecs::MyString.new("")).class.should == String
    (StringSpecs::MyString.new("") + StringSpecs::MyString.new("")).class.should == String
    ("hello" + StringSpecs::MyString.new("foo")).class.should == String
    ("hello" + StringSpecs::MyString.new("")).class.should == String
  end

  it "taints the result when self or other is tainted" do
    strs = ["", "OK", StringSpecs::MyString.new(""), StringSpecs::MyString.new("OK")]
    strs += strs.map { |s| s.dup.taint }

    strs.each do |str|
      strs.each do |other|
        (str + other).tainted?.should == (str.tainted? | other.tainted?)
      end
    end
  end
end
