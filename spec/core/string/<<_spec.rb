require File.dirname(__FILE__) + '/../../spec_helper'

describe "String#<<(obj)" do
  it "should return a new string containing obj concatenated to self" do
    a = 'hello ' << 'world'
    a.should == "hello world"
  end
  
  it "should convert obj to a string using to_str before concatenating" do
    module StringSpec
      class TestObject
        def to_str
          "world!"
        end
      end
    end
    
    a = 'hello ' << StringSpec::TestObject.new
    a.should == 'hello world!'
  end
  
  it "should raise a TypeError if obj can't be converted to a String" do
    module StringSpec
      class TestObject2
        # Does not implement to_str
      end
    end

    should_raise(TypeError) do
      a = 'hello ' << :world
    end

    should_raise(TypeError) do
      a = 'hello ' << Object.new
    end
    
    should_raise(TypeError) do
      a = 'hello ' << StringSpec::TestObject2.new
    end
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a << "test"
    end
  end
end

describe "String#<<(fixnum)" do
  it "should convert the fixnum to a char before concatenating" do
    b = 'hello ' << 'world' << 33
    b.should == "hello world!"
  end
  
  it "should raise a TypeError when fixnum is not between 0 and 255" do
    should_raise(TypeError) do
      "hello world" << 333
    end
  end

  it "should raise a TypeError when self is frozen" do
    should_raise(TypeError) do
      a = "hello"
      a.freeze
      a << 33
    end
  end
end

describe "String#concat" do
  it "should be an alias to String#<<" do
    # TODO:
  end
end
