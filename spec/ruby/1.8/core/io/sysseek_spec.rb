require File.dirname(__FILE__) + '/../../spec_helper'

describe "IO#sysseek on a file" do
  # TODO: This should be made more generic with seek spec
  before :each do
    @file = File.open(File.dirname(__FILE__) + '/fixtures/readlines.txt', 'r')
    @io = IO.open @file.fileno, 'r'
  end

  after :each do
    @file.close
  end

  it "moves the read position relative to the current position with SEEK_CUR" do
    @io.sysseek(10, IO::SEEK_CUR)
    @io.readline.should == "igne une.\n"
  end

  it "raises an error when called after buffered reads" do
    @io.readline
    lambda { @io.sysseek(-5, IO::SEEK_CUR) }.should raise_error(IOError)
  end

  it "moves the read position relative to the start with SEEK_SET" do
    @io.sysseek(42, IO::SEEK_SET)
    @io.readline.should == "quí está la línea tres.\n"
  end

  it "moves the read position relative to the end with SEEK_END" do
    @io.sysseek(0, IO::SEEK_END)
    @io.tell.should == 134
    @io.sysseek(-25, IO::SEEK_END)
    @io.readline.should == "cinco.\n"
  end

  it "can handle any numerical argument without breaking and can seek past EOF" do
    @io.sysseek(1.2).should == 1
    @io.sysseek(2**10).should == 1024
    @io.sysseek(2**32).should == 4294967296
    lambda { @io.sysseek(2**128) }.should raise_error(RangeError)
  end
end
