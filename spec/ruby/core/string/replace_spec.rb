require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes.rb', __FILE__)

describe "String#replace" do
  it "replaces the content of self with other and returns self" do
    a = "some string"
    a.replace("another string").should equal(a)
    a.should == "another string"
  end

  it "replaces the taint status of self with that of other" do
    a = "an untainted string"
    b = "a tainted string".taint
    a.replace(b)
    a.tainted?.should == true
  end

  it "tries to convert other to string using to_str" do
    other = mock('x')
    other.should_receive(:to_str).and_return("converted to a string")
    "hello".replace(other).should == "converted to a string"
  end

  it "raises a TypeError if other can't be converted to string" do
    lambda { "hello".replace(123)       }.should raise_error(TypeError)
    lambda { "hello".replace([])     }.should raise_error(TypeError)
    lambda { "hello".replace(mock('x')) }.should raise_error(TypeError)
  end

  ruby_version_is ""..."1.9" do
    it "raises a TypeError on a frozen instance that is modified" do
      a = "hello".freeze
      lambda { a.replace("")      }.should raise_error(TypeError)
      lambda { a.replace("world") }.should raise_error(TypeError)
    end

    it "does not raise an exception on a frozen instance that would not be modified" do
      a = "hello".freeze
      a.freeze.replace(a).should equal(a)
    end
  end

  ruby_version_is "1.9" do
    it "raises a RuntimeError on a frozen instance that is modified" do
      a = "hello".freeze
      lambda { a.replace("")      }.should raise_error(RuntimeError)
      lambda { a.replace("world") }.should raise_error(RuntimeError)
    end

    # see [ruby-core:23666]
    it "raises a RuntimeError on a frozen instance that would not be modified" do
      a = "hello".freeze
      lambda { a.freeze.replace(a) }.should raise_error(RuntimeError)
    end
  end
end
