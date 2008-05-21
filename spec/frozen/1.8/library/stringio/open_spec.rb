require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO.open" do
  it "contains an empty string if no argument is provided" do
    StringIO.open.string.should == ""
  end

  it "yields the IO object to the block" do
    sio = nil
    StringIO.open("abc") do |io|
      io.string.should == 'abc'
      io.read(2).should == 'ab'
      io.closed?.should == false
      sio = io
    end
    sio.closed?.should == true
  end

  it "calls to_str on the first argument if it is not a String" do
    obj = mock('hello')
    def obj.to_str; "hello"; end
    StringIO.open(obj) do |io|
      io.string.should == "hello"
    end
  end

  it "raises a TypeError if the argument cannot be converted" do
    obj = mock('x')
    lambda { StringIO.open(obj) }.should raise_error(TypeError)
  end
end
