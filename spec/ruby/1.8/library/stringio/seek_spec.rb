require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "StringIO#seek" do
  before(:each) do
    @io = StringIO.new("12345678")
  end

  it "seeks to an absolute position" do
    @io.seek(5).should == 0
    @io.read(1).should == '6'
  end

  it "seeks from the current position" do
    @io.read(1)
    @io.seek(1, IO::SEEK_CUR)
    @io.read(1).should == '3'
  end

  it "seeks from the end of the IO" do
    @io.seek(1, IO::SEEK_END)
    @io.read(1).should == nil
    @io.seek(-2, IO::SEEK_END)
    @io.read(1).should == '7'
  end

  it "can handle any numerical argument" do
    @io.seek(1.2).should == 0
    @io.seek(@io.string.size * 2).should == 0
    @io.seek(1.23423423432e5).should == 0
    @io.seek(0.00000000000000000000001).should == 0
    lambda { @io.seek(2**128) }.should raise_error(RangeError, /bignum too big/)
  end
end
