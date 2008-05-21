require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#close_write" do
  before(:each) do
    @io = StringIO.new("example")
    @io.close_write
  end

  it "prevents further writing" do
    @io.closed_read?.should == false
    @io.closed_write?.should == true
    lambda { @io.write('x') }.should raise_error(IOError)
  end

  it "allows further reading" do
    @io.read(1).should == 'e'
  end
end
