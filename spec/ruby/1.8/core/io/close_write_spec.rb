require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#close_write" do
  before :each do
    @io = IO.popen(RUBY_NAME, "r+")
  end
  after :each do
    @io.close unless @io.closed?
  end

  it "closes the write end of a duplex I/O stream" do
    @io.close_write
    lambda { @io.print("attempt to write") }.should raise_error(IOError)
    lambda { @io.syswrite("attempt to write") }.should raise_error(IOError)
  end

  it "raises an IOError on subsequent invocations" do
    @io.close_write
    lambda { @io.close_write }.should raise_error(IOError)
    lambda { @io.close_write }.should raise_error(IOError)
  end

  it "allows subsequent invocation of close" do
    @io.close_write
    lambda { @io.close }.should_not raise_error
  end

  it "raises an IOError if the stream is not duplexed." do
     @io = IO.popen("ls", "r")
     lambda { @io.close_write }.should raise_error(IOError)
  end

  it "flushes and closes the write stream" do
    @io.print("p 12345")
    @io.close_write
    @io.read.should == "12345\n"
  end
end
