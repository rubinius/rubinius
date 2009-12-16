# -*- encoding: utf-8 -*-
require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#sysseek on a file" do
  # TODO: This should be made more generic with seek spec
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
    @io.sysseek(10, IO::SEEK_CUR)
    @io.readline.should == "igne une.\n"
  end

  it "raises an error when called after buffered reads" do
    @io.readline
    lambda { @io.sysseek(-5, IO::SEEK_CUR) }.should raise_error(IOError)
  end

  it "warns if called immediately after a buffered IO#write" do
    begin
      # copy contents to a separate file
      tmpfile = File.open(tmp("tmp_IO_sysseek"), "w")
      tmpfile.write(@file.read)
      tmpfile.seek(0, File::SEEK_SET)

      tmpfile.write("abcde")
      lambda { tmpfile.sysseek(10) }.should complain(/sysseek/)
    ensure
      tmpfile.close
      File.unlink(tmpfile.path)
    end
  end

  it "moves the read position relative to the start with SEEK_SET" do
    @io.sysseek(42, IO::SEEK_SET)
    @io.readline.should == "quí está la línea tres.\n"
  end

  it "moves the read position relative to the end with SEEK_END" do
    @io.sysseek(1, IO::SEEK_END)

    # this is the safest way of checking the EOF when
    # sys-* methods are invoked
    lambda {
      @io.sysread(1)
    }.should raise_error(EOFError)

    @io.sysseek(-25, IO::SEEK_END)
    @io.sysread(7).should == "cinco.\n"
  end

  it "can handle any numerical argument without breaking and can seek past EOF" do
    @io.sysseek(1.2).should == 1
    @io.sysseek(2**10).should == 1024
    @io.sysseek(2**32).should == 4294967296
    lambda { @io.sysseek(2**128) }.should raise_error(RangeError)
  end

  it "raises IOError on closed stream" do
    lambda { IOSpecs.closed_file.sysseek(0) }.should raise_error(IOError)
  end
end
