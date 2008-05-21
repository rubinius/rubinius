require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#close_read" do
  before(:each) do
    @io = StringIO.new("example")
    @io.close_read
  end

  it "prevents further reading" do
    @io.closed_read?.should == true
    @io.closed_write?.should == false
    lambda { @io.read(1) }.should raise_error(IOError)
  end

  it "allows further writing" do
    @io.write("x").should == 1
  end
end
