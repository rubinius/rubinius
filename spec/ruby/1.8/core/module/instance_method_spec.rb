require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Module#instance_method" do
  it "returns an UnboundMethod copy of the method with the given name" do
    c = Class.new do
      def test
        "test"
      end
    end

    meth = c.instance_method(:test)
    meth.class.should == UnboundMethod

    bound = meth.bind c.new
    bound.call.should == "test"

    meth.inspect.should =~ /#<UnboundMethod(:)? #{c}#test>/

    meth = c.instance_method("test")
    meth.inspect.should =~ /#<UnboundMethod(:)? #{c}#test>/
  end

  it "inspect displays it's module if that is where it is from" do
    meth = Object.instance_method("dup")
    meth.inspect.should =~ /#<UnboundMethod(:)? Object(\(Kernel\))?#dup>/
  end

  it "raises a TypeError if the given name is not a string/symbol" do
    lambda { Object.instance_method(nil)       }.should raise_error(TypeError)
    lambda { Object.instance_method(mock('x')) }.should raise_error(TypeError)
  end

  it "raises a NameError if the given method doesn't exist" do
    lambda { Object.instance_method(:missing) }.should raise_error(NameError)
  end
end
