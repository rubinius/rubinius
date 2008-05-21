require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#readline" do
  before(:each) do
    @io = StringIO.new("r e a d")
  end

  it "returns the next 'line'" do
    @io.readline.should == 'r e a d'
  end

  it "raises an EOFError at the end" do
    @io.readline
    lambda { @io.readline }.should raise_error(EOFError)
  end

  it "raises an IOError when it is not open for reading" do
    @io.close_read
    lambda { @io.readline }.should raise_error(IOError)
  end

  it "support separator strings" do
    @io.gets('e').should == 'r e'
    @io.gets('e').should == ' a d'
  end
end
