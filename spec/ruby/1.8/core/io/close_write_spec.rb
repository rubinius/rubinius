require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

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
    begin
     io_r = IO.popen("ls", "r")
     lambda { io_r.close_write }.should raise_error(IOError)
    ensure
      io_r.close
    end
  end

  it "flushes and closes the write stream" do
    @io.print("p 12345")
    @io.close_write
    @io.read.should == "12345\n"
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.close_write }.should raise_error(IOError)
  end
end
