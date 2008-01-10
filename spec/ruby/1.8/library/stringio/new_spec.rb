require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO.new" do
  it "contains an empty string if no argument is provided" do
    StringIO.new.string.should == ""
  end

  it "calls to_str on the first argument if it is not a String" do
    obj = mock('hello')
    def obj.to_str; "hello"; end
    io = StringIO.new(obj)
    io.string.should == "hello"
  end

  it "raises a TypeError if the argument cannot be converted" do
    obj = mock('x')
    lambda { StringIO.new(obj) }.should raise_error(TypeError)
  end

  it "initializes in read-only mode when given a 'read' mode flag" do
    io = StringIO.new('hi', 'r')
    io.closed_write?.should == true
    io.closed_read?.should == false
    io = StringIO.new('bye', 'rb')
    io.closed_write?.should == true
    io.closed_read?.should == false
    lambda { io.write('!') }.should raise_error(IOError)
  end

  it "does not call to_str on String subclasses" do
    str = StringSubclass.new('keep')
    io = StringIO.new(str)
    io.string.class.should == StringSubclass
    io.write('!')
    io.string.class.should == StringSubclass
  end
end
