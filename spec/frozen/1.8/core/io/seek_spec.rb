require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#seek" do
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    # we *must* close both in order to not leak descriptors
    @io.close unless @io.closed?
    @file.close unless @file.closed? rescue Errno::EBADF
  end

  it "moves the read position relative to the current position with SEEK_CUR" do
    @io.seek(10, IO::SEEK_CUR)
    @io.readline.should == "igne une.\n"
    @io.seek(-5, IO::SEEK_CUR)
    @io.readline.should == "une.\n"
  end

  it "moves the read position relative to the start with SEEK_SET" do
    @io.seek(42, IO::SEEK_SET)
    @io.readline.should == "quí está la línea tres.\n"
    @io.seek(5, IO::SEEK_SET)
    @io.readline.should == " la ligne une.\n"
  end

  it "moves the read position relative to the end with SEEK_END" do
    @io.seek(0, IO::SEEK_END)
    @io.tell.should == 134
    @io.seek(-25, IO::SEEK_END)
    @io.readline.should == "cinco.\n"
  end

  it "can handle any numerical argument without breaking" do
    @io.seek(1.2).should == 0
    @io.seek(2**32).should == 0
    @io.seek(1.23423423432e12).should == 0
    @io.seek(0.00000000000000000000001).should == 0
    lambda { @io.seek(2**128) }.should raise_error(RangeError)
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.seek(0) }.should raise_error(IOError)
  end
end
