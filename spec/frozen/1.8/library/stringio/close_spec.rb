require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#close" do
  before(:each) do
    @io = StringIOSpecs.build
  end
  it "prevents further operations" do
    @io.close.should == nil
    @io.closed?.should == true
    @io.closed_read?.should == true
    @io.closed_write?.should == true
    lambda { @io << 'x' }.should raise_error(IOError)
  end
end
