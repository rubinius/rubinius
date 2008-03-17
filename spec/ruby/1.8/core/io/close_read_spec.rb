require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#close_read" do
  before :each do
    @io = IO.popen(RUBY_NAME, "r+")
  end
  after :each do
    @io.close unless @io.closed?
  end

  it "closes the read end of a duplex I/O stream" do
    @io.close_read
    lambda { @io.read() }.should raise_error(IOError)
    lambda { @io.sysread(1) }.should raise_error(IOError)
  end

  it "raises an IOError on subsequent invocations" do
    @io.close_read
    lambda { @io.close_read }.should raise_error(IOError)
    lambda { @io.close_read }.should raise_error(IOError)
  end

  it "allows subsequent invocation of close" do
    @io.close_read
    lambda { @io.close }.should_not raise_error
  end

  it "raises an IOError if the stream is not duplexed." do
    begin
      io_w = IO.popen(RUBY_NAME, "w")
      lambda { io_w.close_read }.should raise_error(IOError)
    ensure
      io_w.close
    end
  end
  
  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.close_read }.should raise_error(IOError)
  end
end
